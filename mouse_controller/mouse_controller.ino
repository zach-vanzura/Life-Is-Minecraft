/*
 * Owen Cook & Zach Vanzura
 * ========================
 * ESP32 as bluetooth mouse.
 * This sketch is meant to define behavior that the MPUs will interpret 
 * as mouse actions 
 *
 * ================================================================================
 * Use ESP32 board version 2.0.17
 * Ble Mouse and Keyboard combo library: https://github.com/BlynkGO/ESP32-BLE-Combo/tree/main
 * MPU 6050 Light library: https://github.com/rfetick/MPU6050_light/tree/master
 * 
*/

// ==+ std libs +==
#include "Wire.h"

// ==+ custom libs +==
// sketch > include library > ESP 32 Combo Keyboard Mouse
// includes all ncessary libraries for interfacing with MPU and BLEKeyboardMouse
#include "MouseAction.h"
#include <BleCombo.h>
#include <BleComboKeyboard.h>
#include <BleComboMouse.h>
#include <BleConnectionStatus.h>
#include <KeyboardOutputCallbacks.h>
#include <MPU6050_light.h>


// digital two bus I2C
TwoWire busA = TwoWire(0);
TwoWire busB = TwoWire(1);

MPU6050 mpuChest(busA);
MPU6050 mpuRight(busA);
MPU6050 mpuLeft(busB);

unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  MPUinit();
  Serial.println("Done!\n");
  Keyboard.begin();
  Mouse.begin();
}

void loop() {

  mpuChest.update();
  mpuRight.update();
  mpuLeft.update();

  MouseAction mouseInput = getMouseInput(mpuLeft, mpuRight, mpuChest);
  Serial.print("Detected mouse input: ");
  Serial.println(mouseInput);
  // there is no Mouse.isConnected().
  // One .isConnected() to rule them all
  if (Keyboard.isConnected()) {
    actOnInput(Mouse, mouseInput);
    delay(5);
  }
}

// might want to move this into a different header file later on...

/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+=== MPU init  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/

void MPUinit(){
  // Data pin, clock pin
  busA.begin(5, 18);
  busB.begin(4, 16);

  mpuChest.setAddress(0x69);
  mpuRight.setAddress(0x68);
  mpuLeft.setAddress(0x68);

  byte statusA1 = mpuChest.begin();
  byte statusA2 = mpuRight.begin();
  byte statusB1 = mpuLeft.begin();

  Serial.print(F("MPU6050 status: "));
  Serial.println(statusA1);
  Serial.println(statusA2);
  Serial.println(statusB1);
  while (statusA1 != 0) {}  // stop everything if could not connect to MPU6050
  while (statusA2 != 0) {}  // stop everything if could not connect to MPU6050
  while (statusB1 != 0) {}  // stop everything if could not connect to MPU6050

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpuChest.calcOffsets();
  mpuRight.calcOffsets();
  mpuLeft.calcOffsets();
}

/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+=== END MPU init  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/


