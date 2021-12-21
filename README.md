# AvgBMP+screen
 ESP32 + Pulse Sensor + screen 
 
#include <Adafruit_GFX.h>   // libreria para pantallas graficas

#include <Adafruit_SSD1306.h>   // libreria para controlador SSD1306

******************************************************
  Authors Erin Gee // Martin Peach // Thomas Ouellet-Fredericks
  for more info see README at https://github.com/eringee/BioData/
  
  **************AvgBPM2_screenCLIENT****************************************
  NO FUNCIONA
  
//// WiFi UDP connection ////

#include <WiFi.h>

#include <WiFiUdp.h>

const char* ssid     = "ESPdatos";

const char* password = "respiracion";

IPAddress ip(192, 168, 4, 1);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

WiFiUDP Udp;

IPAddress myIP;  

