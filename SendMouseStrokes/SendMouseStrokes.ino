#include <BleConnectionStatus.h>
#include <BleMouse.h>

/*
 * Owen Cook & Zach Vanzura
 * ========================
 * ESP32 as bluetooth mouse. Upload code, go to your computer settings
 * and look for ESP32 Mouse. Once connected, you should be all set! 
 * Make sure your computer registers the device correctly, as a mouse, not a keyboard.
 *
 * ================================================================================
 * Use ESP32 board version 2.0.17
 * Download keyboard library from github and place in libraries
 * folder
 * https://github.com/T-vK/ESP32-BLE-Mouse 
 *
*/


BleMouse bleMouse;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleMouse.begin();
}

void loop() {
  if(bleMouse.isConnected()) {

    unsigned long startTime;

    Serial.println("Scroll up");
    startTime = millis();
    while(millis()<startTime+2000) {
      bleMouse.move(0,0,1);
      delay(100);
    }
    delay(500);

    Serial.println("Scroll down");
    startTime = millis();
    while(millis()<startTime+2000) {
      bleMouse.move(0,0,-1);
      delay(100);
    }
    delay(500);

    Serial.println("Scroll left");
    startTime = millis();
    while(millis()<startTime+2000) {
      bleMouse.move(0,0,0,-1);
      delay(100);
    }
    delay(500);

    Serial.println("Scroll right");
    startTime = millis();
    while(millis()<startTime+2000) {
      bleMouse.move(0,0,0,1);
      delay(100);
    }
    delay(500);

    Serial.println("Move mouse pointer up");
    startTime = millis();
    while(millis()<startTime+2000) {
      bleMouse.move(0,-1);
      delay(100);
    }
    delay(500);

    Serial.println("Move mouse pointer down");
    startTime = millis();
    while(millis()<startTime+2000) {
      bleMouse.move(0,1);
      delay(100);
    }
    delay(500);

    Serial.println("Move mouse pointer left");
    startTime = millis();
    while(millis()<startTime+2000) {
      bleMouse.move(-1,0);
      delay(100);
    }
    delay(500);

    Serial.println("Move mouse pointer right");
    startTime = millis();
    while(millis()<startTime+2000) {
      bleMouse.move(1,0);
      delay(100);
    }
    delay(500);

  }
}
