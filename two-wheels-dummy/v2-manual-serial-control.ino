/*
Manual control through serial connection, remote-ready.

Controls:
f [L] [R]: set forward mode. Set relative number of revolutions between the left and right wheels to control steering.
r [L] [R]: set reverse mode. Set relative number of revolutions between the left and right wheels to control steering.
s [rpm]: set a base speed in RPM. The range 3-15 seems to work nice for our setup.
t [sec]: Move in the specified direction for a number of seconds. 

Example valid serial commands:
f11                   // Set straight forward direction.
f31                   // Set forward direction, left wheel spins 3 times for each 1 spin of the right one.
r11                   // Set straight reverse direction.
f11 t4 f31 t2 r11 t4  // Go straight for 4 seconds then steer for 2 seconds then reverse straight for 4 seconds.
f11s4t2s5r34t1        // It also works without spaces.

Possible future improvements: 
  - Always do same ministeps for both wheels but change the speeds. May be unstable sometimes but less bumpy.
  - Rearrange some code into functions for readability on next review (if it isnt broken dont fix it)
*/

#include <Stepper.h>

// Serial control variables
char serial_incoming;
int two_digits_num;
int first_digit = 1;
int secnd_digit = 1;

// Total movement time + slaves
int action_time_sec;
int starttime;
int endtime;

// Absolute speed and relative wheel steps per loop
int speed_rpm = 5;
int spl_left = 1;
int spl_right = 1;

// 28BYJ-48 steps per rev at full step accounting gearbox 64 * 64
const int stepsPerRevolution = 4096; 

// Instancing stepper objects, mind the pin order!
Stepper myStepper_left(stepsPerRevolution, 9, 11, 10, 12);
Stepper myStepper_right(stepsPerRevolution, 5, 7, 6, 8);

// This exclusively for debugging
bool verbose = false;


void setup() {
    // Boot serial
    Serial.begin(9600);
    Serial.println("Serial initialized. Dummy-two-wheels ready!");

    // Set base RPMs for each motor, stablest range: (1, 15)
    myStepper_left.setSpeed(speed_rpm);
    myStepper_right.setSpeed(speed_rpm);
}


void loop() {
    // 'Wait' for serial buffer to have something
    if (Serial.available() > 0){
        serial_incoming = Serial.read();

        // Interpreter. Expects something like 'f13r11s5t1'
        switch (serial_incoming) {
          
            case 'f':
                // Forward gear, parse next digits as relative speeds
                two_digits_num = Serial.parseInt();
                first_digit = two_digits_num / 10;
                secnd_digit = two_digits_num % 10;
                
                // Load relative steps here
                spl_left = first_digit;
                spl_right = secnd_digit;
        
                // Debugging
                if (verbose){
                    Serial.println(two_digits_num);
                    Serial.println(spl_left);
                    Serial.println(spl_right);
                }
            break;

            case 'r':
                // Reverse gear, parse next digits as relative speeds
                two_digits_num = Serial.parseInt();
                first_digit = two_digits_num / 10;
                secnd_digit = two_digits_num % 10;
                
                // Load relative steps here
                spl_left = -first_digit;
                spl_right = -secnd_digit;
                
                // Debugging
                if (verbose){
                Serial.println(two_digits_num);
                Serial.println(-spl_left);
                Serial.println(-spl_right);
                }
            break;
  
            case 's':
                // Set base speed in RPM for both motors
                speed_rpm = Serial.parseInt();
                myStepper_left.setSpeed(speed_rpm);
                myStepper_right.setSpeed(speed_rpm);
                
                // Debugging
                if (verbose){
                Serial.println(speed_rpm);
                }
                break;  

            case 't':
                // Read action seconds and move during that time
                action_time_sec = Serial.parseInt();
                
                // Loop alternated mini-motions for 1000 ms
                starttime = millis();
                endtime = starttime;
                while ((endtime - starttime) <= action_time_sec*1000){
                    myStepper_left.step(spl_left);
                    myStepper_right.step(spl_right);
                    endtime = millis();
                } 
            break;

            case 'd':
                // Display current config
                Serial.println("Steps per loop left wheel: " + String(spl_left));
                Serial.println("Steps per loop right wheel: " + String(spl_right));
                Serial.println("Default speed [rpm]: " + String(speed_rpm));
            break;

            default:
                // Do nothing if something weird is introduced
                if(verbose) {Serial.println("Catched void case");}
            break;
        }
    }
    delay(10);
    
    /* // This is useful but a pain to have in Telnet because it bloats the CLI
    Serial.print(spl_left);
    Serial.print(spl_right);
    Serial.println(speed_rpm);
    */
}
