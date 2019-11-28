class Player{
  private:
    int buttonState,
        lastButtonState,
        buttonPin;
  public:
    int points;
    boolean chance = true;
    Player(int pin){
      buttonState = digitalRead(pin);
      lastButtonState = buttonState; 
      buttonPin = pin;
      points = 0;
    }
    
  boolean buttonRead(int delayValue){
    buttonState = digitalRead(buttonPin);
    if (buttonState!=lastButtonState && this->chance){
      delay(delayValue);
      lastButtonState = buttonState;
      this->chance = false;
      return true;
    }
    return false;
  }
};

const int p1Pin = 13,
          p2Pin = 12;

const int serialDataPin = 11,
          latchPin = 10,
          clockPin = 9;

const int sound = 8;

void setup() {
  pinMode(p1Pin, INPUT_PULLUP);
  pinMode(p2Pin, INPUT_PULLUP);
  pinMode(serialDataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(sound, OUTPUT);
  startAnimation();
  Serial.begin(9600);
}

Player *p1 = new Player(p1Pin);
Player *p2 = new Player(p2Pin);

const int ledsAmount = 16;
int resolution = 10;
const int numberTranslation[]={126, 18, 188, 182, 210, 230, 238, 50, 254, 246, 1, 0};
const int maxPoints = 5;

void loop() {
  if(p1->points < maxPoints && p2->points < maxPoints){
    p1Leds(readSpeed(), 1);
  }
  if(p2->points < maxPoints && p1->points < maxPoints){
    p2Leds(readSpeed(), 15);
  }
  
  if(p1->points == maxPoints){
    delay(300);
    p2->points=11;
    resolution = 20;
    p1EndAnimation(1, 13);
  }
  if(p2->points == maxPoints){
    delay(300);
    p1->points=11;
    resolution = 20;
    p2EndAnimation(1, 3);
  }
}



float readSpeed(){
  return (analogRead(A0)/32);
}

void playPointSound(){
  tone(sound, 1600);
  delay(200);
  tone(sound, 1800);
  delay(200);
  noTone(sound);
}

void playStartingMusic(){
  tone(sound, 1400);
  delay(200);
  tone(sound, 1600);
  delay(200);
  tone(sound, 1800);
  delay(200);
  noTone(sound);
}

void p1Leds(float delVal, int startingLed){
  for (int i=startingLed;i<ledsAmount;i++){
    writeOut(i, false);
    for (int j=0;j<resolution;j++){
      p1->buttonRead(1);
      delay(delVal);
      if (p1->buttonRead(1)){
        if (i==ledsAmount-1){
          p1->points++;
          playPointSound();
          delay(50+analogRead(A0)/8);
        }else{
          p2Leds(readSpeed(), i+1);
          i=0;
        }
      }
    }
  p1->chance=true;
  }
}

void p2EndAnimation(float delVal, int startingLed){
  for (int i=startingLed;i<ledsAmount;i++){
    writeOut(i, true);
    for (int j=0;j<resolution;j++){
      delay(delVal);
    }
  }
}

void p2Leds(float delVal, int startingLed){
  for (int i=startingLed;i>0;i--){
    writeOut(i-1, false);
    for (int j=0;j<resolution;j++){
      p2->buttonRead(1);
      delay(delVal);
      if (p2->buttonRead(1)){
        if(i==1){
          p2->points++;
          playPointSound();
          delay(50+analogRead(A0)/8);
        }else{
          p1Leds(readSpeed(), i);
          i=16;
        }
      }
    }
  p2->chance=true;
  }
}

void p1EndAnimation(float delVal, int startingLed){
  for (int i=startingLed;i>0;i--){
    writeOut(i-1, true);
    for (int j=0;j<resolution;j++){
      delay(delVal);
    }
  }
}

void writeOut(int currentLed, boolean nWordPass){
  if ((p1->points < maxPoints && p2->points < maxPoints) || nWordPass){
    int bits = 0;
    bitSet(bits, currentLed);
    int bitPointsP1 = numberTranslation[p1->points];
    int bitPointsP2 = numberTranslation[p2->points];
    
    digitalWrite(latchPin, LOW);
    shiftOut(serialDataPin, clockPin, LSBFIRST, bitPointsP1);
    shiftOut(serialDataPin, clockPin, LSBFIRST, bitPointsP2);
    shiftOut(serialDataPin, clockPin, LSBFIRST, bits);
    shiftOut(serialDataPin, clockPin, LSBFIRST, (bits >> 8));
    digitalWrite(latchPin, HIGH);
  }else{
    if(p1->points == maxPoints){
      delay(300);
      p2->points=11;
      resolution = 20;
      p1EndAnimation(1, 13);
    }
    if(p2->points == maxPoints){
      delay(300);
      p1->points=11;
      resolution = 20;
      p2EndAnimation(1, 3);
    }
  }
}

void startAnimation(){
  for(int i=0;i<8;i++){
    int x=0;
    bitSet(x, i);
    digitalWrite(latchPin, LOW);
    shiftOut(serialDataPin, clockPin, LSBFIRST, x);
    shiftOut(serialDataPin, clockPin, LSBFIRST, x);
    shiftOut(serialDataPin, clockPin, MSBFIRST, x);
    shiftOut(serialDataPin, clockPin, LSBFIRST, x);
    digitalWrite(latchPin, HIGH);
    delay(50);
  }
  for(int i=0;i<8;i++){
    int x=0;
    bitSet(x, i);
    digitalWrite(latchPin, LOW);
    shiftOut(serialDataPin, clockPin, LSBFIRST, x);
    shiftOut(serialDataPin, clockPin, LSBFIRST, x);
    shiftOut(serialDataPin, clockPin, LSBFIRST, x);
    shiftOut(serialDataPin, clockPin, MSBFIRST, x);
    digitalWrite(latchPin, HIGH);
    delay(50);
  }
  delay(100);
  playStartingMusic();
  delay(200);
}
