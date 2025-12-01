/*
 * Owen Cook and Zach Vanzura
 * 
 * Handling the following movements/key presses in minecraft
 * -----------------------------------------------------------
 * | Key      | Action in Game  | Action in Real Life        |
 * -----------------------------------------------------------
 * | W        | Move forward    | Swing both arms opposite   |
 * | S        | Move backward   | Swing both arms same       |
 * | A        | Move left       | Lean left                  |
 * | D        | Move right      | Lean right                 |
 * | F        | Toggle sprint   | Swing arms faster          |
 * | Shift    | Crouch          | ?                          |
 * | Space    | Jump            | Jump                       |
 * | Numbers  | Hotbar Nav      | Press buttons on hand      |
 * -----------------------------------------------------------
 */


#include "Wire.h"
#include <MPU6050_light.h>
#include "direction_speed.h"
#include <Bounce2.h>

#include <BleCombo.h>
#include <BleComboKeyboard.h>
#include <BleComboMouse.h>
#include <BleConnectionStatus.h>
#include <KeyboardOutputCallbacks.h>
#include <BleCombo.h>

// Pins
const byte CLK_BUS_A  = 18;
const byte DATA_BUS_A = 5;
const byte CLK_BUS_B  = 25;
const byte DATA_BUS_B = 26;

const byte PIN_BUTTON_LEFT  = 4;
const byte PIN_BUTTON_RIGHT = 16;

const byte PIN_START_BUTTON = 23;

// Busses
TwoWire busA = TwoWire(0);
TwoWire busB = TwoWire(1);

MPU6050 mpuChest(busA);
MPU6050 mpuLeft(busA);
MPU6050 mpuRight(busB);

Bounce startButton = Bounce();

Bounce leftButton  = Bounce();
Bounce rightButton = Bounce();

int currSlot = 1; // Hotbar slot

bool start = false;

void setup() {
  Serial.begin(115200);

  Keyboard.begin();
  Mouse.begin();

  startButton.attach(PIN_START_BUTTON, INPUT_PULLUP);
  leftButton.attach(PIN_BUTTON_LEFT , INPUT_PULLUP);
  rightButton.attach(PIN_BUTTON_RIGHT, INPUT_PULLUP);

  startButton.interval(5);
  leftButton.interval(5);
  rightButton.interval(5);
  
  // Data pin, clock pin
  busA.begin(DATA_BUS_A, CLK_BUS_A);
  busB.begin(DATA_BUS_B, CLK_BUS_B);

  mpuChest.setAddress(0x69);
  mpuLeft.setAddress(0x68);
  mpuRight.setAddress(0x68);

  Serial.println("Press the button when you are ready");

  while(!start) {
    startButton.update();
    if (startButton.fell()) {
      start = true;
    }
  }

  Serial.println("Get into position");

  delay(5000); // Wait 5 seconds

  Serial.println("Accelerometers callibrating, do not move");
  
  byte statusA1 = mpuChest.begin();
  byte statusA2 = mpuLeft.begin();
  byte statusB1 = mpuRight.begin();

  Serial.print(F("MPU6050 status: "));
  // If something goes wrong then
  while (statusA1 != 0 || statusA2 != 0 || statusB1 != 0) {};

  Serial.println(F("Calculating offsets, do not move"));
  delay(1000);

  mpuChest.calcOffsets();
  mpuLeft.calcOffsets();
  mpuRight.calcOffsets();
  Serial.println("Done!\n");

  // Wait until the keyboard is connected
  while(!Keyboard.isConnected()) {};
  

  char invSlot = currSlot + '0';
  Keyboard.press(invSlot);
  Keyboard.release(invSlot);

}

void loop() {
  if (Keyboard.isConnected()) {

    static Speed speedState = STOP;

    mpuRight.update();
    mpuLeft.update();
    mpuChest.update();

    leftButton.update();
    rightButton.update();

    // going down
    if (leftButton.fell()) {
      // Serial.println("Left button");
      currSlot--;
      if (currSlot == 0) currSlot = 9;
      Serial.println(currSlot);
      char invSlot = currSlot + '0';
      Keyboard.press(invSlot);
      Keyboard.release(invSlot);
    }
    
    // going up
    if (rightButton.fell()) {
      // Serial.println("Right button");
      currSlot++;
      if (currSlot == 10) currSlot = 1;
      Serial.println(currSlot);
      char invSlot = currSlot + '0';
      Keyboard.press(invSlot);
      Keyboard.release(invSlot);

    }

    float absGyroLeft = abs(mpuLeft.getGyroZ()); // angular acceleration z
    float angleLeft = mpuLeft.getAngleZ(); // angle z

    float absGyroRight = abs(mpuRight.getGyroZ()); // angular acceleration in z
    float angleRight = mpuRight.getAngleZ(); // Angle in z

    float accYChest = mpuChest.getAccY(); // Acceleration in y for jumping
    float angleChest = mpuChest.getAngleZ(); // Z angle of chest

    int magAngleLeft = abs(angleLeft);
    int magAngleRight = abs(angleRight);

    checkSpeed(absGyroLeft, absGyroRight);

    if (accYChest > 0.75) {
      Keyboard.press(' ');   // press the key
      Keyboard.release(' '); // release it right away
    }




  }
  else {
    Serial.println("Keyboard not connected");
  }


  // // If the signs are opposite +/- 10 degrees 
  // if (magAngleRight =< magAngleLeft + 10 && magAngleRight >= magAngle - 10) {

  //   // Move backward switch case (both angles have same sign)
  //   if ((angleRight > 0 && angleLeft > 0) || (angleRight < 0 && angleLeft < 0)) {
  //     checkSpeed(absGyroLeft, absGyroRight);
  //   }
  //   // Move forward switch case (angles have different sign)
  //   else {
  //     checkSpeed(absGyroLeft, absGyroRight);

  //   }

  // }


}


/* Moving forward / Backwards */
// Get the z gyroscope values of L and R hands

// See what a good value is to set as a threshold for the runnning vs walking in terms of jerk

// Check to see that the Z angle of sensors are the same 
// in magnitude and sign (because they will be mounted opposite)

// If theres a jerk and theyre same then move foward
// If jerk and theyre same move backwards




/* Move left and right */
// Get the chest angle Z value (because it faces up)
// thats kinda it


void checkSpeed(float gyroLeft, float gyroRight) {

  // Sprinting
  if (gyroLeft >= 230 && gyroRight >= 230) {
    // Serial.println("SPRINTING");
    Keyboard.press('f');
    
  }
  // Walking
  else if (gyroLeft >= 45 && gyroRight >= 45) {
    // Serial.println("WALKING");
    Keyboard.press('w');
  }
  // Stopped
  else if (gyroLeft <= 1 && gyroRight <= 1) {
    // Serial.println("STOPPED");
    Keyboard.release('f');
    Keyboard.release('w');
  }

}






















