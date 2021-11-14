#include <Stepper.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// 1D Kalman filter library from https://github.com/bachagas/Kalman
#include <Kalman.h>;


//* Stepper config *//
// Control variables for movement
int stepsPerLoop_1;
int stepsPerLoop_2;
int starttime;
int endtime;
//
// Instancing stepper objects, mind the irregular pin order!
// 28BYJ-48 steps per rev at full step accounting gearbox 64 * 64 = 4096
Stepper myStepper_1(4096, 9, 11, 10, 12);
Stepper myStepper_2(4096, 5, 7, 6, 8);

//* Accelerometer config *//
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
float yaw_raw = 0;
float imu_last_read_time = 0;
float dt;

//* Ultrasonic sensor config*/
const int pingPin = A1; // Trigger Pin of Ultrasonic Sensor
const int echoPin = A0; // Echo Pin of Ultrasonic Sensor
float dist_raw;
float duration;

//* Kalman filters config */
// Instance of time-variant kf with default init values:
// {process noise cov, measurement noise cov, estimation error cov} = {Q, R, P, init_measurement} 
Kalman yaw_KalmanFilter(0.05, 0.05, 100, 0); 
float yaw_filtered = 0;
//
Kalman distance_KalmanFilter(0.05, 0.05, 100, 0); 
float dist_filtered = 0;



void setup() {
  
  // join I2C bus
  Wire.begin();

  // initialize serial communication & test devices
  Serial.begin(9600);
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  // Set base RPMs for each motor, stablest range: (1, 15)
  myStepper_1.setSpeed(5);
  myStepper_2.setSpeed(5);
  
  
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);
}


void loop() {
  // read indirect yaw measurements from IMU
  yaw_raw, yaw_filtered = readIMU();
  
  // read distance from ultrasonic sensor
  dist_raw, dist_filtered = readUltrasonic();

  // Serial display
  Serial.print(dist_raw); Serial.print("\t");
  Serial.print(dist_filtered); Serial.print("\t");
  Serial.print(yaw_raw); Serial.print("\t");
  Serial.println(yaw_filtered);

  delay(10);
}


int readUltrasonic() {
  /*
   * Ultrasonic sensors work by emitting a pulse for a certain amount
   * of time and checking how long it takes to come back.
   */
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pingPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  dist_raw = microsecondsToCentimeters(duration);
  dist_filtered = distance_KalmanFilter.getFilteredValue(dist_raw);
  return dist_raw, dist_filtered;
}

float microsecondsToCentimeters(float microseconds) {
   return microseconds / 29 / 2;
}


double readIMU() {
  /* 
   * Update the yaw's measurement from the accelerometer's gz.
   * Returns both filtered and unfiltered yaw estimates for comparison 
   * and filter tuning. 
   * Unit conversion ruled by I2C class defaults:  
   *    +-2g for accelerations
   *    +-250 deg/sec for gyros
   *    +- 32768 digital resolution of all measurements
   */
  // Update yaw estimate from gz and timestep, skip 1st iteration
  if (imu_last_read_time != 0){
    dt = (millis() - imu_last_read_time);
    yaw_raw = yaw_raw + (float)gz * dt / 1000;
    yaw_filtered = yaw_filtered + yaw_KalmanFilter.getFilteredValue((float)gz) * dt / 1000;
  }
  
  // Read IMU, store timestamp and convert to physical units
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  imu_last_read_time = millis();
  // ax = ax * (9.81/16384.0);     // m/s^2
  // ay = ay * (9.81/16384.0);     // m/s^2
  // az = az * (9.81/16384.0);     // m/s^2
  // gx = gx * (250.0/32768.0);    // º/s  
  // gy = gy * (250.0/32768.0);    // º/s  
  gz = gz * (250.0/32768.0);       // º/s  
  
  return yaw_raw, yaw_filtered;
}


void movement() {
  /*  Because we cannot move two motors simultaneously with Stepper.step, 
   *  we use this sneaky trick to alternate small motions in each motor, 
   *  giving the impression that they move at the same time.
   *  This shouldn't be isolated in a function but placed  as main routine.
   */
  stepsPerLoop_1 = random(-5, 5);
  stepsPerLoop_2 = random(-5, 5);
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <=1000){
    myStepper_1.step(stepsPerLoop_1);
    myStepper_2.step(stepsPerLoop_2);
    endtime = millis();
  }
}
