#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "fauxmoESP.h"
 
#define WIFI_SSID "andy"
#define WIFI_PASS "andy1974"
 
#define SERIAL_BAUDRATE 115200
#define LED 2
 
/* Set Relay Pins */
#define RELAY_1 5
 
fauxmoESP fauxmo;
// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------
 
boolean wifiSetup() {
 
// Set WIFI module to STA mode
WiFi.mode(WIFI_STA);
 
// Connect
Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
WiFi.begin(WIFI_SSID, WIFI_PASS);
 
// Wait
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(100);
}
Serial.println();
 
// Connected!
Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
 
}
 
void setup() {
 
Serial.begin(SERIAL_BAUDRATE);
//setup and wifi connection
    if (wifiSetup()) {
    // Setup fauxmo
     Serial.println("Adding device");
     fauxmo.setPort(80);  
     fauxmo.enable(true);
     fauxmo.addDevice("Sanitizer");   
     fauxmo.onSetState([](unsigned char device_id, const char * device_name, 
                    bool state, unsigned char value)); {
    Serial.print("Sanitizer");
    Serial.println("Sanitizer");
    // Here we handle the command received
//Set relay pins to outputs
pinMode(RELAY_1, OUTPUT);
 
//Set each relay pin to HIGH ====== NOTE THAT THE RELAYS USE INVERSE LOGIC =====
digitalWrite(RELAY_1, HIGH);
delay(500);
 
 
// fauxmoESP 2.0.0 has changed the callback signature to add the device_id,
// this way it's easier to match devices to action without having to compare strings.
fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
digitalWrite(LED, !state);
 
//Switching action on detection of device name
 
if ( (strcmp(device_name, "Sanitizer") == 0) )
{
if (!state)
{
digitalWrite(RELAY_1, HIGH);
}
else
{
digitalWrite(RELAY_1, LOW);
}
}
});
 
}


void loop() 
{
 
// Since fauxmoESP 2.0 the library uses the "compatibility" mode by
// default, this means that it uses WiFiUdp class instead of AsyncUDP.
// The later requires the Arduino Core for ESP8266 staging version
// whilst the former works fine with current stable 2.3.0 version.
// But, since it's not "async" anymore we have to manually poll for UDP
// packets
fauxmo.handle();
 
static unsigned long last = millis();
if (millis() - last > 5000) {
last = millis();
Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
}
 
}
