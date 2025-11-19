/* 
 * ok so...
 *
 * Accelerometer can have 2 addresses:
 * 0x68 when AD0 is pulled low/floating
 * 0x69 when AD0 is pulled high
 *
 * Have to use 2 I2C busses which is fine we just make them like we did below
 */

#include "Wire.h"
#include <MPU6050_light.h>

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
  while(statusA1!=0){ } // stop everything if could not connect to MPU6050
  while(statusA2!=0){ } // stop everything if could not connect to MPU6050
  while(statusB1!=0){ } // stop everything if could not connect to MPU6050
  
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
  
  if((millis()-timer)>500){ // print data every 10ms
  Serial.print("X-A1 : ");
  Serial.print(mpuChest.getAngleX());
  Serial.print("\tY-A2 : ");
  Serial.print(mpuRight.getAngleY());
  Serial.print("\tZ-B1: ");
	Serial.println(mpuLeft.getAngleZ());
	timer = millis();  
  }
}
