# ESP32 BlockIng WiFiManager MQTT LittleFS CustomIP

## Prerequisites

- **Hardware:** ESP32 development board
- **Arduino IDE** (or PlatformIO)
- **Libraries:**
    - [WiFiManager](https://github.com/tzapu/WiFiManager)
    - LittleFS
    - [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
    - [PubSubClient](https://github.com/knolleary/pubsubclient)
    - [Button2](https://github.com/LennartHennigs/Button2)
    - [ezLED](https://github.com/raphaelbs/ezLED)
    - [TickTwo](https://github.com/RobTillaart/TickTwo)

## Features

- **WiFiManager**: Captive portal for WiFi and MQTT configuration, with custom static IP support.
- **LittleFS**: Stores and loads configuration (WiFi, MQTT, static IP) in flash memory.
- **MQTT**: Connects to a configurable MQTT broker, supports username/password, and can subscribe/publish to topics.
- **Status LED**: Visual feedback for connection status using the built-in LED.
- **Reset Button**: Long press resets WiFi and MQTT configuration and restarts the device.
- **Debugging**: Optional serial debug output.
- **Non-blocking Tasks**: Uses TickTwo for non-blocking MQTT connection management.