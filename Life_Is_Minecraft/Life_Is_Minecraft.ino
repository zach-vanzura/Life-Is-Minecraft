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

// ======= STANDARD LIBRARIES ======= //
#include "Wire.h"

// ======= CUSTOM LIBRARIES ======= //
// includes all ncessary libraries for interfacing with MPU and BLEKeyboardMouse
#include "MouseAction.h"
#include "direction_speed.h"
#include <Bounce2.h>  // Bound2

// ======= PINW ======= //
// I2C busses
const byte CLK_BUS_A  = 13;
const byte DATA_BUS_A = 14;
const byte CLK_BUS_B  = 25;
const byte DATA_BUS_B = 26;

// Inventory control buttons
const byte PIN_BUTTON_LEFT  = 32;
const byte PIN_BUTTON_RIGHT = 33;

// Start/callibration button
const byte PIN_START_BUTTON = 27;

// ======= CONSTANTS ======= //

const float WALKING_THRESHOLD = 230;
const float SPRINTING_THRESHOLD = 60;
const float STOP_THRESHOLD = 15;

const float JUMP_THRESHOLD = 0.75;

// ======= OBJECTS/VARIABLES DEFS ======= //
// I2C busses
TwoWire busA = TwoWire(0);
TwoWire busB = TwoWire(1);

// IMU accelerometers in respective locations
MPU6050 mpuChest(busA);
MPU6050 mpuRight(busA);
MPU6050 mpuLeft(busB);

// Start/callibration logic
Bounce startButton = Bounce();
bool start = false;

// Hotbar selection logic
Bounce leftButton  = Bounce();
Bounce rightButton = Bounce();
int currSlot = 1; // Hotbar slot

// Turning right and left logic
State currState = NOT_TURNING;
int prevChestY = 0;
int prevChestX = 0;


void setup() {
  Serial.begin(115200);
  
  // Buttons
  startButton.attach(PIN_START_BUTTON, INPUT_PULLUP);
  leftButton.attach(PIN_BUTTON_LEFT , INPUT_PULLUP);
  rightButton.attach(PIN_BUTTON_RIGHT, INPUT_PULLUP);

  // Debounce duration
  startButton.interval(5);
  leftButton.interval(5);
  rightButton.interval(5);

  // Initialize MPU and keyboard
  MPUinit();
  Serial.println("Done!\n");
  Keyboard.begin();
  Mouse.begin();

  // Wait until the keyboard is connected
  while(!Keyboard.isConnected()) {};

  // Make sure hotbart slot starts at 1 
  Keyboard.press('1');
  Keyboard.release('1');

}

void loop() {
  // Make sure keyboard is connected
  if (Keyboard.isConnected()) {

    // Used for looking left and right state machine
    static MouseAction prevAction = NO_MOUSE_INPUT;

    // Update necessary stuff
    mpuChest.update();
    mpuRight.update();
    mpuLeft.update();

    leftButton.update();
    rightButton.update();

    // Hotbar selection
    // Moving it left
    if (leftButton.fell()) {

      currSlot--;
      if (currSlot == 0) currSlot = 9;

      char invSlot = currSlot + '0';
      Keyboard.press(invSlot);
      delay(10);
      Keyboard.release(invSlot);
      delay(10);
    }

    // Moving it right
    if (rightButton.fell()) {

      currSlot++;
      if (currSlot == 10) currSlot = 1;

      char invSlot = currSlot + '0';
      Keyboard.press(invSlot);
      delay(10);
      Keyboard.release(invSlot);
      delay(10);

    }

    // Getting all the accelerometer data
    float absGyroLeft = abs(mpuLeft.getGyroZ()); // angular acceleration z
    float angleLeft = mpuLeft.getAngleZ(); // angle z

    float absGyroRight = abs(mpuRight.getGyroZ()); // angular acceleration in z
    float angleRight = mpuRight.getAngleZ(); // Angle in z

    float accYChest = mpuChest.getAccY(); // Acceleration in y for jumping
    float angleChest = mpuChest.getAngleZ(); // Z angle of chest

    int magAngleLeft = abs(angleLeft);
    int magAngleRight = abs(angleRight);

    float accXLeft = mpuLeft.getAccX(); // Acceleration in y for jumping
    float accYLeft = mpuLeft.getAccY(); // Acceleration in y for jumping
    float accZLeft = mpuLeft.getAccZ(); // Acceleration in y for jumping

    float magAccLeft = sqrt(accXLeft*accXLeft +
                            accYLeft*accYLeft +
                            accZLeft*accZLeft);

    float accXRight = mpuRight.getAccX(); // Acceleration in y for jumping
    float accYRight = mpuRight.getAccY(); // Acceleration in y for jumping
    float accZRight = mpuRight.getAccZ(); // Acceleration in y for jumping

    float magAccRight = sqrt(accXRight*accXRight +
                            accYRight*accYRight +
                            accZRight*accZRight);


    // Check for walking or running
    checkSpeed(absGyroLeft, absGyroRight, magAccLeft, magAccRight);

    // Jumping
    if (accYChest > JUMP_THRESHOLD) {
      Keyboard.press(' ');   // press the key
      delay(10);
      Keyboard.release(' '); // release it right away
      delay(10);
    }

    // Acting on mouse inputs
    MouseAction mouseInput = getMouseInput(mpuLeft, mpuRight, mpuChest, prevAction, clicking);
    actOnInput(Mouse, mouseInput, clicking);

    // Saving previous loop states
    delay(5);
    prevChestY = (int)mpuChest.getAngleY();
    prevChestX = (int)mpuChest.getAngleX();
    Serial.println(currState);

    prevAction = mouseInput;

  } 

}


/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+===  MPU init  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/

void MPUinit() {
  // create two virtual I2C busses using Wire.h (only possible on ESP32)
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

void checkSpeed(float gyroLeft, float gyroRight, float magAccLeft, float magAccRight) {

  // if (magAccLeft < 1.25 && magAccRight < 1.25) {
    // Sprinting
    if (gyroLeft >= SPRINTING_THRESHOLD && gyroRight >= SPRINTING_THRESHOLD) {
      // Serial.println("SPRINTING");
      Keyboard.press('f');
      delay(10);
      
    }
    // Walking
    else if (gyroLeft >= WALKING_THRESHOLD && gyroRight >= WALKING_THRESHOLD) {
      // Serial.println("WALKING");
      Keyboard.press('w');
      delay(10);
    }

    // Stopped
    else if (gyroLeft <= STOP_THRESHOLD && gyroRight <= STOP_THRESHOLD) {
      // Serial.println("STOPPED");
      Keyboard.releaseAll();
      delay(10);

    }
  // }

}

/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+=== END Keyboard Functions  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/


