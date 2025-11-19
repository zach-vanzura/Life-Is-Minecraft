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
#include "directions.h"

TwoWire busA = TwoWire(0);
TwoWire busB = TwoWire(1);

MPU6050 mpuChest(busA);
MPU6050 mpuRight(busA);
MPU6050 mpuLeft(busB);

void setup() {
  Serial.begin(115200);
  
  // Data pin, clock pin
  busA.begin(5, 18);
  busB.begin(4, 16);

  // mpuChest.setAddress(0x69);
  mpuRight.setAddress(0x68);
  mpuLeft.setAddress(0x68);
  
  // byte statusA1 = mpuChest.begin();
  byte statusA2 = mpuRight.begin();
  byte statusB1 = mpuLeft.begin();

  Serial.print(F("MPU6050 status: "));
  // Serial.println(statusA1);
  Serial.println(statusA2);
  Serial.println(statusB1);
  // while(statusA1!=0){ } // stop everything if could not connect to MPU6050
  while(statusA2!=0){ } // stop everything if could not connect to MPU6050
  while(statusB1!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  // mpuChest.calcOffsets();
  mpuRight.calcOffsets();
  mpuLeft.calcOffsets();
  Serial.println("Done!\n");
}

void loop() {

  mpuRight.update();
  mpuLeft.update();

  Serial.print("Right hand values for gyro: ");
  Serial.print(mpuRight.getGyroZ());
  Serial.print("\tLeft hand values for gyro: ");
  Serial.println(mpuLeft.getGyroZ());

  delay(500);

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























