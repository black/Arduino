#include "MPU6050.h"

void MPU6050::initialise()
{
    Wire.setSDA(12);
    Wire.setSCL(13);
    Wire.begin();                        // Initialize comunication
    Wire.beginTransmission(MPU_ADDRESS); // Start communication with MPU6050 // MPU=0x68
    Wire.write(0x6B);                    // Talk to the register 6B
    Wire.write(0x00);                    // Make reset - place a 0 into the 6B register
    Wire.endTransmission(true);
    delay(100);
    wake_up();
    delay(20);
    calculate_IMU_error();
}

void MPU6050::wake_up()
{
    Wire.beginTransmission(MPU_ADDRESS); // Start communication with MPU6050 // MPU=0x68
    Wire.write(0x6B);                    // Talk to the register 6B
    Wire.write(0x01);                    // To wake up - place a 1 into the 6B register
    Wire.endTransmission(true);
}

void MPU6050::update_values()
{
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDRESS, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
    // For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
    acceleration[0] = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
    acceleration[1] = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
    acceleration[2] = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value

    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(0x43); // Gyro data first register address 0x43
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDRESS, 6, true);             // Read 4 registers total, each axis value is stored in 2 registers
    gyro[0] = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
    gyro[1] = (Wire.read() << 8 | Wire.read()) / 131.0;
    gyro[2] = (Wire.read() << 8 | Wire.read()) / 131.0;

    acceleration_angle[0] = (atan(acceleration[1] / sqrt(pow(acceleration[0], 2) + pow(acceleration[2], 2))) * 180 / M_PI) - acceleration_error[0];      // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
    acceleration_angle[1] = (atan(-1 * acceleration[0] / sqrt(pow(acceleration[1], 2) + pow(acceleration[2], 2))) * 180 / M_PI) - acceleration_error[1]; // AccErrorY ~(-1.58)

    previousTime = currentTime;                        // Previous time is stored before the actual time read
    currentTime = millis();                            // Current time actual time read
    elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds

    for (uint8_t i = 0; i < 3; i++)
    {
        gyro[i] = gyro[i] - gyro_error[i];
    }

    gyro_angle[0] = gyro_angle[0] + gyro[0] * elapsedTime; // deg/s * s = deg
    gyro_angle[1] = gyro_angle[1] + gyro[1] * elapsedTime;

    roll_pitch_yaw[2] = roll_pitch_yaw[2] + gyro[2] * elapsedTime;
    roll_pitch_yaw[0] = 0.96 * gyro_angle[0] + 0.04 * acceleration_angle[0];
    roll_pitch_yaw[1] = 0.96 * gyro_angle[1] + 0.04 * acceleration_angle[1];
}

void MPU6050::calculate_IMU_error()
{
    int c = 0;
    while (c < 200)
    {
        Wire.beginTransmission(MPU_ADDRESS);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU_ADDRESS, 6, true);
        acceleration[0] = (Wire.read() << 8 | Wire.read()) / 16384.0;
        acceleration[1] = (Wire.read() << 8 | Wire.read()) / 16384.0;
        acceleration[2] = (Wire.read() << 8 | Wire.read()) / 16384.0;
        acceleration_error[0] = acceleration_error[0] + ((atan((acceleration[1]) / sqrt(pow((acceleration[0]), 2) + pow((acceleration[2]), 2))) * 180 / M_PI));
        acceleration_error[1] = acceleration_error[1] + ((atan(-1 * (acceleration[0]) / sqrt(pow((acceleration[1]), 2) + pow((acceleration[2]), 2))) * 180 / M_PI));
        c++;
    }
    acceleration_error[0] = acceleration_error[0] / 200;
    acceleration_error[1] = acceleration_error[1] / 200;

    c=0;
    while (c < 200)
    {
        Wire.beginTransmission(MPU_ADDRESS);
        Wire.write(0x43);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU_ADDRESS, 6, true);
        gyro[0] = Wire.read() << 8 | Wire.read();
        gyro[1] = Wire.read() << 8 | Wire.read();
        gyro[2] = Wire.read() << 8 | Wire.read();
        // Sum all readings
        gyro_error[0] = gyro_error[0] + (gyro[0] / 131.0);
        gyro_error[1] = gyro_error[1] + (gyro[1] / 131.0);
        gyro_error[2] = gyro_error[2] + (gyro[2] / 131.0);
        c++;
    }
    for(int i=0;i<3;i++){
        gyro_error[i]=gyro_error[i]/200;
    }
}