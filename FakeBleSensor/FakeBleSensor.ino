#include <ArduinoBLE.h>
// Calculate maximum message length:
// - Identifiers (a,i,v,p): 4 bytes
// - 8 EEG values: each could be up to 4 digits + sign (±9999) = 5 bytes each = 40 bytes
// - 6 IMU values: each could be up to 4 digits + sign = 5 bytes each = 30 bytes
// - Battery voltage (4 digits with decimal): 5 bytes
// - Battery percentage (3 digits): 3 bytes
// - Separators (']'): 16 separators = 16 bytes
// - Total: 4 + 40 + 30 + 5 + 3 + 16 = 98 bytes
// - Round up to 100 for safety

const int MAX_MESSAGE_LENGTH = 100;

// Create a BLE Service
BLEService sensorService("19B10000-E8F2-537E-4F6C-D104768A1214");

// Create a BLE Characteristic for the sensor data
BLEStringCharacteristic eegCharacteristic("19B10002-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, MAX_MESSAGE_LENGTH);
BLEStringCharacteristic imuCharacteristic("19B10003-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, MAX_MESSAGE_LENGTH);
BLEStringCharacteristic batteryCharacteristic("19B10004-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, MAX_MESSAGE_LENGTH);

// Variables to store our simulated data
int eeg1, eeg2, eeg3, eeg4, eeg5, eeg6, eeg7, eeg8;
int imu1, imu2, imu3, imu4, imu5, imu6;
int batVoltage,batPercentage;

unsigned long previousMillis = 0;
const long interval = 10;  // Update interval (1 second)
float bat = 100;

void setup() {
  Serial.begin(9600);
  
  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("BLE initialization failed!");
    while (1);
  }

  // Set the local name peripheral advertises
  BLE.setLocalName("FakeSensor 3");
  
  // Set the UUID for the service
  BLE.setAdvertisedService(sensorService);

  // Add characteristics to the service
  sensorService.addCharacteristic(eegCharacteristic);
  sensorService.addCharacteristic(imuCharacteristic);
  sensorService.addCharacteristic(batteryCharacteristic);
  
  // Add the service
  BLE.addService(sensorService);

  // Start advertising
  BLE.advertise();
  
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLE.poll();
  if(bat>0)bat-=0.00005;
  unsigned long currentMillis = millis();
  
  // Check if it's time to update the values
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Generate random EEG values (-500 to 500 µV range)
    eeg1 = random(-500, 501);
    eeg2 = random(-500, 501);
    eeg3 = random(-500, 501);
    eeg4 = random(-500, 501);
    eeg5 = random(-500, 501);
    eeg6 = random(-500, 501);
    eeg7 = random(-500, 501);
    eeg8 = random(-500, 501);
    
    // Generate random IMU values (-180 to 180 degrees for example)
    imu1 = random(-180, 181);
    imu2 = random(-180, 181);
    imu3 = random(-180, 181);
    imu4 = random(-180, 181);
    imu5 = random(-180, 181);
    imu6 = random(-180, 181);
    
    // Generate random battery values
    batVoltage = abs(bat*3.0); // 3.0V to 4.0V
    batPercentage = abs(bat); // 0 to 100%
    
    // Create separate data strings for each characteristic
    String eegData = String(eeg1) + "]" + 
                     String(eeg2) + "]" + 
                     String(eeg3) + "]" + 
                     String(eeg4) + "]" + 
                     String(eeg5) + "]" + 
                     String(eeg6) + "]" + 
                     String(eeg7) + "]" + 
                     String(eeg8);
    
    String imuData = String(imu1) + "]" + 
                     String(imu2) + "]" + 
                     String(imu3) + "]" + 
                     String(imu4) + "]" + 
                     String(imu5) + "]" + 
                     String(imu6);
    
    String batteryData = String(batVoltage) + "]" + 
                         String(batPercentage);
    
    // Write the value to the characteristic
    eegCharacteristic.writeValue(eegData);
    imuCharacteristic.writeValue(imuData);
    batteryCharacteristic.writeValue(batteryData);
    
    // Print the data to Serial for debugging
    // Print the data to Serial for debugging
    Serial.println("Sent EEG: " + eegData);
    Serial.println("Sent IMU: " + imuData);
    Serial.println("Sent Battery: " + batteryData);
  }
}