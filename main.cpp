#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
const char *ssid = "Milk_Can_WiFi";
const char *password = "12345678";
// Pins Setup
#define ONE_WIRE_BUS 4  // D2 Pin (DS18B20 Temperature Sensor)
#define PH_PIN A0       // A0 Pin (Analog pH Sensor)
#define RELAY_PIN 14    // D5 Pin (12V Peltier Cooling Relay Module)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
ESP8266WebServer server(80);
float tempC = 0.0;
float phValue = 0.0;
bool isCoolingOn = false;
// JSON Data Endpoint for Mobile App (http://192.168.4.1/data)
void handleData() {
  String json = "{\"temp\":" + String(tempC, 1) + ",\"ph\":" + String(phValue, 2) + "}";
  server.send(200, "application/json", json);
}
void setup() {
    Serial.begin(115200);
    // Relay Pin Configuration (Active LOW relay module assumed)
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH); // Initially OFF (HIGH = Relay OFF)
    sensors.begin();
    // Start Access Point (No Internet Required)
    WiFi.softAP(ssid, password);
    Serial.println("\nMilk Can Hotspot Started!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP()); // Default IP: 192.168.4.1
    / Handle API Requests from MIT App Inventor
    server.on("/data", handleData);
    server.begin();
}
void loop() {
  server.handleClient();
  // Read DS18B20 Temperature Sensor
  sensors.requestTemperatures();
  float currentTemp = sensors.getTempCByIndex(0);
  int analogVal = analogRead(PH_PIN);
  float voltage = analogVal * (3.3 / 1023.0);
  phValue = 3.5 * voltage; 
  // Valid Temperature Check (Avoid sensor disconnect error -127°C)
  if (currentTemp != -127.00 && currentTemp != 85.00) {
    tempC = currentTemp;
// --- Temperature Maintenance Logic (4°C to 6°C) ---//
    if (tempC > 6.0 && !isCoolingOn) {
        digitalWrite(RELAY_PIN, LOW); // Relay ON -> Peltier Cooling Started
        isCoolingOn = true;
        Serial.println("Temp > 6.0 C -> Cooling Started");
    }    
    else if (tempC <= 4.0 && isCoolingOn) {
        digitalWrite(RELAY_PIN, HIGH); // Relay OFF -> Peltier Cooling Stopped
        isCoolingOn = false;
        Serial.println("Temp <= 4.0 C -> Cooling Stopped");
    }
  }
  delay(1000); // 1 second update cycle
}