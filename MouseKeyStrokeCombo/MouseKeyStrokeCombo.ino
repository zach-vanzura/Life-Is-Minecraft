/*
 * Owen Cook & Zach Vanzura
 * ========================
 * ESP32 as bluetooth mouse and keyboard. Upload code, go to your computer settings
 * and look for ESP32 Mouse. Once connected, you should be all set! 
 * Make sure your computer registers the device correctly, as a mouse, not a keyboard.
 *
 * ================================================================================
 * Use ESP32 board version 2.0.17
 * Download keyboard library from github and place in libraries
 * folder
 * mouse repo: https://github.com/T-vK/ESP32-BLE-Mouse
 * keyboard repo: https://github.com/T-vK/ESP32-BLE-Keyboard/releases/tag/0.3.0 
 *
*/

#include <BleCombo.h>
#include <BleComboKeyboard.h>
#include <BleComboMouse.h>
#include <BleConnectionStatus.h>
#include <KeyboardOutputCallbacks.h>



#include <BleCombo.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Starting work!");
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  if(Keyboard.isConnected()) {
    Serial.println("Sending 'Hello world'");
    Keyboard.println("Hello World");

    Serial.println("Sending Enter key...");
    Keyboard.write(KEY_RETURN);

    Serial.println("Move mouse pointer up");
    Mouse.move(0,-10);
    
    Serial.println("Scroll Down");
    Mouse.move(0,0,-1);

    Serial.println("Left click");
    Mouse.click(MOUSE_LEFT);
  }
  
  Serial.println("Waiting 2 seconds...");
  delay(2000);
}
