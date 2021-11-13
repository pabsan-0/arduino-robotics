/*
Basic motion test: Looped randomly chosen motion for a few seconds.
*/

#include <Stepper.h>

// Control variables
int spl_1;
int spl_2;
int starttime;
int endtime;

// 28BYJ-48 steps per rev at full step accounting gearbox 64 * 64
const int stepsPerRevolution = 4096; 

// Instancing stepper objects, mind the pin order!
Stepper myStepper_1(stepsPerRevolution, 9, 11, 10, 12);
Stepper myStepper_2(stepsPerRevolution, 5, 7, 6, 8);


void setup() {
  // Set base RPMs for each motor, stablest range: (1, 15)
  myStepper_1.setSpeed(5);
  myStepper_2.setSpeed(5);
}

void loop() {
  /*  Because we cannot move two motors simultaneously with Stepper.step, 
   *  we use this sneaky trick to alternate small motions in each motor, 
   *  giving the impression that they move at the same time.    
   */

  // Steps per loop, proportional to real output speed
  spl_1 = random(-5, 5);
  spl_2  = random(-5, 5);

  // Loop these alternated mini-motions for 1000 ms
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <=1000){
    myStepper_1.step(spl_1);
    myStepper_2.step(spl_2);
    endtime = millis();
    }
}
