const int ledPin = 8;
const int buttonPin = 2;

void setup() {
  pinMode(8, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    digitalWrite(8, HIGH);
    delay(500);
    digitalWrite(8, LOW);
    delay(500);
  } else {
    digitalWrite(8, LOW);
  }
}