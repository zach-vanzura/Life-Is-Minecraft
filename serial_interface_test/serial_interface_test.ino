#include <BleKeyboard.h>

/*
This sketch is a test script for using Keyboard.h and Mouse.h to generate serial commands 
to play minecraft

Created by Zach Vanzura in collaboration with Owen Cook
11/14/2025
*/


#define LED_BUILTIN 2


void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

}
