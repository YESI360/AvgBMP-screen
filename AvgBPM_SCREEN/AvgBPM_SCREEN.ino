// This example demonstrates beating an LED on heartbeat, and determining an average BPM over several heartbeats
// for more info see README at https://github.com/eringee/BioData/
/******************************************************
  copyright Erin Gee 2017

  Authors Erin Gee // Martin Peach // Thomas Ouellet-Fredericks

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 3 as published by
  the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For more details: <http://www.gnu.org/licenses/>.

******************************************************/
#include <Heart.h>



// Create instance for sensor on analog input pin.
Heart heart(2);

// variables for lighting onboard LED on heartbeat without delays
int LED = 2;       // onboard LED
unsigned long litMillis = 0;        // will store how long LED was lit up
const long ledInterval = 50;        // interval at which to blink LED (milliseconds)

// if you do not receive a heartbeat value in over 5 seconds, flush the BPM array and start fresh
const long flushInterval = 2000;    //interval at which to refresh values stored in array

boolean doOnce = true;   // makes sure that if a heartbeat is found that information is gathered only once during cycle

// variables for averaging BPM over several heartbeats
int bpmCounter = 0;      // counter for counting bpmArray position
int bpmArray[100];   // the array that holds bpm values. Define as a large number you don't need to use them all.
int totalBPM = 0;          // value for displaying average BPM over several heartbeats
int arraySize = 5;   //determine how many beats you will collect and average

////////////// TFT SCREEN //////////
#include <SPI.h>
#include <Wire.h>      // libreria para bus I2C
#include <Adafruit_GFX.h>   // libreria para pantallas graficas
#include <Adafruit_SSD1306.h>   // libreria para controlador SSD1306
#define ANCHO 128     // reemplaza ocurrencia de ANCHO por 128
#define ALTO 64       // reemplaza ocurrencia de ALTO por 64
#define OLED_RESET 4      // necesario por la libreria pero no usado
Adafruit_SSD1306 display(ANCHO, ALTO, &Wire, OLED_RESET);  // crea objeto


void setup() {
  Serial.begin(9600);  // works best in testing with 9600 or lower
  //optional LED for displaying heartbeat
  pinMode(LED, OUTPUT);
  litMillis = ledInterval;  // make sure the LED doesn't light up automatically

  // Initialize sensor.
  heart.reset();

  Wire.begin();         // inicializa bus I2C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void loop() {
  // Update sensor.
  heart.update();
  // update time
  unsigned long currentMillis = millis();

  if (heart.beatDetected()) {
    if (doOnce == true) {
      litMillis = currentMillis;
      digitalWrite(LED, HIGH);                // turn on an LED for visual feedback that heartbeat occurred
      Serial.println("HOLAA");
      bpmArray[bpmCounter] = heart.getBPM();  // grab a BPM snapshot every time a heartbeat occurs
      bpmCounter++;                           // increment the BPMcounter value
      doOnce = false;
    }
  }
  else {
    doOnce = true;
  }

  if (bpmCounter == (arraySize)) {                    // if you have grabbed enough heartbeats to average

    for (int x = 0; x <= (arraySize - 1); x++) {        // add up all the values in the array
      totalBPM = totalBPM + bpmArray[x];
      Serial.println(bpmArray[x]);
    }

    int avgBPM = totalBPM / arraySize;               // divide by amount of values processed in array

    Serial.print("Your average BPM over ");
    Serial.print(arraySize);//5
    Serial.print(" beats is ");
    Serial.println(avgBPM);

    // Display values on the screen
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0, 0);
    display.setCursor(5, 16);
    display.setTextSize(2);             // Draw 2X-scale text
    display.setTextColor(WHITE);
    // display.invertDisplay(true);
    display.println(F("AVG BPM"));
    display.setCursor(15, 35);
    display.println(avgBPM);////////////////////////////
    display.display();
    delay(50);

    bpmCounter = 0;                     //  reset bpmCounter
    totalBPM = 0;                       // refresh totalBPM
    avgBPM = 0;                        // refresh avgBPM
    delay(2000);


  }

  // check to see if it's time to turn off the LED

  if (currentMillis - litMillis >= ledInterval) {   // if led interval has been surpassed turn it off
    digitalWrite(LED, LOW);
  }

  if (currentMillis - litMillis >= flushInterval) { // if you haven't received a heartbeat in a while keep the array fresh
    bpmCounter = 0;
  }
}
