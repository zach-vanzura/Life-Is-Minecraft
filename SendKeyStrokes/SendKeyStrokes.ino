/*
 * Owen Cook & Zach Vanzura
 * ========================
 * ESP32 as bluetooth keyboard. Upload code, go to your computer settings
 * and look for ESP32 Keyboard. Once connected, you should be all set!
 *
 * ================================================================================
 * Use ESP32 board version 2.0.17
 * Download keyboard library from github and place in libraries
 * folder
 * https://github.com/T-vK/ESP32-BLE-Keyboard/releases/tag/0.3.0
 * 
 * Documentation for library found here:
 * https://docs.arduino.cc/language-reference/en/functions/usb/Keyboard/
 *
*/


#include <BleKeyboard.h>

BleKeyboard bleKeyboard;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
}

void loop() {
  if(bleKeyboard.isConnected()) {
    Serial.println("Sending 'Hello world'...");
    bleKeyboard.print("Hello world");

    delay(1000);

    Serial.println("Sending Enter key...");
    bleKeyboard.write(KEY_RETURN);

    delay(1000);

    // Serial.println("Sending Play/Pause media key...");
    // bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);

    // delay(1000);

   //
   // Below is an example of pressing multiple keyboard modifiers 
   // which by default is commented out.
    /*
    Serial.println("Sending Ctrl+Alt+Delete...");
    bleKeyboard.press(KEY_LEFT_CTRL);
    bleKeyboard.press(KEY_LEFT_ALT);
    bleKeyboard.press(KEY_DELETE);
    delay(100);
    bleKeyboard.releaseAll();
    */
  }

  Serial.println("Waiting 5 seconds...");
  delay(5000);
}
