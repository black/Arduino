#include <BLE52_Mouse_and_Keyboard.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial) delay(10);  // for nrf52840 with native usb
  Mouse.begin();
  Serial.print("Attempting to connect");
  uint8_t i = 0;
  while (!Mouse.isConnected()) {
    Serial.print(".");
    delay(100);
    if ((++i) > 50) {
      i = 0;
      Serial.println();
    }
  };
  delay(1000);  //just in case
  Serial.println("\nConnection successful");
}

void loop() {
  // put your main code here, to run repeatedly:
  Mouse.move(50,0);
}
