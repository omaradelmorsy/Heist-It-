const int led1Pin = 8;

const int led2Pin = 7;

const int led3Pin = 6;

const int buttonPin = 2;

 

void setup() {

  pinMode(led1Pin, OUTPUT);

  pinMode(led2Pin, OUTPUT);

  pinMode(led3Pin, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);

}

 

void loop() {

  int blinking = digitalRead(buttonPin);

 

  if (blinking == LOW) {

    digitalWrite(led1Pin, HIGH);

    digitalWrite(led2Pin, LOW);

    digitalWrite(led3Pin, LOW);

    delay(400);

 

    digitalWrite(led1Pin, LOW);

    digitalWrite(led2Pin, HIGH);

    digitalWrite(led3Pin, LOW);

    delay(400);

 

    digitalWrite(led1Pin, LOW);

    digitalWrite(led2Pin, LOW);

    digitalWrite(led3Pin, HIGH);

    delay(400);

  } else {

    digitalWrite(led1Pin, LOW);

    digitalWrite(led2Pin, LOW);

    digitalWrite(led3Pin, LOW);

  }

}

