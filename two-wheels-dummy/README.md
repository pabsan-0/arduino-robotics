# Two wheels dummy

### Milestones
- [x] Assembly and test run 
- [ ] Obstacle avoidance in linear motion
- [ ] Obstacle avoidance in nonlinear motion
- [x] Wireless manual control


## Version 1. First build

### Overview:

<img src="https://user-images.githubusercontent.com/63670587/141654521-a040bb99-e218-456b-a573-680062324f18.png" alt="drawing" height="300"/>  <img src="https://user-images.githubusercontent.com/63670587/141654462-7668c230-0b05-4027-b2b4-52ad10a65646.png" alt="drawing" height="300"/> 

### Components:
- 3D printed hardware from https://www.thingiverse.com/thing:1230244.  
- Arduino Nano ATMEGA 328P  
- Inertial unit MPU 6050   
- Ultrasonic sensor HC-SR04  
- Stepper motor + drivers 28BYJ-48 ULN2003 (x2)  
- 5V 2A 5000mAh USB powerbank  
- Smaller electronics:
  - 10K resistor (x2)
  - Scrapped USB wire
  - Switch
  - 2A fuse   
  
### Schematics:
><img src="https://user-images.githubusercontent.com/63670587/141653853-ef24cca7-cc7e-48de-aaa1-870cefbc6e1e.png" alt="drawing" width="600"/>
  



## Version 2. Wireless control upgrade

### Overview:

<img src="https://user-images.githubusercontent.com/63670587/142772383-b2b5fba5-edd7-40c9-83e4-dd4ace7062e0.png" alt="drawing" height="350"/>  <img src="https://user-images.githubusercontent.com/63670587/142772391-022ecb7e-26da-4e8e-90cb-f0def1ce7f10.png" alt="drawing" height="350"/> 



### Components:
- All of the base components from first build 
- Android phone (must support OTG)
- Trailer hardware (simply to hold the phone)
- [Server Bridge X app](https://play.google.com/store/apps/details?id=com.cidtepole.serverbridge&hl=en&gl=US) to link server with phone's serial
- Wiring:
  - USB type C to USB
  - USB to mini USB

### Communication scheme:
![image](https://user-images.githubusercontent.com/63670587/142772078-97749bba-52f6-4aeb-907b-dcc90311a886.png)

### Practical usage guide
#### Setup:
- Start app 
- Plug Arduino into android
- Turn on powerbank to feed motors effectively
- Start Telnet server from app
- Connect to server
  - Bash telnet / Powershell Telnet (may require activation of Telnet in 'Turn Windows features on or off')
  - Python interface
    ```
    from telnetlib import Telnet
    with Telnet('192.168.1.10', '1234') as tn:
      tn.interact()
    ```

#### Interfacing and controls:
Commands are received in the Arduino via serial and parsed one character at a time. The telnet server only takes input from a remote terminal to the phone serial bus, so the input in the remote terminal is the same as it would be through a direct serial connection.
- `f [L] [R]`: set forward mode. Set relative number of revolutions between the left and right wheels to control steering.
- `r [L] [R]`: set reverse mode. Set relative number of revolutions between the left and right wheels to control steering.
- `s [rpm]`: set a base speed in RPM. The range 3-15 seems to work for the setup.
- `t [sec]`: Move in the specified direction for a number of seconds.
Example valid commands:
```
f11                   # Set straight forward direction.
f31                   # Set forward direction, left wheel spins 3 times for each 1 spin of the right one.
r11                   # Set straight reverse direction.
f11 t4 f31 t2 r11 t4  # Go straight for 4 seconds then steer for 2 seconds then reverse straight for 4 seconds.
f11s4t2s5r34t1        # It also works without spaces.
```

### Resources:

#### Other interfacing alternatives that were attempted but discarded...
- Accessing a terminal emulator in android and write straight to USB -> requires rooted phone
- Accessing a terminal emulator in SSH and write commands to text file, then have an app send them to serial
  - Creating such app with MIT App Inventor -> great idea, painful debugging 
  - Creating such app with Android Studio + libraries -> seemed quite promising, untested

#### Some tools and apps that were helpful through the process:
- [Server Bridge X](https://play.google.com/store/apps/details?id=com.cidtepole.serverbridge&hl=en&gl=US): To link the phone's serial to a Telnet server. This is our interfacing of choice.
- [Serial USB Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=en&gl=US): for straight serial communication android-arduino
- [SSH helper](https://play.google.com/store/apps/details?id=com.arachnoid.sshelper&hl=en&gl=US): for SSHing to the phone easily
- [Termux](https://termux.com/): capable bash terminal on android
- [MIT App inventor](https://appinventor.mit.edu/): GUI for creating apps easily. Its a pain to debug though (long compilation + Abstracted GUI)
