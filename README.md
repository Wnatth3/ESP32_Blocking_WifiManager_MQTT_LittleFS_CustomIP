
 # ESP32 Blocking WiFiManager + MQTT + LittleFS + Custom IP Example
 
 ## Prerequisites:
  
- Hardware: ESP32 development board
- Arduino IDE or PlatformIO
- Libraries:
- WiFiManager (https://github.com/tzapu/WiFiManager)
- LittleFS
- ArduinoJson (https://github.com/bblanchon/ArduinoJson)
- PubSubClient (https://github.com/knolleary/pubsubclient)
- Button2 (https://github.com/LennartHennigs/Button2)
- ezLED (https://github.com/raphaelbs/ezLED)
- TickTwo (https://github.com/StefanBruens/arduino-ticktock)
 
 ## Features:
- Blocking WiFiManager captive portal for WiFi and MQTT configuration
- MQTT client with configurable broker, port, username, and password
- Configuration stored in LittleFS as JSON
- Optional static IP configuration (enable with #define CUSTOM_IP)
- Status LED using ezLED library
- Long-press button to reset WiFi and MQTT configuration (deletes config file and restarts)
- Debug output via serial (enable with #define _DEBUG_)
- MQTT subscribe/publish example placeholders
- Modular code structure for easy extension
 