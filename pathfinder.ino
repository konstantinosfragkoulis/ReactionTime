const int NUM_BUTTONS = 2;
const int BTN1 = D0;
const int BTN2 = D1;
const int BUTTONS[NUM_BUTTONS] = {BTN1, BTN2};

const int NUM_LEDS = 4;
const int LED1 = D5;
const int LED2 = D4;
const int LED3 = D3;
const int LED4 = D2;
const int LEDS[NUM_LEDS] = {LED1, LED2, LED3, LED4};

const int DELAY_LOW = 600;
const int DELAY_HIGH = 1000;
const int IDLE_CYCLE_TIME = 1000;
const int RESULTS_TIMEOUT = 20000;

enum State {
  IDLE = 0,
  TESTING = 1,
  RESULT = 2
};

enum State curState;

unsigned long startTime;
unsigned long idleTime;
int idleLedOn;
bool testStarted;
unsigned long testStartTime;
long reactionTime;
unsigned long resultsTime;
long delays[5];

void setup() {
  for(int i = 0; i < NUM_BUTTONS; ++i) pinMode(BUTTONS[i], INPUT);
  for(int i = 0; i < NUM_LEDS; ++i) pinMode(LEDS[i], OUTPUT);

  startTime = millis();
  idleTime = startTime;
  idleLedOn = 0;
  testStarted = false;
  testStartTime = 0;
  reactionTime = 0;
  resultsTime = 0;
}

bool ledOn;
unsigned long lastFlash;
void flashLeds(int leds, unsigned long time, unsigned long interval) {
  if(time - lastFlash >= interval) {
    ledOn = !ledOn;
    lastFlash = time;
  }
  for(int i = 0; i < leds; ++i) digitalWrite(LEDS[i], HIGH & ledOn);
}

void toggleLeds(PinStatus *pinStatus) {
  for(int i = 0; i < NUM_LEDS; ++i) digitalWrite(LEDS[i], pinStatus[i]);
}

unsigned long lastCycleTime;
void idle() {
  if((millis() - idleTime) - lastCycleTime >= IDLE_CYCLE_TIME) {
    PinStatus pinStatus[NUM_LEDS] = {LOW, LOW, LOW, LOW};
    pinStatus[(idleLedOn++) % NUM_LEDS] = HIGH;
    toggleLeds(pinStatus);
    lastCycleTime = millis() - lastCycleTime;
  }

  if(digitalRead(BTN2) == HIGH) {
    testStarted = false;
    testStartTime = 0;
    reactionTime = 0;
    resultsTime = 0;
    curState = TESTING;
    lastCycleTime = 0;
    lastFlash = 0;
    ledOn = true;
  }
}

void testing() {
  if(!testStarted) {
    testStartTime = millis();
    delays[0] = random(150, 300);
    for(int i = 1; i < 5; ++i) delays[i] = random(DELAY_LOW, DELAY_HIGH) + delays[i-1];
    testStarted = true;
    lastFlash = 0;
    lastCycleTime = 0;
  }
  if(millis() - testStartTime < delays[0]) {
    for(int i = 0; i < NUM_LEDS; ++i) digitalWrite(LEDS[i], LOW);
  } else if(millis() - testStartTime < delays[1]) {
    PinStatus pinStatus[NUM_LEDS] = {LOW, LOW, LOW, HIGH};
    toggleLeds(pinStatus);
  } else if(millis() - testStartTime < delays[2]) {
    PinStatus pinStatus[NUM_LEDS] = {LOW, LOW, HIGH, HIGH};
    toggleLeds(pinStatus);
  } else if(millis() - testStartTime < delays[3]) {
    PinStatus pinStatus[NUM_LEDS] = {LOW, HIGH, HIGH, HIGH};
    toggleLeds(pinStatus);
  } else if(millis() - testStartTime < delays[4]){
    PinStatus pinStatus[NUM_LEDS] = {HIGH, HIGH, HIGH, HIGH};
    toggleLeds(pinStatus);
  } else {
    PinStatus pinStatus[NUM_LEDS] = {LOW, LOW, LOW, LOW};
    toggleLeds(pinStatus);
  }

  if(digitalRead(BTN1) == HIGH) {
      // User reacted
      reactionTime = millis() - delays[4] - testStartTime;
      resultsTime = millis();
      curState = RESULT;
    }
}

void result() {
  if(reactionTime <= 0) flashLeds(4, millis() - resultsTime, 100);      // Failed attempt
  else if(reactionTime <= 250) flashLeds(4, millis() - resultsTime, 500);
  else if(reactionTime < 350) flashLeds(3, millis() - resultsTime, 500);
  else if(reactionTime < 450) flashLeds(2, millis() - resultsTime, 500);
  else flashLeds(1, millis() - resultsTime, 500);

  if((digitalRead(BTN2) == HIGH) || millis() - resultsTime > RESULTS_TIMEOUT) curState = IDLE;
}

void loop() {
  switch(curState) {
    case IDLE:
      idle();
      break;
    case TESTING:
      testing();
      break;
    case RESULT:
      result();
      break;
  }

  delay(10);
}
