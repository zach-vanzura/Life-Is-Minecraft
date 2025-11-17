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

MPU6050 mpuA1(busA);
MPU6050 mpuA2(busA);
MPU6050 mpuB1(busB);

unsigned long timer = 0;

void setup() {
  Serial.begin(115200);
  
  // Data pin, clock pin
  busA.begin(5, 18);
  busB.begin(4, 16);

  mpuA1.setAddress(0x69);
  mpuA2.setAddress(0x68);
  mpuB1.setAddress(0x68);
  
  byte statusA1 = mpuA1.begin();
  byte statusA2 = mpuA2.begin();
  byte statusB1 = mpuB1.begin();

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
  mpuA1.calcOffsets();
  mpuA2.calcOffsets();
  mpuB1.calcOffsets();
  Serial.println("Done!\n");
}

void loop() {

  mpuA1.update();
  mpuA2.update();
  mpuB1.update();
  
  if((millis()-timer)>500){ // print data every 10ms
  Serial.print("Z-A1 : ");
  Serial.print(mpuA1.getAngleZ());
  Serial.print("\tZ-A2 : ");
  Serial.print(mpuA2.getAngleZ());
  Serial.print("\tZ-B1: ");
	Serial.println(mpuB1.getAngleZ());
	timer = millis();  
  }
}
