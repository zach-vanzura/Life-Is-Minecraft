
#ifndef MOUSEACTION_H
#define MOUSEACTION_H

#include <MPU6050_light.h>
// sketch > include library > ESP 32 Combo Keyboard Mouse
#include <BleCombo.h>


#define MOUSE_SENS 15


enum MouseAction {
  CURSOR_UP,
  CURSOR_DOWN,
  CURSOR_LEFT,
  CURSOR_RIGHT,
  CLICK_LEFT,
  CLICK_RIGHT,
  CLICK_RELEASE,
  NO_MOUSE_INPUT
};

MouseAction getMouseInput(MPU6050 &mpuLeft, MPU6050 &mpuRight, MPU6050 &mpuChest);
void actOnInput(BleComboMouse &Mouse, MouseAction &mouseInput, MouseAction &prevAction);
void moveUp(BleComboMouse &mouse);
void moveDown(BleComboMouse &mouse);
void moveLeft(BleComboMouse &mouse);
void moveRight(BleComboMouse &mouse);
void leftClick(BleComboMouse &mouse);
void rightClick(BleComboMouse &mouse);
void releaseClick(BleComboMouse &mouse);
int detectClick(MPU6050 &handMPU);


MouseAction getMouseInput(MPU6050 &mpuLeft, MPU6050 &mpuRight, MPU6050 &mpuChest, MouseAction &prevAction, bool &clicking) {
  // in Degrees [º]
  int currChestX = (int) mpuChest.getAngleX();
  int currChestY = (int) mpuChest.getAngleY();

  // Serial.print("Chest Angle X: \t");
  // Serial.print(currChestX);
  // Serial.print("Chest Angle Y: ");
  // Serial.println(currChestY);


  // Cursor UP and DOWN
  switch (currChestX) {
    case -180 ... -5:
      // add to current player angle 
      return CURSOR_DOWN;
      break;
    case 3 ... 180:
      return CURSOR_UP;
      break;
  }

  // Cursor LEFT and RIGHT
  switch (currChestY) {
    case -180 ... -20:
      return CURSOR_LEFT;
      break;
    case 20 ... 180:
      return CURSOR_RIGHT;
      break;
  }

  if (detectClick(mpuLeft)) {
    clicking = true;
    return CLICK_RIGHT;
  } else if (detectClick(mpuRight)) {
    clicking = true;
    return CLICK_LEFT;
  } else if (clicking) {
      Serial.println("releasing click");
      clicking = false;
    return CLICK_RELEASE;
  } else {
    return NO_MOUSE_INPUT;
  }
}


void actOnInput(BleComboMouse &Mouse, MouseAction &mouseInput, MouseAction prevAction, bool clicking) {
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
      // if prev oreintation was move right or centered, move right, else move left
        if (prevACTION == ())
        moveRight(Mouse);
        break;
      case CLICK_LEFT:
        leftClick(Mouse);
        break;
      case CLICK_RIGHT:
        rightClick(Mouse);
        break;
      case CLICK_RELEASE:
        releaseClick(Mouse);
        break;
    }
}


int detectClick(MPU6050 &handMPU) {
  
  float AccX = handMPU.getAccX();
  float AccY = handMPU.getAccY();
  float AccZ = handMPU.getAccZ();

  if ((AccX > 1 || AccX < -1) && AccZ <= 1.5 && abs(AccY) < 1) return 1;
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
  mouse.press(MOUSE_LEFT);
  delay(300);
}

void rightClick(BleComboMouse &mouse) {
  mouse.press(MOUSE_RIGHT);
  delay(300);
}

void releaseClick(BleComboMouse &mouse) {
  mouse.release(MOUSE_LEFT);
  mouse.release(MOUSE_RIGHT);
}

#endif