#include <Arduino.h>
// #include <LittleFS.h>
// #include <FS.h>
// #include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
// #include <ArduinoJson.h>
// #include <PubSubClient.h>
// #include <Button2.h>
#include <ezLED.h>
// #include <TickTwo.h>

// #define _DEBUG_

#ifdef _DEBUG_
#define _serialBegin(...) Serial.begin(__VA_ARGS__)
#define _de(...)          Serial.print(__VA_ARGS__)
#define _deln(...)        Serial.println(__VA_ARGS__)
#define _deF(...)         Serial.print(F(__VA_ARGS__))
#define _delnF(...)       Serial.println(F(__VA_ARGS__))  // printing text using the F macro
#define _deVar(...) Serial.print(F(#__VA_ARGS__ " = ")); Serial.print(__VA_ARGS__); Serial.print(F(" "))
#define _deVarln(...) _deVar(__VA_ARGS__); Serial.println()
#else
#define _serialBegin(...)
#define _de(...)
#define _deln(...)
#define _deF(...)
#define _delnF(...)
#define _deVar(...)
#define _deVarln(...)
#endif

#define led LED_BUILTIN  // define the LED pin, usually it's the built-in LED pin
ezLED statusLed(led);  // create a LED object that attach to pin 9

void setup() {
  _serialBegin(115200);  // initialize serial communication at 115200 baud rate
  statusLed.blinkNumberOfTimes(200, 300, 3);  // 250ms ON, 750ms OFF, repeat 3 times, blink immediately
  // statusLed.blink(250, 750, 1000); // 250ms ON, 750ms OFF, blink after 1 second
}

void loop() {
  statusLed.loop(); // MUST call the led.loop() function in loop()

  // if (statusLed.getState() == LED_BLINKING)
    _deVarln(statusLed.getState());  // print the current state of the LED
  // else if (statusLed.getState() == LED_IDLE)
  // _deVarln(stausLed.getState());

  // To stop blinking immediately, call led.cancel() function
  // delay(100);  // wait for 1 second
}