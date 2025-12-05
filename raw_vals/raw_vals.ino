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
   | Mouse x/y| Looking around  | Cursor Movement            |
   | Mouse 1  | Left Click      | Left Click                 |
   | Mouse 2  | Right Click     | Right Click                |
 * -----------------------------------------------------------
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

// #include <BleCombo.h>
// #include <BleComboKeyboard.h>
// #include <BleComboMouse.h>
// #include <BleConnectionStatus.h>
// #include <KeyboardOutputCallbacks.h>

#include "direction_speed.h"
// #include <MPU6050_light.h>
#include <Bounce2.h>  // Bound2

// Pins
const byte CLK_BUS_A  = 13;
const byte DATA_BUS_A = 14;
const byte CLK_BUS_B  = 25;
const byte DATA_BUS_B = 26;

const byte PIN_BUTTON_LEFT  = 32;
const byte PIN_BUTTON_RIGHT = 33;

const byte PIN_START_BUTTON = 27;

// digital two bus I2C
TwoWire busA = TwoWire(0);
TwoWire busB = TwoWire(1);

MPU6050 mpuChest(busA);
MPU6050 mpuRight(busA);
MPU6050 mpuLeft(busB);

Bounce startButton = Bounce();

Bounce leftButton  = Bounce();
Bounce rightButton = Bounce();

int currSlot = 1; // Hotbar slot
int currChestX = 0;
int currChestY = 0;

bool start = false;

void setup() {
  Serial.begin(115200);
  
  startButton.attach(PIN_START_BUTTON, INPUT_PULLUP);
  leftButton.attach(PIN_BUTTON_LEFT , INPUT_PULLUP);
  rightButton.attach(PIN_BUTTON_RIGHT, INPUT_PULLUP);

  startButton.interval(5);
  leftButton.interval(5);
  rightButton.interval(5);

  MPUinit();
  Serial.println("Done!\n");
  Keyboard.begin();
  Mouse.begin();

}

void loop() {
  if (Keyboard.isConnected()) {

    mpuChest.update();
    mpuRight.update();
    mpuLeft.update();

    leftButton.update();
    rightButton.update();

    // float absGyroLeft = abs(mpuLeft.getGyroZ()); // angular acceleration z
    // float angleLeft = mpuLeft.getAngleZ(); // angle z
    float accXLeft = mpuLeft.getAccX();
    float accYLeft = mpuLeft.getAccY();
    float accZLeft = mpuLeft.getAccZ();

    Serial.println("Left Hand Accelerations");
    Serial.print("X: ");
    Serial.print(accXLeft);
    Serial.print("\tY: ");
    Serial.print(accYLeft);
    Serial.print("\tZ: ");
    Serial.println(accZLeft);


    float angleYChest = mpuChest.getAngleY();
    float angleZChest = mpuChest.getAngleZ();

    Serial.println("Chest Angles");
    Serial.print("Y: ");
    Serial.print(angleYChest);
    Serial.print("\tZ: ");
    Serial.println(angleZChest);

    // float absGyroRight = abs(mpuRight.getGyroZ()); // angular acceleration in z
    // float angleRight = mpuRight.getAngleZ(); // Angle in z

    // float accYChest = mpuChest.getAccY(); // Acceleration in y for jumping
    // float angleChest = mpuChest.getAngleZ(); // Z angle of chest

    // int magAngleLeft = abs(angleLeft);
    // int magAngleRight = abs(angleRight);






    delay(1000);
  } 

}

// might want to move this into a different header file later on...

/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+=== MPU init  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/

void MPUinit() {

  // Data pin, clock pin
  busA.begin(DATA_BUS_A, CLK_BUS_A);
  busB.begin(DATA_BUS_B, CLK_BUS_B);

  mpuChest.setAddress(0x69);
  mpuRight.setAddress(0x68);
  mpuLeft.setAddress(0x68);

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
  byte statusA2 = mpuRight.begin();
  byte statusB1 = mpuLeft.begin();

  Serial.print(F("MPU6050 status: "));
  Serial.println(statusA1);
  Serial.println(statusA2);
  Serial.println(statusB1);
  // If something goes wrong then
  while (statusA1 != 0 || statusA2 != 0 || statusB1 != 0) {};

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpuChest.calcOffsets();
  mpuRight.calcOffsets();
  mpuLeft.calcOffsets();
  Serial.println("MPUs are set up!\n");

}

/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+=== END MPU init  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/


/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+=== Keyboard Functions  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/
/* Move left and right */
// Get the chest angle Z value (because it faces up)
// thats kinda it

void checkSpeed(float gyroLeft, float gyroRight) {

  // Sprinting
  if (gyroLeft >= 230 && gyroRight >= 230) {
    // Serial.println("SPRINTING");
    Keyboard.press('f');
    delay(10);
    
  }
  // Walking
  else if (gyroLeft >= 45 && gyroRight >= 45) {
    // Serial.println("WALKING");
    Keyboard.press('w');
    delay(10);
  }
  // Stopped
  else if (gyroLeft <= 10 && gyroRight <= 10) {
    // Serial.println("STOPPED");
    Keyboard.releaseAll();
    delay(10);

  }

}

/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+=== END Keyboard Functions  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/


