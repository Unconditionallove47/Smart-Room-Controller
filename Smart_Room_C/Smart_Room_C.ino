/*Project:Smart Room Controller
   Description:Smart Room Controller based on my mother and fathers heat concerns
   Author:Kalif Purce
   Date:6/28/21
*/
#include <SPI.h>
#include <Ethernet.h>
#include <mac.h>
#include <hue.h>
#include "colors.h"
#include <OneButton.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <math.h>
#include <wemoObj.h>
wemoObj myWemo;
OneButton button1(23, false);
Adafruit_NeoPixel pixel(PIXELCOUNT , PIXELPIN , NEO_GRB + NEO_KHZ800);
int led = 13; // define the LED pin  **FOR IR**
int digitalPin = 21; // KY-026 digital interface  **FOR IR**
int analogPin = 20; // KY-026 analog interface  **FOR IR**
int digitalVal; // digital readings  **FOR IR**
int analogVal; //analog readings  **FOR IR**

void setup() {
  Serial.begin(9600);

}

void loop() {


}
