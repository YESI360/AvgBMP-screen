// This example demonstrates beating an LED on heartbeat, and determining an average BPM over several heartbeats
// for more info see README at https://github.com/eringee/BioData/

#include <Heart.h>
// Create instance for sensor on analog input pin.
Heart heart(2);

// variables for lighting onboard LED on heartbeat without delays
int counter;  // counter for timing routine without using delay
int LED = 2;       // onboard LED
unsigned long litMillis = 0;        // will store how long LED was lit up
const long ledInterval = 50;        // interval at which to blink LED (milliseconds)

// if you do not receive a heartbeat value in over 5 seconds, flush the BPM array and start fresh
const long flushInterval = 2000;    //interval at which to refresh values stored in array

//variable for attenuating data flow to serial port prevents crashes
const long printInterval = 20;       // millis

boolean doOnce = true;   // makes sure that if a heartbeat is found that information is gathered only once during cycle

// variables for averaging BPM over several heartbeats
int bpmCounter = 0;      // counter for counting bpmArray position
int bpmArray[100];   // the array that holds bpm values. Define as a large number you don't need to use them all.
int totalBPM = 0;          // value for displaying average BPM over several heartbeats
int arraySize = 5;   //determine how many beats you will collect and average
int avgBPM;// = totalBPM / arraySize;

//// WiFi UDP connection ////
#include <WiFi.h>
#include <WiFiUdp.h>
const char* ssid     = "ESPdatos";
const char* password = "respiracion";
IPAddress ip(192, 168, 4, 1);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
WiFiUDP Udp;
IPAddress myIP;                    // the IP address of your shield

////////////// TFT SCREEN //////////
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
/////////////////////////////

void setup() {
  Serial.begin(9600);  // works best in testing with 9600 or lower

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  myIP = WiFi.localIP();

  //optional LED for displaying heartbeat
  pinMode(LED, OUTPUT);
  litMillis = ledInterval;  // make sure the LED doesn't light up automatically

  // Initialize sensor.
  heart.reset();

  ///////////// TFT DISPLAY /////////////
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 1 second
  // Clear the buffer
  display.clearDisplay();
  ////////////////////////////////
}

void loop() {
  // Update sensor.
  heart.update();

  unsigned long currentMillis = millis();   // update time

  if (currentMillis % printInterval == 0) //impose a delay to avoid taxing your serial port
  {
    Serial.print("\t");                  // tab separated values
    Serial.println(heart.getBPM());

    // Display values on the screen
    display.clearDisplay();
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setTextColor(WHITE);        // Draw white text
    display.setCursor(0, 0);
    //display.println(myIP);// Start at top-left corner
    //display.println(F("Client connected ?"));
    display.setCursor(5, 16);
    display.setTextSize(2);             // Draw 2X-scale text
    display.setTextColor(WHITE);
    // display.invertDisplay(true);
    display.println(F("BPM"));
    display.setCursor(15, 35);
    display.println(avgBPM);////////////////////////////
    //display.println(heart.getBPM());////////////////////////////
    display.display();

  }
  // An example of how to do something when a heartbeat is detected.
  // Remember that you should avoid using delays in order to preserve samplerate.

  if (heart.beatDetected())
  {
    digitalWrite(LED, HIGH);
    if (doOnce == true) {
      litMillis = currentMillis;
      digitalWrite(LED, HIGH);                // turn on an LED for visual feedback that heartbeat occurred

      bpmArray[bpmCounter] = heart.getBPM();  // grab a BPM snapshot every time a heartbeat occurs
      bpmCounter++;                           // increment the BPMcounter value
      doOnce = false;
    }

  }
  else
  {
    doOnce = true;
  }


  if (bpmCounter == (arraySize)) // if you have grabbed enough heartbeats to average
  {

    for (int x = 0; x <= (arraySize - 1); x++)// add up all the values in the array
    { 
      totalBPM = totalBPM + bpmArray[x];
      //Serial.println(bpmArray[x]);
    }

    avgBPM = totalBPM / arraySize;       // divide by amount of values processed in array/////////////////////

    Udp.beginPacket(ip, 8888);
    Udp.print(avgBPM);
    Udp.endPacket();

    

    //Serial.print("Your average BPM over ");///////////////////////////////////////////
    //Serial.print(arraySize);
    Serial.print(" beats is ");
    Serial.println(avgBPM);
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
