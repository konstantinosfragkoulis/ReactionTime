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

enum State {
  IDLE = 0,
  TESTING = 1,
  RESULT = 2
};

enum State curState;

void setup() {
  for(int i = 0; i < NUM_BUTTONS; ++i) pinMode(BUTTONS[i], INPUT);
  for(int i = 0; i < NUM_LEDS; ++i) pinMode(LEDS[i], OUTPUT);

  unsigned long startTime = millis();
}

bool ledOn;
void flashLeds(int leds, unsigned long time, unsigned long interval) {
  if(time % interval == 0) {
    if(ledOn) ledOn = false;
    else ledOn = true;
  }

  for(int i = 0; i < leds; ++i) digitalWrite(LEDS[i], HIGH & ledOn);
}

bool testStarted = false;
unsigned long testStartTime;
unsigned long reactionTime;
unsigned long resultsTime;

void loop() {
  switch(curState) {
  case IDLE:
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);

    if(digitalRead(BTN2) == HIGH) {
      testStarted = false;
      testStartTime = 0;
      reactionTime = 0;
      resultsTime = 0;
      curState = TESTING;
    }
  case TESTING:
    if(!testStarted) {
      testStartTime = millis();
      testStarted = true;
    }
    if(millis() - testStartTime < 200) {
      for(int i = 0; i < NUM_LEDS; ++i) digitalWrite(LEDS[i], LOW);
    } else if(millis() - testStartTime < 1000) {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      digitalWrite(LED4, HIGH);
    } else if(millis() - testStartTime < 1800) {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
    } else if(millis() - testStartTime < 2600) {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
      digitalWrite(LED4, HIGH);
    } else if(millis() - testStartTime < 3400){
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
      digitalWrite(LED4, LOW);
      
      if(digitalRead(BTN1) == HIGH) {
        // User reacted
        reactionTime = millis() - 3400 - testStartTime;
        resultsTime = millis();
        curState = RESULT;
      }
    }
  case RESULT:
    if(reactionTime <= 250) flashLeds(4, millis() - resultsTime, 250);
    else if(reactionTime < 350) flashLeds(3, millis() - resultsTime, 250);
    else if(reactionTime < 450) flashLeds(2, millis() - resultsTime, 250);
    else flashLeds(1, millis() - resultsTime, 250);

    if(digitalRead(BTN2) == HIGH) curState = IDLE;
  }
}
