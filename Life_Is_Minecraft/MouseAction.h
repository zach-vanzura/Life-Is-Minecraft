
#ifndef MOUSEACTION_H
#define MOUSEACTION_H

#include <MPU6050_light.h>
// sketch > include library > ESP 32 Combo Keyboard Mouse
#include <BleCombo.h>
#include <BleComboKeyboard.h>
#include <BleComboMouse.h>
#include <BleConnectionStatus.h>
#include <KeyboardOutputCallbacks.h>

#define MOUSE_SENS 10


enum MouseAction {
  CURSOR_UP,
  CURSOR_DOWN,
  CURSOR_LEFT,
  CURSOR_RIGHT,
  CLICK_LEFT,
  CLICK_RIGHT,
  NO_MOUSE_INPUT
};

MouseAction getMouseInput(MPU6050 &mpuLeft, MPU6050 &mpuRight, MPU6050 &mpuChest);
void moveUp(BleComboMouse &mouse);
void moveDown(BleComboMouse &mouse);
void moveLeft(BleComboMouse &mouse);
void moveRight(BleComboMouse &mouse);
void leftClick(BleComboMouse &mouse);
void rightClick(BleComboMouse &mouse);
int detectClick(MPU6050 &handMPU);


MouseAction getMouseInput(MPU6050 &mpuLeft, MPU6050 &mpuRight, MPU6050 &mpuChest) {
  // in Degrees [º]
  int chestAngleX = (int) mpuChest.getAngleX();
  int chestAngleY = (int) mpuChest.getAngleY();

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
    case -180 ... -5:
      return CURSOR_RIGHT;
      break;
    case 5 ... 180:
      return CURSOR_LEFT;
      break;
  }

  if (detectClick(mpuLeft)) return CLICK_LEFT;
  if (detectClick(mpuRight)) return CLICK_RIGHT;

  // code only gets here if there is no input 
  return NO_MOUSE_INPUT;
}


void actOnInput(BleComboMouse &Mouse, MouseAction &mouseInput) {
    switch (mouseInput) {
      case CURSOR_UP:
        moveUp(Mouse);
        break;
      case CURSOR_DOWN:
        moveDown(Mouse);
        break;
      case CURSOR_LEFT:
        moveLeft(Mouse);
        break;
      case CURSOR_RIGHT:
        moveRight(Mouse);
        break;
      case CLICK_LEFT:
        leftClick(Mouse);
        break;
      case CLICK_RIGHT:
        rightClick(Mouse);
        break;
    }
}


int detectClick(MPU6050 &handMPU) {
  
  float AccX = handMPU.getAccX();
  float AccY = handMPU.getAccY();
  float AccZ = handMPU.getAccZ();

  if (abs(AccX) < 1 && AccY > 1 && abs(AccZ) < 1) return 1;
  return 0;
}

void moveUp(BleComboMouse &mouse) {
  mouse.move(0, -MOUSE_SENS);
}

void moveDown(BleComboMouse &mouse) {
  mouse.move(0, MOUSE_SENS);
}

void moveLeft(BleComboMouse &mouse) {
  mouse.move(-MOUSE_SENS, 0);
}

void moveRight(BleComboMouse &mouse) {
  mouse.move(MOUSE_SENS, 0);
}

void leftClick(BleComboMouse &mouse) {
  mouse.click(MOUSE_LEFT);
}

void rightClick(BleComboMouse &mouse) {
  mouse.click(MOUSE_RIGHT);
}

#endif