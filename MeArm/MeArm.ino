/////////////////////////////////////////////////////////////////////////
///  \file MeArm.ino
///  \brief Control a MeArm robot arm with an Arduino, Arduino USB Host
///         Shield, and a Logitech F310 gamepad
///
///  \author Alan Ford
/////////////////////////////////////////////////////////////////////////

// Pins used by the Arduino USB Host Shield (via the SPI bus - also known as the ICSP header)
// Uno digital pins 10, 11, 12, and 13
// Mega digital pins 10, 50, 51, and 52
// Also pins 7, 8, 9, and 10

// Servo Control Pins
// Motor        Pin
//  Base         2
//  Shoulder     3
//  Elbow        4
//  Gripper      5
#define BASEPIN     2
#define SHOULDERPIN 3
#define ELBOWPIN    4
#define GRIPPERPIN  5

#define BASEHOME        90  // The arm should be pointed toward the front
#define SHOULDERHOME    90  // The shoulder should be pointed upward
#define ELBOWHOME       90  // The elbow should be horizontal
#define GRIPPERHOME     90  // gripper tongs should be at -45 and +45 degrees

#define BASEOFFSET 10
#define SHOULDEROFFSET  10

#define BASEMIN  10
#define BASEMAX  170

#define SHOULDERMIN  10
#define SHOULDERMAX  170

#define ELBOWMIN  80
#define ELBOWMAX  170

#define GRIPPERMIN  73
#define GRIPPERMAX  95


// the angle macro maps the range 0-255 to the range 0-180
// the reverseAngle macro maps the range 0-255 to the range 180-0
// the trimAngle macro trims an angle in the range of 0-180 to a range of MIN-MAX
#define angle(JOYSTICK) (((JOYSTICK)*180)/255)  //converts joystick input into servo angle
#define reverseAngle(JOYSTICK) (((255-(JOYSTICK))*180)/255)
#define trimAngle(ANGLE,MIN,MAX) ((ANGLE)<(MIN)?(MIN):((ANGLE)>(MAX)?(MAX):(ANGLE)))

// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <SPI.h>
#include "lf310.h"
//#include <Servo.h>
#include <ServoEasing.h>


USB Usb;
LF310 lf310(&Usb);
Servo baseServo;  // create servo object to control a servo
Servo shoulderServo;  // create servo object to control a servo
Servo elbowServo;  // create servo object to control a servo
Servo gripperServo;  // create servo object to control a servo

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the Arduino and the Arduino USB Host Shield
///////////////////////////////////////////////////////////////////////////////
void setup() {
        Serial.begin(115200);
#if !defined(__MIPSEL__)
        while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
        Serial.println("Starting Logitech F310 gamepad");

        if (Usb.Init() == -1)
                Serial.println("OSC did not start.");
                
        // Set this to higher values to enable more debug information
        // minimum 0x00, maximum 0xff, default 0x80
        // UsbDEBUGlvl = 0xff;

        delay(200);
    
        baseServo.attach(BASEPIN);      // attaches the base servo to the servo object
        shoulderServo.attach(SHOULDERPIN);  // attaches the shoulder servo to the servo object
        elbowServo.attach(ELBOWPIN);     // attaches the elbow servo to the servo object
        gripperServo.attach(GRIPPERPIN);   // attaches the gripper servo to the servo object
    
        // sets the servo position according to the scaled value
        baseServo.write(BASEHOME+BASEOFFSET);
        shoulderServo.write(SHOULDERHOME+SHOULDEROFFSET);
        elbowServo.write(ELBOWHOME);
        gripperServo.write(GRIPPERHOME);
        // waits 15 milliseconds for the servo to get there
        delay(15);

}

/*
 * These four variable hold the "old" values of the servo positions
 * only if they change.
 */
   uint8_t oldBase = BASEHOME;
   uint8_t oldShoulder = SHOULDERHOME;
   uint8_t oldElbow = ELBOWHOME;
   uint8_t oldGripper = GRIPPERHOME;
   uint8_t base;
   uint8_t shoulder;
   uint8_t elbow;
   uint8_t gripper;

void loop() {

    Usb.Task();
    uint8_t newAngle;
    if (lf310.connected()) {

        // move the base servo if needed
        newAngle = reverseAngle(lf310.lf310Data.X)+BASEOFFSET;  // map gamepad output (0-255) to servo angle (0-180)
        newAngle = trimAngle(newAngle,BASEMIN,BASEMAX);       // restrict servo to avoid grinding or lockup
        if (abs(newAngle - oldBase) > 5) {    // implement +/- 5 degree deadband to avoid oscillating
            base = newAngle;
            baseServo.write(base);
            oldBase = base;
            //Serial.print("newAngle: ");
            //Serial.print(newAngle);
            //Serial.print(", base: ");
            //Serial.println(base);
        }
        
        // move the shoulder servo if needed
        newAngle = reverseAngle(lf310.lf310Data.Y)+SHOULDEROFFSET;  // map gamepad output (0-255) to servo angle (0-180)
        newAngle = trimAngle(newAngle,SHOULDERMIN,SHOULDERMAX);       // restrict servo to avoid grinding or lockup
        if (abs(newAngle - oldShoulder) > 2) {    // implement +/- 2 degree deadband to avoid oscillating
            shoulder = newAngle;
            shoulderServo.write(shoulder);
            oldShoulder = shoulder;
        }

        // move the elbow servo if needed
        
        newAngle = reverseAngle(lf310.lf310Data.Rz);  // map gamepad output (0-255) to servo angle (0-180)
        newAngle = trimAngle(newAngle,ELBOWMIN,ELBOWMAX);       // restrict servo to avoid grinding or lockup
        if (abs(newAngle - oldElbow) > 2) {    // implement +/- 2 degree deadband to avoid oscillating
            elbow = newAngle;
            elbowServo.write(elbow);
            oldElbow = elbow;
            //Serial.print(", elbow: ");
            //Serial.println(elbow);
        }


        // move the gripper servo if needed
        // newAngle = 180;
        // gripperServo.write(newAngle);
        newAngle = reverseAngle(lf310.lf310Data.Z);  // map gamepad output (0-255) to servo angle (0-180)
        newAngle = trimAngle(newAngle,GRIPPERMIN,GRIPPERMAX);       // restrict servo to avoid grinding or lockup
        if (abs(newAngle - oldGripper) > 2) {    // implement +/- 2 degree deadband to avoid oscillating
            gripper = newAngle;
            gripperServo.write(gripper);
            oldGripper = gripper;
            Serial.print(", gripper: ");
            Serial.println(gripper);
        } 
        delay(15);                       // waits 15ms for the servo to reach the position
    }
}
