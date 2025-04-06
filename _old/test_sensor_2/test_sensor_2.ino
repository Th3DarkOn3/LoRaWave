#include "FastIMU.h"
#include <Wire.h>

#define IMU1_ADDR 0x68  // MPU 1 on Wire 0
#define IMU2_ADDR 0x68  // MPU 2 on Wire 0
#define IMU3_ADDR 0x69  // MPU 3 on Wire 1
//#define PERFORM_CALIBRATION //Comment to disable startup calibration

MPU6050 IMU1;
MPU6050 IMU2;
MPU6050 IMU3; 

// Currently supported IMUS: MPU9255 MPU9250 MPU6886 MPU6500 MPU6050 ICM20689 ICM20690 BMI055 BMX055 BMI160 LSM6DS3 LSM6DSL QMI8658

calData calib1 = {0};  //Calibration data
calData calib2 = {0};
calData calib3 = {0};

AccelData aData1;    //Sensor data
AccelData aData2;
AccelData aData3;
GyroData gData1;
GyroData gData2;
GyroData gData3;

void setup() {
  //Wire.begin(4,15); //SDA,SCL
  Wire.begin(21,22);
    
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  int err1 = IMU1.init(calib1, IMU1_ADDR);
  if (err1 != 0) {
    Serial.print("Error initializing IMU1: ");
    Serial.println(err1);
    while (true) {
      ;
    }
  }
  /*
  int err2 = IMU2.init(calib2, IMU2_ADDR);
  if (err2 != 0) {
    Serial.print("Error initializing IMU2: ");
    Serial.println(err2);
    while (true) {
      ;
    }
  }
  */
  
#ifdef PERFORM_CALIBRATION
  Serial.println("FastIMU calibration & data example");
  if (IMU.hasMagnetometer()) {
    delay(1000);
    Serial.println("Move IMU in figure 8 pattern until done.");
    delay(3000);
    IMU.calibrateMag(&calib1);
    Serial.println("Magnetic calibration done!");
  }
  else {
    delay(5000);
  }

  delay(5000);
  Serial.println("Keep IMU level.");
  delay(5000);
  IMU.calibrateAccelGyro(&calib1);
  Serial.println("Calibration done!");
  Serial.println("Accel biases X/Y/Z: ");
  Serial.print(calib1.accelBias[0]);
  Serial.print(", ");
  Serial.print(calib1.accelBias[1]);
  Serial.print(", ");
  Serial.println(calib1.accelBias[2]);
  Serial.println("Gyro biases X/Y/Z: ");
  Serial.print(calib1.gyroBias[0]);
  Serial.print(", ");
  Serial.print(calib1.gyroBias[1]);
  Serial.print(", ");
  Serial.println(calib1.gyroBias[2]);
  if (IMU.hasMagnetometer()) {
    Serial.println("Mag biases X/Y/Z: ");
    Serial.print(calib1.magBias[0]);
    Serial.print(", ");
    Serial.print(calib1.magBias[1]);
    Serial.print(", ");
    Serial.println(calib1.magBias[2]);
    Serial.println("Mag Scale X/Y/Z: ");
    Serial.print(calib1.magScale[0]);
    Serial.print(", ");
    Serial.print(calib1.magScale[1]);
    Serial.print(", ");
    Serial.println(calib1.magScale[2]);
  }
  delay(5000);
  IMU.init(calib1, IMU1_ADDRESS);
#endif

  //err = IMU.setGyroRange(500);      //USE THESE TO SET THE RANGE, IF AN INVALID RANGE IS SET IT WILL RETURN -1
  //err = IMU.setAccelRange(2);       //THESE TWO SET THE GYRO RANGE TO ±500 DPS AND THE ACCELEROMETER RANGE TO ±2g
  
  if (err1 != 0) {
    Serial.print("Error Setting range IMU1: ");
    Serial.println(err1);
    while (true) {
      ;
    }
  }

  Serial.println("Gyro1X;Gyro1Y;Gyro1Z;Accel1X;Accel1Y;Accel1Z;Gyro2X;Gyro2Y;Gyro2Z;Accel2X;Accel2Y;Accel2Z");
}

void loop() {
  IMU1.update();
  IMU2.update();
  
  // ANGLE
  IMU1.getAccel(&aData1);
  IMU2.getAccel(&aData2);
  /* Serial OUT
  Serial.print(aData1.accelX);
  Serial.print("\t");
  Serial.print(aData1.accelY);
  Serial.print("\t");
  Serial.print(aData1.accelZ);
  Serial.print("\t -- ");

  Serial.print(aData2.accelX);
  Serial.print("\t");
  Serial.print(aData2.accelY);
  Serial.print("\t");
  Serial.print(aData2.accelZ);
  Serial.print("\t");
  */

  // ACCELERATION
  IMU1.getGyro(&gData1);
  IMU2.getGyro(&gData2);
  /* Serial OUT
  Serial.print(gData1.gyroX);
  Serial.print("\t");
  Serial.print(gData1.gyroY);
  Serial.print("\t");
  Serial.print(gData1.gyroZ);
  Serial.print("\t");

  Serial.print(gData2.gyroX);
  Serial.print("\t");
  Serial.print(gData2.gyroY);
  Serial.print("\t");
  Serial.print(gData2.gyroZ);
  Serial.print("\t");
  */
  
  /*
  char buffer[40];
  sprintf(buffer, "The %d burritos are %d degrees F", 20, "pipi");
  Serial.println(buffer);
  */
  
  int temp = int(trunc(aData1.accelX*100.0));
  Serial.printf("%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d" ,  
                  int(aData1.accelX*100),  int(aData1.accelY*100),  int(aData1.accelZ*100), // GYRO  DATA IMU 1
                  int(gData1.gyroX),   int(gData1.gyroY),   int(gData1.gyroZ),    // ACCEL DATA IMU 1
                  int(aData2.accelX*100),  int(aData2.accelY*100),  int(aData2.accelZ*100), // GYRO  DATA IMU 2
                  int(gData2.gyroX),   int(gData2.gyroY),   int(gData2.gyroZ) );  // ACCEL DATA IMU 2
  
  Serial.println();
  delay(200);
}