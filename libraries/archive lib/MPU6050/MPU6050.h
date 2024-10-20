#ifndef __MPU6050__
#define __MPU6050__

// #include "pico/stdlib.h"
// #include "pico/binary_info.h"
// #include "hardware/i2c.h"
//  #include <Arduino.h>
#include <math.h>
#include <stdint.h>
#include <Wire.h>

#define I2C_SDA 12
#define I2C_SCL 13

#define ACC_SCALING_FACTOR 16384.0
#define GYRO_SCALING_FACTOR 131.0
#define MPU_ADDRESS 0x68

class MPU6050{
    private:    
    
    public:
    double acceleration[3];
    double gyro[3];
    double acceleration_angle[2];
    double gyro_angle[2];
    double roll_pitch_yaw[3];
    double acceleration_error[2];
    double gyro_error[3];
    double elapsedTime, currentTime, previousTime;
    
    void initialise();
    void update_values();
    void wake_up();
    void calculate_IMU_error();
};

#endif
