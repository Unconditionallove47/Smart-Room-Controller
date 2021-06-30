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
const int PIXELCOUNT = 12;
const int PIXELPIN = 17;
bool status;
float tempF = 0;
float tempC;
float pressinHg;
float pressPA;
float humidRH;
int position;
float tempGuage = tempF;
#include <wemoObj.h>
wemoObj myWemo;
Adafruit_BME280 bme;
#define LOGO_HEIGHT   1
#define LOGO_WIDTH    1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
OneButton button1(23, false);
Adafruit_NeoPixel pixel(PIXELCOUNT , PIXELPIN , NEO_GRB + NEO_KHZ800);
int led = 13; // define the LED pin  **FOR IR**
int digitalPin = 21; // KY-026 digital interface  **FOR IR**
int analogPin = 20; // KY-026 analog interface  **FOR IR**
int digitalVal; // digital readings  **FOR IR**
int analogVal; //analog readings  **FOR IR**

void setup() {
  Serial.begin(9600);
  pixel.begin();
  pixel.show();
  pixel.setBrightness(15);
  (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) ;
  status = bme.begin(0X76);
  display.clearDisplay();
  display.display();
  delay(2000);
  display.invertDisplay(true);
  delay(1000);
  display.invertDisplay(false);
  delay(1000);
}

void loop() {
  position = tempF;
  tempGuage = map(position, 10, 120, 0, 12);  //encoder Pos to Position based on 0-95 being convered to 0-15(of available lights)
  pixel.clear();
  if (tempF < 68) {
    pixel.fill( 0x0000FF, 0, tempGuage);
  }
  else if (tempF >= 69 && tempF <= 80) {
    pixel.fill( 0x808000 , 0, tempGuage);
  }
  else {
    pixel.fill(0xFF0000, 0, tempGuage);
  }
  pixel.show();
  tempC = bme.readTemperature(); //deg C
  tempF = (tempC * 1.8) + 32;
  pressPA = bme.readPressure(); //pascals
  pressinHg = (pressPA / 33.863886666667);
  humidRH = bme.readHumidity(); //%RH
  Serial.print("Temp=");
  Serial.print(tempF);
  Serial.println("°F");
  delay(1000);
  Serial.print("pressure=");
  Serial.print(pressinHg / 100.0F);
  Serial.println("hPa");
  delay(1000);
  Serial.print("Humidity=");
  Serial.print(bme.readHumidity());
  Serial.println("%");
  delay(1000);
  testdrawstyles();
}

void testdrawstyles(void) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Draw 'inverse' text
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.printf("temp=%f \n Pressure=%f \n Humidity=%f\n", tempF, pressinHg / 100.0F, bme.readHumidity());
  display.display();
  delay(2000);

}
