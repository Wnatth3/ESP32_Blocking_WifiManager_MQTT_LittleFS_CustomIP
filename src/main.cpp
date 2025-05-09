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

// callback notifying us of the need to save config
void saveConfigCallback() {
    // Serial.println("Should save config");
    _delnF("Should save config");
    shouldSaveConfig = true;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println();

    // clean FS, for testing
    // SPIFFS.format();

    // read configuration from FS json
    Serial.println("mounting FS...");

    //     if (SPIFFS.begin()) {
    //         Serial.println("mounted file system");
    //         if (SPIFFS.exists("/config.json")) {
    //             //file exists, reading and loading
    //             Serial.println("reading config file");
    //             File configFile = SPIFFS.open("/config.json", "r");
    //             if (configFile) {
    //                 Serial.println("opened config file");
    //                 size_t size = configFile.size();
    //                 // Allocate a buffer to store contents of the file.
    //                 std::unique_ptr<char[]> buf(new char[size]);

    //                 configFile.readBytes(buf.get(), size);
    // #if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
    //                 DynamicJsonDocument json(1024);
    //                 auto                deserializeError = deserializeJson(json, buf.get());
    //                 serializeJson(json, Serial);
    //                 if (!deserializeError) {
    // #else
    //                 DynamicJsonBuffer jsonBuffer;
    //                 JsonObject&       json = jsonBuffer.parseObject(buf.get());
    //                 json.printTo(Serial);
    //                 if (json.success()) {
    // #endif
    //                     Serial.println("\nparsed json");

    //                     strcpy(mqtt_server, json["mqtt_server"]);
    //                     strcpy(mqtt_port, json["mqtt_port"]);
    //                     strcpy(api_token, json["api_token"]);

    //                     if (json["ip"]) {
    //                         Serial.println("setting custom ip from config");
    //                         strcpy(static_ip, json["ip"]);
    //                         strcpy(static_gw, json["gateway"]);
    //                         strcpy(static_sn, json["subnet"]);
    //                         Serial.println(static_ip);
    //                     } else {
    //                         Serial.println("no custom ip in config");
    //                     }
    //                 } else {
    //                     Serial.println("failed to load json config");
    //                 }
    //             }
    //         }
    //     } else {
    //         Serial.println("failed to mount FS");
    //     }
    loadConfiguration(LittleFS, filename);

    // end read
    // Serial.println(static_ip);
    // Serial.println(api_token);
    // Serial.println(mqtt_server);
    _deVar("ip: ", static_ip);
    _deVar(" | gw: ", static_gw);
    _deVar(" | sn: ", static_sn);
    _deVarln(" | dns: ", static_dns);

    // The extra parameters to be configured (can be either global or just in the setup)
    // After connecting, parameter.getValue() will get you the configured value
    // id/name placeholder/prompt default length
    // WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    // WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
    // WiFiManagerParameter custom_api_token("apikey", "API token", api_token, 34);
    WiFiManagerParameter customMqttBroker("broker", "mqtt server", mqttBroker, 16);
    WiFiManagerParameter customMqttPort("port", "mqtt port", mqttPort, 6);
    WiFiManagerParameter customMqttUser("user", "mqtt user", mqttUser, 10);
    WiFiManagerParameter customMqttPass("pass", "mqtt pass", mqttPass, 10);

    // WiFiManager
    // Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    // set static ip
    // IPAddress _ip, _gw, _sn;
    // _ip.fromString(static_ip);
    // _gw.fromString(static_gw);
    // _sn.fromString(static_sn);
    // wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn);
    IPAddress _ip, _gw, _sn, _dns;
    _ip.fromString(static_ip);
    _gw.fromString(static_gw);
    _sn.fromString(static_sn);
    _dns.fromString(static_dns);
    wifiManager.setSTAStaticIPConfig(_ip, _gw, _sn, _dns);
    // add all your parameters here
    // wifiManager.addParameter(&custom_mqtt_server);
    // wifiManager.addParameter(&custom_mqtt_port);
    // wifiManager.addParameter(&custom_api_token);
    wifiManager.addParameter(&customMqttBroker);
    wifiManager.addParameter(&customMqttPort);
    wifiManager.addParameter(&customMqttUser);
    wifiManager.addParameter(&customMqttPass);

    // reset settings - for testing
    // wifiManager.resetSettings();

    // set minimu quality of signal so it ignores AP's under that quality
    // defaults to 8%
    wifiManager.setDarkMode(true);
    // wifiManager.setMinimumSignalQuality(20);

    // sets timeout until configuration portal gets turned off
    // useful to make it all retry or go to sleep
    // in seconds
    // wifiManager.setTimeout(120);

    // fetches ssid and pass and tries to connect
    // if it does not connect it starts an access point with the specified name
    // here  "AutoConnectAP"
    // and goes into a blocking loop awaiting configuration
    // if (!wifiManager.autoConnect("AutoConnectAP", "password")) {
    if (!wifiManager.autoConnect(deviceName, "password")) {
        // Serial.println("failed to connect and hit timeout");
        _delnF("failed to connect and hit timeout");
        delay(3000);
        // reset and try again, or maybe put it to deep sleep
        ESP.restart();
        delay(5000);
    }

    // if you get here you have connected to the WiFi
    // Serial.println("connected...yeey :)");
    _delnF("connected...yeey :)");

    // read updated parameters
    // strcpy(mqtt_server, custom_mqtt_server.getValue());
    // strcpy(mqtt_port, custom_mqtt_port.getValue());
    // strcpy(api_token, custom_api_token.getValue());
    strcpy(mqttBroker, customMqttBroker.getValue());
    strcpy(mqttPort, customMqttPort.getValue());
    strcpy(mqttUser, customMqttUser.getValue());
    strcpy(mqttPass, customMqttPass.getValue());

    // save the custom parameters to FS
    if (shouldSaveConfig) {
        //         Serial.println("saving config");
        // #if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
        //         DynamicJsonDocument json(1024);
        // #else
        //         DynamicJsonBuffer jsonBuffer;
        //         JsonObject&       json = jsonBuffer.createObject();
        // #endif
        //         json["mqtt_server"] = mqtt_server;
        //         json["mqtt_port"]   = mqtt_port;
        //         json["api_token"]   = api_token;

        //         json["ip"]      = WiFi.localIP().toString();
        //         json["gateway"] = WiFi.gatewayIP().toString();
        //         json["subnet"]  = WiFi.subnetMask().toString();

        //         File configFile = SPIFFS.open("/config.json", "w");
        //         if (!configFile) {
        //             Serial.println("failed to open config file for writing");
        //         }

        // #if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
        //         serializeJson(json, Serial);
        //         serializeJson(json, configFile);
        // #else
        //         json.printTo(Serial);
        //         json.printTo(configFile);
        // #endif
        //         configFile.close();
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

    // Serial.println("local ip");
    // Serial.println(WiFi.localIP());
    // Serial.println(WiFi.gatewayIP());
    // Serial.println(WiFi.subnetMask());
    _deVar("ip: ", WiFi.localIP());
    _deVar(" | gw: ", WiFi.gatewayIP());
    _deVar(" | sn: ", WiFi.subnetMask());
    _deVarln(" | dns: ", WiFi.dnsIP());

    statusLed.blinkNumberOfTimes(200, 200, 3);
}

void loop() {
    statusLed.loop();
    // put your main code here, to run repeatedly:
}
