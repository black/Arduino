#include <ArduinoBLE.h> 
#include "LSM6DS3.h"
#include "Wire.h"

const int MAX_MESSAGE_LENGTH = 100;

// Create a BLE Service
BLEService sensorService("19B10000-E8F2-537E-4F6C-D104768A1212");

// Create a BLE Characteristic for the sensor data
BLEStringCharacteristic imuCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1212", BLERead | BLENotify, MAX_MESSAGE_LENGTH); 
// BLEStringCharacteristic microphoneCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1212", BLERead | BLENotify, MAX_MESSAGE_LENGTH);
// BLEStringCharacteristic temperatureCharacteristic("19B10003-E8F3-537E-4F6C-D104768A1212", BLERead | BLENotify, MAX_MESSAGE_LENGTH);

// Variables to store our simulated data 
float xa, ya, za, xg, yg, zg;
int temperature;
int microphone;

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A
 

void setup() {
  Serial.begin(9600);

  //while (!Serial);
  //Call .begin() to configure the IMUs
  if (myIMU.begin() != 0) {
    Serial.println("Device error");
  } else {
    Serial.println("Device OK!");
  }
  
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while (1);
  }

  // Set the local name peripheral advertises
  BLE.setLocalName("BLE IMU MICRO");
  
  // Set the UUID for the service
  BLE.setAdvertisedService(sensorService);

  // Add characteristics to the service 
  sensorService.addCharacteristic(imuCharacteristic);
  // sensorService.addCharacteristic(microphoneCharacteristic);
  // sensorService.addCharacteristic(temperatureCharacteristic);
  
  // Add the service
  BLE.addService(sensorService);

  // Start advertising
  BLE.advertise();
  
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLE.poll(); 

    // Generate IMU Accel
    xa = myIMU.readFloatAccelX()* 0.061;
    ya = myIMU.readFloatAccelY()* 0.061;
    za = myIMU.readFloatAccelZ()* 0.061;

    // Generate IMU GYRO
    xg = myIMU.readFloatGyroX()/ 17.5;
    yg = myIMU.readFloatGyroY()/ 17.5;
    zg = myIMU.readFloatGyroZ()/ 17.5; 

    float angle_xz = vector_2_degrees(xa, za);
    float angle_yz = vector_2_degrees(ya, za);

    // Generate Temprature 
    // Create separate data strings for each characteristic  
    String imuData = String(xa) + "]" + 
                     String(ya) + "]" + 
                     String(za) + "]" + 
                     String(angle_xz) + "]" + 
                     String(angle_yz) + "]" + 
                     String(zg); 
    
    // Write the value to the characteristic 
    imuCharacteristic.writeValue(imuData);
    // microphoneCharacteristic.writeValue(microphone); 
}


float vector_2_degrees(float x, float y) {
  float angle = atan2(y, x) * 180.0f / PI;
  if (angle < 0) {
    angle += 360;
  }
  return angle;
}
 