/*
 * Seeed XIAO BLE exercise
 * act as USBMouse,
 * read IMU LSM6DS3 and report mouse movement accordingly.
 */

#include "LSM6DS3.h"
#include "Wire.h"
#include "PluggableUSBHID.h"
#include "USBMouse.h"

USBMouse Mouse;

//Create a instance of class LSM6DS3
LSM6DS3 myIMU(I2C_MODE, 0x6A);    //I2C device address 0x6A

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    //while (!Serial);
    //Call .begin() to configure the IMUs
    if (myIMU.begin() != 0) {
        Serial.println("Device error");
    } else {
        Serial.println("Device OK!");
    }
}

float vector_2_degrees(float x, float y){
    float angle = atan2 (y, x) * 180.0f/PI;;

    if (angle < 0){
      angle += 360;
    }

    return angle;
}

void loop() {
    //Accelerometer
    float x, y, z;
    x = myIMU.readFloatAccelX();
    y = myIMU.readFloatAccelY();
    z = myIMU.readFloatAccelZ();

    float angle_xz = vector_2_degrees(x, z);
    float angle_yz = vector_2_degrees(y, z);

    Serial.print("angle_xz:");
    Serial.print(angle_xz);
    Serial.print(",");
    Serial.print("angle_yz:");
    Serial.println(angle_yz);

    int Xmove = 0;
    int Ymove = 0;
    //LEFT/RIGHT
    if (angle_xz > 120)
        Xmove = +3;
    else if (angle_xz > 105)
        Xmove = +1;
    else if (angle_xz < 60)
        Xmove = -3;
    else if (angle_xz < 75)
        Xmove = +1;
        
    //UP/DOWN
    if (angle_yz > 120)
        Ymove = -3;
    else if (angle_yz > 105)
        Ymove = -1;
    else if (angle_yz < 60)
        Ymove = +3;
    else if (angle_yz < 75)
        Ymove = +3;

    if ((Xmove!=0)||(Ymove!=0)){
      Mouse.move(Xmove,Ymove);
    }
        
    delay(50);
}