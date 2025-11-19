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
#include <BleCombo.h>
#include <BleComboKeyboard.h>
#include <BleComboMouse.h>
#include <BleConnectionStatus.h>
#include <KeyboardOutputCallbacks.h>

#include <MPU6050_light.h>

#include "MouseAction.h"

// digital two bus I2C
TwoWire busA = TwoWire(0);
TwoWire busB = TwoWire(1);

MPU6050 mpuChest(busA);
MPU6050 mpuRight(busA);
MPU6050 mpuLeft(busB);


unsigned long timer = 0;

void setup() {
  Serial.begin(115200);

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
  Serial.println("Done!\n");
}

void loop() {

  mpuChest.update();
  mpuRight.update();
  mpuLeft.update();

  MouseAction mouseInput = getMouseInput()
  Serial.print("Detected mouse input: ");
  Serial.println(mouseInput);
  switch (mouseInput) {
    case CURSOR_UP:
    case CURSOR_DOWN:
    case CURSOR_LEFT:
    case CURSOR_RIGHT:
    
    case CLICK_LEFT:
    case CLICK_RIGHT:
  }
}

MouseAction getMouseInput() {
  // in Degrees [º]
  float chestAngleX = mpuChest.getAngleX();
  float chestAngleY = mpuChest.getAngleY();

  // returns as a multiple of the gravity norm g = 9.81
  float leftAccX = mpuLeft.getAccX();
  float leftAccY = mpuLeft.getAccY();
  float leftAccZ = mpuLeft.getAccZ();

  float rightAccX = mpuRight.getAccX();
  float rightAccY = mpuRight.getAccY();
  float rightAccZ = mpuRight.getAccZ();

  // Cursor UP and DOWN
  switch (chestAngleX) {
    case -180 ... - 10:
      return CURSOR_UP;
      break;
    case 10 ... 180:
      return CURSOR_DOWN;
      break;
  }

  // Cursor LEFT and RIGHT
  switch (chestAngleY) {
    case -180 ... - 5:
      return CURSOR_LEFT;
      break;
    case 5 ... 180:
      return CURSOR_RIGHT;
      break;
  }

  if (detectClick(leftAccX, leftAccY, leftAccZ)) return CLICK_LEFT;
  if (detectClick(rightAccX, rightAccY, rightAccZ)) return CLICK_RIGHT;

  // code only gets here if there is no input 
  return NO_MOUSE_INPUT
}

int detectClick(float X, float Y, float Z) {
  if (abs(X) < 1 && Y > 1 && abs(Z) < 1) return 1;
  return 0;
}
