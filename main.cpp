#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// WiFi Access Point Details
const char *ssid = "Milk_Can_WiFi";
const char *password = "12345678";
// Pins Setup
#define ONE_WIRE_BUS 4  // D2 Pin (DS18B20 Temperature Sensor)
#define PH_PIN A0       // A0 Pin (Analog pH Sensor)
#define RELAY_PIN 14    // D5 Pin (12V Peltier Cooling Relay Module)