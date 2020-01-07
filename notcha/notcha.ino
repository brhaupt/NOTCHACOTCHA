//************************************************************
// NOTCHA-COTCHA CODE
//************************************************************

#include <Wire.h>
#include <LOLIN_I2C_BUTTON.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
int  displayLength, minX;
#define BUFFDSIZE 200
char message[BUFFDSIZE];

int pulse = 1; // pulse size length in microseconds
int choice = 0;  // select laser gun model type
int lastchoice = 0; // recall last selection choice
bool fire = false; // trigger fire button
int LIDAR_EMITTER = 13;
int TRIGGER_BUTTON = 11;
int CHANGE_MODE_BUTTON = 12;
int LEDARRAY = 0;
int LEDB = 2;
int lidar_count = 0;

void (*lidar_functions[])() = {ultralyteNonLR, ultralyteRev1, jenoptikLaserPL, kustomProlaser3,
                               jenoptikLaveg, kustomProlaser1, ultralyteRev2, stalkerLZ1,
                               kustomProlaser2, laserAtlanta, laserAtlantaStealthMode, kustomProLite};

void setup() {
  lidar_count = (int)(sizeof(lidar_functions)/sizeof(lidar_functions[0])); // store number of lidar systems defined
  
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  pinMode(LIDAR_EMITTER, OUTPUT);  // 940nM LED output (mimic 904nM laser diode)
  pinMode(CHANGE_MODE_BUTTON, INPUT);
  pinMode(TRIGGER_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(CHANGE_MODE_BUTTON), changeMode, FALLING); // attach interrupts to input pins
  attachInterrupt(digitalPinToInterrupt(CHANGE_MODE_BUTTON), fireJammer, RISING);  // instead of handling in loop
  Serial.println("start");

  setLaserName("Power On");
  displayLength = display.width();
  minX = -12 * strlen(message);
  displayData();
}

void loop() {
  displayData();
  (*lidar_functions[choice])();
}

void basicDelayEmitter(int delay_us, int pulse_count=3, bool double_pulse=false, int half_delay_2=12603) {
  if (fire) {
    for (int a = 1; a <= pulse_count; a++) {
      digitalWrite(LIDAR_EMITTER, HIGH);
      delayMicroseconds(pulse);
      digitalWrite(LIDAR_EMITTER, LOW);
      delayMicroseconds(delay_us);
      if (double_pulse){
        digitalWrite(LIDAR_EMITTER, HIGH);
        delayMicroseconds(pulse);
        digitalWrite(LIDAR_EMITTER, LOW);
        delayMicroseconds(half_delay_2);
        delayMicroseconds(half_delay_2);
      }
    }
    fire = false;
  }
}
void ultralyteNonLR() {
  setLaserName("Ultralyte Non-LR"); // 100 pulses per second
  basicDelayEmitter(9999);
}

void ultralyteRev1(){
  setLaserName("Ultralyte Rev.1"); // 100pps
  basicDelayEmitter(9999);
}

void jenoptikLaserPL() {
  setLaserName("Jenoptik LaserPL");// 100pps
  basicDelayEmitter(9999);
}

void kustomProlaser3() {
  setLaserName("Kustom Prolaser3");// 200 pps
  basicDelayEmitter(4999);
}

void jenoptikLaveg() {
  setLaserName("Jenoptik Laveg"); // 600pps
  basicDelayEmitter(1666);
}

void kustomProlaser1() {
  setLaserName("Kustom Prolaser1"); // 380pps
  basicDelayEmitter(2631);
}

void ultralyteRev2() {
  setLaserName("Ultralyte Rev.2");  // 125 pps
  basicDelayEmitter(8000);
}

void stalkerLZ1() {
  setLaserName("Stalker LZ-1");  // 130pps
  basicDelayEmitter(7691);
}

void kustomProlaser2() {
  setLaserName("Kustom Prolaser2");  // 238pps
  basicDelayEmitter(4201);
}

void laserAtlanta() {
  setLaserName("Laser Atlanta");  // 238pps
  basicDelayEmitter(4201);
}

void kustomProLite() {
  setLaserName("Kustom ProLite");  // 200 pps
  basicDelayEmitter(4999);
}

void laserAtlantaStealthMode() {
  setLaserName("Laser Atlanta Stealth Mode");  // 238pps  // 2 pulses fire followed by 5 missing pulses
  basicDelayEmitter(4201, 2, true, 12601);
}

ICACHE_RAM_ATTR void changeMode() {
  choice = (choice + 1) % lidar_count;

  if (choice != lastchoice) {
    // clear display only if it's different
    displayLength = display.width();
  }
  lastchoice = choice;
}

ICACHE_RAM_ATTR void fireJammer() {
  Serial.print("fired ");
  Serial.println(choice);
  display.setTextSize(1.5);
  display.println("      ...FIRE...");
  
  fire = true;
}

void setLaserName(char* name) {
  sprintf(message, "%s", name);
  //displayLength = display.width();
}

void clearBUFFD() { //just clear the data buffer
  memset(message, 0x00, BUFFDSIZE);
}

void displayData() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setTextWrap(false);
  display.setCursor(displayLength, 00);
  display.println(message);
  display.display();

  if (--displayLength < minX) displayLength = display.width();
}
