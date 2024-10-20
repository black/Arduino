/*
https://github.com/csash7/mbed-BLE-Mouse
*/

#include <HIDMouse.h>

HIDMouse bleMouse;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  bleMouse.setDeviceName("WearMouse");
  bleMouse.setManufacturerName("Indic Origin");
  bleMouse.setBatteryLevel(75);
  bleMouse.begin();
}

void loop() {
  while (bleMouse.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH);
    bleMouse.move(0, 0, 1);
    delay(1000);
  } 
  digitalWrite(LED_BUILTIN, LOW);
}