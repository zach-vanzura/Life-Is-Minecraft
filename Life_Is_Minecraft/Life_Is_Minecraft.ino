/*
 * Owen Cook & Zach Vanzura
 * ========================
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

#include <BleCombo.h>
#include <BleComboKeyboard.h>
#include <BleComboMouse.h>
#include <BleConnectionStatus.h>
#include <KeyboardOutputCallbacks.h>

#include "direction_speed.h"
#include <MPU6050_light.h>
#include <Bounce2.h>  // Bound2

// Pins
const byte CLK_BUS_A  = 18;
const byte DATA_BUS_A = 5;
const byte CLK_BUS_B  = 25;
const byte DATA_BUS_B = 26;

const byte pinButton = 23;


// digital two bus I2C
TwoWire busA = TwoWire(0);
TwoWire busB = TwoWire(1);

MPU6050 mpuChest(busA);
MPU6050 mpuRight(busA);
MPU6050 mpuLeft(busB);

Bounce button = Bounce();
bool start = false;

void setup() {
  Serial.begin(115200);

  button.attach(pinButton, INPUT_PULLUP);
  button.interval(5);

  MPUinit();
  Serial.println("Done!\n");
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  if (Keyboard.isConnected()) {
    static Speed speedState = STOP;

    mpuChest.update();
    mpuRight.update();
    mpuLeft.update();
    
    actOnInput(Mouse, mouseInput);
    delay(5);

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

    MouseAction mouseInput = getMouseInput(mpuLeft, mpuRight, mpuChest);
    Serial.print("Detected mouse input: ");
    Serial.println(mouseInput);
    // there is no Mouse.isConnected().
    // One .isConnected() to rule them all

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
    button.update();
    if (button.fell()) {
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
  Serial.println("Done! \n");

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
    Serial.println("SPRINTING");
    Keyboard.press('f');
    
  }
  // Walking
  else if (gyroLeft >= 45 && gyroRight >= 45) {
    Serial.println("WALKING");
    Keyboard.press('w');
  }
  // Stopped
  else if (gyroLeft <= 1 && gyroRight <= 1) {
    Serial.println("STOPPED");
    Keyboard.release('f');
    Keyboard.release('w');
  }

}

/*
 ===+=== ===+=== ===+=== ===+=== ===+=== ===+=== END Keyboard Functions  ===+=== ===+=== ===+=== ===+=== ===+=== ===+===
*/


