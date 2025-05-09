#include <FS.h>           //this needs to be first, or it all crashes and burns...
#include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager
#include <LittleFS.h>
#include <ArduinoJson.h>  //https://github.com/bblanchon/ArduinoJson
#include <ezLED.h>

// #define _DEBUG_
#include "Debug.h"

//******************************** Configulation ****************************//
#define FORMAT_LITTLEFS_IF_FAILED true

//******************************** Variables & Objects **********************//
#define deviceName "MyESP32"

const char* filename = "/config.txt";  // Config file name

bool mqttParameter;

//----------------- esLED ---------------------//
#define led LED_BUILTIN
ezLED statusLed(led);

//----------------- WiFi Manager --------------//
// default custom static IP
char static_ip[16]  = "192.168.0.191";
char static_gw[16]  = "192.168.0.1";
char static_sn[16]  = "255.255.255.0";
char static_dns[16] = "1.1.1.1";
// MQTT parameters
char mqttBroker[16] = "192.168.0.10";
char mqttPort[6]    = "1883";
char mqttUser[10];
char mqttPass[10];

bool shouldSaveConfig = false;

WiFiManager wifiManager;

WiFiManagerParameter customMqttBroker("broker", "mqtt server", mqttBroker, 16);
WiFiManagerParameter customMqttPort("port", "mqtt port", mqttPort, 6);
WiFiManagerParameter customMqttUser("user", "mqtt user", mqttUser, 10);
WiFiManagerParameter customMqttPass("pass", "mqtt pass", mqttPass, 10);

//******************************** Functions ********************************//
//----------------- LittleFS ------------------//
// Loads the configuration from a file
void loadConfiguration(fs::FS& fs, const char* filename) {
    // Open file for reading
    File file = fs.open(filename, "r");
    if (!file) {
        _delnF("Failed to open data file");
        return;
    }

    // Allocate a temporary JsonDocument
    JsonDocument doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        _delnF("Failed to read file, using default configuration");
    }
    // Copy values from the JsonDocument to the Config
    // strlcpy(Destination_Variable, doc["Source_Variable"] /*| "Default_Value"*/, sizeof(Destination_Name));
    strlcpy(mqttBroker, doc["mqttBroker"], sizeof(mqttBroker));
    strlcpy(mqttPort, doc["mqttPort"], sizeof(mqttPort));
    strlcpy(mqttUser, doc["mqttUser"], sizeof(mqttUser));
    strlcpy(mqttPass, doc["mqttPass"], sizeof(mqttPass));
    mqttParameter = doc["mqttParameter"];

    if (doc["ip"]) {
        strlcpy(static_ip, doc["ip"], sizeof(static_ip));
        strlcpy(static_gw, doc["gateway"], sizeof(static_gw));
        strlcpy(static_sn, doc["subnet"], sizeof(static_sn));
        strlcpy(static_dns, doc["dns"], sizeof(static_dns));
    } else {
        _delnF("No custom IP in config file");
    }

    file.close();
}

void saveConfigCallback() {
    _delnF("Should save config");
    shouldSaveConfig = true;
}

void setup() {
    _serialBegin(115200);

    while (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
        _delnF("Failed to initialize LittleFS library");
        delay(1000);
    }

     _delnF("Loading configuration");
    loadConfiguration(LittleFS, filename);

    _deVar("ip: ", static_ip);
    _deVar(" | gw: ", static_gw);
    _deVar(" | sn: ", static_sn);
    _deVarln(" | dns: ", static_dns);

    wifiManager.setSaveConfigCallback(saveConfigCallback);

    // set static ip
    IPAddress _ip, _gw, _sn, _dns;
    _ip.fromString(static_ip);
    _gw.fromString(static_gw);
    _sn.fromString(static_sn);
    _dns.fromString(static_dns);
    wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn, _dns);
    // add all your parameters here
    wifiManager.addParameter(&customMqttBroker);
    wifiManager.addParameter(&customMqttPort);
    wifiManager.addParameter(&customMqttUser);
    wifiManager.addParameter(&customMqttPass);

    // reset settings - for testing
    // wifiManager.resetSettings();
    wifiManager.setDarkMode(true);
    // wifiManager.setMinimumSignalQuality(20); // Default 8%
    // wifiManager.setConfigPortalTimeout(60);
    // wifiManager.setConfigPortalBlocking(false);
    // wifiManager.setTimeout(120);

    // if (!wifiManager.autoConnect(deviceName, "password")) {
    //     _delnF("failed to connect and hit timeout");
    //     delay(3000);
    //     ESP.restart();
    // }
    // _delnF("WiFI is connected :D");
     if (wifiManager.autoConnect(deviceName, "password")) {
        _delnF("WiFI is connected :D");
    } else {
        _delnF("Configportal running");
    }

    // read updated parameters
    strcpy(mqttBroker, customMqttBroker.getValue());
    strcpy(mqttPort, customMqttPort.getValue());
    strcpy(mqttUser, customMqttUser.getValue());
    strcpy(mqttPass, customMqttPass.getValue());

    // save the custom parameters to FS
    if (shouldSaveConfig) {
        File file = LittleFS.open(filename, "w");
        if (!file) {
            _delnF("Failed to open config file for writing");
            return;
        }

        // Allocate a temporary JsonDocument
        JsonDocument doc;
        // Set the values in the document
        doc["mqttBroker"] = mqttBroker;
        doc["mqttPort"]   = mqttPort;
        doc["mqttUser"]   = mqttUser;
        doc["mqttPass"]   = mqttPass;

        doc["ip"]      = WiFi.localIP().toString();
        doc["gateway"] = WiFi.gatewayIP().toString();
        doc["subnet"]  = WiFi.subnetMask().toString();
        doc["dns"]     = WiFi.dnsIP().toString();

        if (doc["mqttBroker"] != "") {
            doc["mqttParameter"] = true;
            mqttParameter        = doc["mqttParameter"];
        }

        // Serialize JSON to file
        if (serializeJson(doc, file) == 0) {
            _delnF("Failed to write to file");
        } else {
            _deVarln("The configuration has been saved to ", filename);
        }

        file.close();  // Close the file
        // end save
    }

    _deVar("ip: ", WiFi.localIP());
    _deVar(" | gw: ", WiFi.gatewayIP());
    _deVar(" | sn: ", WiFi.subnetMask());
    _deVarln(" | dns: ", WiFi.dnsIP());

    statusLed.blinkNumberOfTimes(200, 200, 3);
}

void loop() {
    statusLed.loop();
}
