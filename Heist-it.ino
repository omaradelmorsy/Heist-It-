//Some libraries I needed them 
#include <Wire.h>
#include <Servo.h>
#include <DFRobot_DFPlayerPro.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>


// Some global variables i needed (Placeholders not actual values)
#define GREEN_LED    0  // place holder TO TEST
#define RED_LED      1  // place holder
#define START_BTN    2  // place holder
#define REED         3   //place holder
#define SERVO_PIN    5    //place holder
#define VCNL_INT     6   //place holder

#define VCNL_ADDR    0x60   //place holder 
#define OLED_ADDR    0x3C   //Place holder

//Some Objects
Servo servo;
DFRobot_DFPlayerPro audio;
Adafruit_SSD1306 screen(128, 64, &Wire, -1);

int score     = 0;
long timeLeft = 5000;
bool playing  = false;

volatile int  dialPulses  = 0;
volatile bool gameStart   = false;
volatile bool fireHit     = false;

int dialBase = 0;

enum Move { DIAL, FIRE, JIGGLE };  //Three Commands


void onDialOrStart() {
  if (!playing) {
    gameStart = true;
  } else {
    dialPulses++;
  }
}

void onReed() {
  fireHit = true;
}


void greenOn()  {
     digitalWrite(GREEN_LED, HIGH); 
     digitalWrite(RED_LED, LOW); 
     }
void redOn()    { 
    digitalWrite(GREEN_LED, LOW);  
    digitalWrite(RED_LED, HIGH);
 }
void ledsOff()  { 
    digitalWrite(GREEN_LED, LOW);  
    digitalWrite(RED_LED, LOW); 
 }

void blink(bool green, int times, int ms) {
  for (int i = 0; i < times; i++) {
    green ? greenOn() : redOn();
    delay(ms);
    ledsOff();
    delay(ms);
  }
}


void play(int track) {
  audio.playFileNum(1, track);
}


void lock()   { 
    servo.write(0);  
    delay(300); 
}
void unlock() { 
    servo.write(90); 
    delay(300); 
}


void resetDial() {
  noInterrupts();
  dialBase = dialPulses;
  interrupts();
}

int dialCount() {
  noInterrupts();
  int n = dialPulses - dialBase;
  interrupts();
  return n;
}


void resetFire() {
  noInterrupts();
  fireHit = false;
  interrupts();
}


void i2cSend(uint8_t addr, uint8_t reg, uint8_t val) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

uint16_t i2cGet(uint8_t addr, uint8_t reg) {
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)2);
  uint16_t lo = Wire.read();
  uint16_t hi = Wire.read();
  return (hi << 8) | lo;
}


void setupVCNL() {
  i2cSend(VCNL_ADDR, 0x03, 0x00);
}

bool tooClose() {
  return i2cGet(VCNL_ADDR, 0x08) < 300;
}

bool fireDetected() {
  return fireHit || tooClose();
}


void showScore(int s) {
  screen.clearDisplay();
  screen.setTextColor(WHITE);
  screen.setTextSize(1);
  screen.setCursor(28, 2);
  screen.println("HEIST-IT!");
  screen.drawLine(0, 13, 127, 13, WHITE);
  screen.setTextSize(4);
  screen.setCursor(s < 10 ? 52 : s < 100 ? 34 : 16, 22);
  screen.println(s);
  screen.display();
}

void showMessage(const char* line1, const char* line2) {
  screen.clearDisplay();
  screen.setTextColor(WHITE);
  screen.setTextSize(2);
  int x1 = (128 - strlen(line1) * 12) / 2;
  int x2 = (128 - strlen(line2) * 12) / 2;
  screen.setCursor(max(0, x1), 10);
  screen.println(line1);
  screen.setCursor(max(0, x2), 34);
  screen.println(line2);
  screen.display();
}

void showCommand(const char* cmd) {
  screen.clearDisplay();
  screen.setTextColor(WHITE);
  screen.setTextSize(1);
  screen.setCursor(28, 2);
  screen.println("HEIST-IT!");
  screen.drawLine(0, 13, 127, 13, WHITE);
  screen.setTextSize(2);
  int x = (128 - strlen(cmd) * 12) / 2;
  screen.setCursor(max(0, x), 28);
  screen.println(cmd);
  screen.display();
}

void showWaiting() {
  screen.clearDisplay();
  screen.setTextColor(WHITE);
  screen.setTextSize(2);
  screen.setCursor(10, 4);
  screen.println("HEIST-IT!");
  screen.drawLine(0, 24, 127, 24, WHITE);
  screen.setTextSize(1);
  screen.setCursor(14, 34);
  screen.println("Press SW1 to");
  screen.setCursor(28, 46);
  screen.println("start game");
  screen.display();
}


Move pickMove() {
  return (Move)random(0, 3);
}

void giveCommand(Move m) {
  resetDial();
  resetFire();
  if (m == DIAL)   { 
    play(1); 
    showCommand("DIAL IT!");  
 }
  if (m == FIRE)   { 
    play(2); 
    showCommand("FIRE IT!");   
}
  if (m == JIGGLE) { 
    play(3); 
    showCommand("JIGGLE IT!"); 
}
}

bool gotItRight(Move m) {
  if (m == DIAL)   
  return dialCount() >= 5;
  if (m == FIRE)   
  return fireDetected();
  if (m == JIGGLE) 
  return dialCount() >= 20;
  return false;
}

bool gotItWrong(Move m) {
  if (m == DIAL)  
  return fireDetected();
  if (m == FIRE)   
  return dialCount() >= 5;
  if (m == JIGGLE) 
  return fireDetected();
  return false;
}

void win() {
  play(6);
  unlock();
  showMessage("YOU WIN!", "Diamond :)");
  blink(true, 10, 120);
  playing = false;
}

void lose() {
  play(5);
  lock();
  showMessage("CAUGHT!", "Game Over");
  blink(false, 6, 180);
  playing = false;
}


void setup() {
  Serial.begin(9600);

  pinMode(GREEN_LED,  OUTPUT);
  pinMode(RED_LED,    OUTPUT);
  pinMode(START_BTN,  INPUT_PULLUP);
  pinMode(REED,       INPUT);
  pinMode(VCNL_INT,   INPUT_PULLUP);
  ledsOff();

  attachInterrupt(digitalPinToInterrupt(START_BTN), onDialOrStart, FALLING);
  attachInterrupt(digitalPinToInterrupt(REED),      onReed,        RISING);

  servo.attach(SERVO_PIN);
  lock();

  Wire.begin();

  if (!screen.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED failed");
  } else {
    screen.clearDisplay();
    screen.display();
  }

  setupVCNL();

  if (!audio.begin()) {
    Serial.println("Audio failed");
  } else {
    audio.setVolume(25);
  }

  randomSeed(analogRead(A0));
  blink(true, 3, 100);
  showWaiting();
}


void loop() {
  if (!playing) {
    greenOn(); delay(400);
    ledsOff(); delay(400);

    if (gameStart) {
      delay(50);
      noInterrupts();
      gameStart = false;
      interrupts();

      score    = 0;
      timeLeft = 2000;
      lock();
      ledsOff();
      blink(true, 2, 150);
      delay(1200);
      showScore(0);
      playing = true;
    }
    return;
  }

  Move current = pickMove();
  giveCommand(current);

  unsigned long deadline = millis() + timeLeft;
  bool done = false;

  while (millis() < deadline) {
    if (gotItRight(current)) {
      score++;
      showScore(score);
      greenOn();
      play(4);
      delay(500);
      ledsOff();

      timeLeft -= 50;
      if (timeLeft < 400) timeLeft = 400;

      done = true;
      break;
    }

    if (gotItWrong(current)) {
      lose();
      done = true;
      break;
    }

    delay(20);
  }

  if (!done) {
    lose();
  }

  if (playing && score >= 99) {
    win();
  }

  if (!playing) {
    delay(3000);
    showWaiting();
  }
}