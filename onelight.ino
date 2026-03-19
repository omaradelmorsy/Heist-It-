void setup() {
  pinMode(8, OUTPUT);
  pinMode(9, INPUT_PULLUP);
}

void loop() {
  int state = digitalRead(9);
  if (state == LOW){
    digitalWrite(8, HIGH);
    delay(1000);
    digitalWrite(8, LOW);
    delay(1000);  
  } 
  else {
    digitalWrite(8, LOW);}
}


// sara keriakes 
// Andrew White
