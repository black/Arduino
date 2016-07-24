void setup() {
  Serial.begin(9000);

}
int pinPort = 9, t = 0;
void loop() {
  int val = analogRead(0);
  int t = map(val, 0, 1024, 0, 255);
  Serial.println(int(t));
  // int t = map();
  //if (t > 500) {
  analogWrite(pinPort, int(t));
  //  } else {
  //    t++;
  //    Serial.println(t);
  //  }
}
