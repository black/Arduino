void setup()
{
  pinMode(8, OUTPUT);
  Serial.begin(9600);
}
int potPin = 0, t;
void loop() {
  int val = 1;
  if (t > 200) {
    //for (int i = 0; i < 100; i++) {
      digitalWrite(8, HIGH);
      Serial.println("TEST");
      delayMicroseconds(val); // Approximately 10% duty cycle @ 1KHz
      digitalWrite(8, LOW);
      delayMicroseconds(4000-val);
    //}
  } else {
    digitalWrite(8, HIGH);
    Serial.println(t);
    t++;
  }
}

