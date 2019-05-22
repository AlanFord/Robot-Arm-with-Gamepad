/////////////////////////////////////////////////////////////////////////
///  \file MeArm.ino
///  \brief Control a MeArm robot arm with an Arduino, Arduino USB Host
///         Shield, and a Logitech F310 gamepad
///
///  \author Alan Ford
/////////////////////////////////////////////////////////////////////////


// Satisfy IDE, which only needs to see the include statment in the ino.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <SPI.h>
#include "lf310.h"

USB Usb;
LF310 lf310(&Usb);

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
}

   uint8_t oldX = 128;
   uint8_t oldY = 128;
   uint8_t oldZ = 128;
   uint8_t oldRz = 128;
 
void loop() {
    /*
     * These four variable hold the "old" values of the joysticks so that action can be taken
     * only if they change.
     */
 
    Usb.Task();
    if (lf310.connected()) {

        if (lf310.lf310Data.X != oldX) {
          Serial.print("Left Joystick X: ");
          Serial.println(lf310.lf310Data.X);
          oldX = lf310.lf310Data.X;
        }

        if (lf310.lf310Data.Y != oldY) {
          Serial.print("Left Joystick Y: ");
          Serial.println(lf310.lf310Data.Y);
          oldY = lf310.lf310Data.Y;
        }

        if (lf310.lf310Data.Z != oldZ) {
          Serial.print("Right Joystick X: ");
          Serial.println(lf310.lf310Data.Z);
          oldZ = lf310.lf310Data.Z;
        }

        if (lf310.lf310Data.Rz != oldRz) {
          Serial.print("Right Joystick Y: ");
          Serial.println(lf310.lf310Data.Rz);
          oldRz = lf310.lf310Data.Rz;
        }
    }
}
