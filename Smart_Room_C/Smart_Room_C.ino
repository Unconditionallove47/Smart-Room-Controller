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
#define LOGO_HEIGHT   1
#define LOGO_WIDTH    1
#define SCREEN_WIDTH 128      // OLED display width, in pixels
#define SCREEN_HEIGHT 64      // OLED display height, in pixels
#define OLED_RESET     4      // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C     ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
unsigned long timeStamp;    //Timestamp for current time
unsigned long lastStamp;
const int PIXELCOUNT = 12;   //NEO PIXEL COUNT
const int PIXELPIN = 17;     //NEO PIXEL PINS SETUP
float tempF = 0;       // FOR BME
float tempC;           //FOR BME C TO F
float pressinHg;       //PRESSURE FOR BME
float pressPA;         //PRESSURE FOR BME CONVERSION
float humidRH;         //BME HUMIDITY
float tempGuage = tempF;   //FOR TEMP CONV
bool buttonState;         //FOR BUTTON FUNC
bool buttonStateHold;     //FOR HOLD FUNC
bool status;
bool status1;
int heat;
int position;            //BME AND NEO
int rot = 2;
int clicks;                 //INTEGER FOR PRESS X2
int led = 13;       // define the LED pin  **FOR IR**
int digitalPin = 21;      // KY-026 digital interface  **FOR IR**
int analogPin = 20;      // KY-026 analog interface  **FOR IR**
int digitalVal;      // digital readings  **FOR IR**
int analogVal;      //analog readings  **FOR IR**
wemoObj myWemo;
Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
EthernetClient client;
Adafruit_NeoPixel pixel(PIXELCOUNT , PIXELPIN , NEO_GRB + NEO_KHZ800);
OneButton button(23, false);    //BUTTON SET TO FALSE

void setup() {

  Serial.begin(9600);
  pinMode(led, OUTPUT);   //FOR IR
  pinMode(digitalPin, INPUT);    //FOR IR
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  status1 = Ethernet.begin(mac);           //ETHER BEGIN TO MAC
  if (!status1) {
    Serial.printf("failed to configure Ethernet using DHCP \n");
    //no point in continueing
    while (1);
  }
  Serial.print("My IP address:");
  for (byte thisbyte = 0; thisbyte < 4; thisbyte++) {
    //print value of each byte of the IP address
    Serial.print(Ethernet.localIP()[thisbyte], DEC);
    if (thisbyte < 3) Serial.print(".");
  }
  pixel.begin();                 // NEOPIXEL SETTING
  pixel.show();                  //NEOPIXEL SETTING
  pixel.setBrightness(15);       //NEOPIXEL SETTING
  (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) ;      //OLED
  status = bme.begin(0X76);          //BME START
  display.display();                 //DISPLAYS DISP
  display.setRotation(rot);          //DISPLAY ROTATION
  button.attachClick(click);    //BUTTON CLICK
  button.attachLongPressStart(longPress);   //BUTTON LONGPRESS
  buttonState = false;           //BUTTON STATE
  buttonStateHold = false;       //BUTTON STATE FOR HOLD
  button.setPressTicks(250);    //BUTTON TICKS
}

void loop() {

  timeStamp = millis();     // TIMESTAMP TO MILLISECONDS
  button.tick();
  position = tempF;    //SETS POSITION TO THE TEMP
  tempGuage = map(position, 10, 120, 0, 12);  // Position based on 10-120 degrees being convered to 0-12(of available lights)
  heat = map(heat, 0, 1023, 1023, 0); //Switches heat value to min max instead of max min
  pixel.clear();
  if (tempF < 68) {
    pixel.fill( 0x0000FF, 0, tempGuage);    //COLD TEMP FOR NEO
  }
  else if (tempF >= 69 && tempF <= 80) {    //MED TEMP FOR NEO
    pixel.fill( 0x808000 , 0, tempGuage);
  }
  else {
    pixel.fill(0xFF0000, 0, tempGuage);     //HOT TEMP FOR NEO
  }
  pixel.show();
  tempC = bme.readTemperature(); //deg C   //TEMP CONVERSION
  tempF = (tempC * 1.8) + 32;
  pressPA = bme.readPressure(); //pascals
  pressinHg = (pressPA / 33.863886666667);  //PRESSURE CONVERSION
  humidRH = bme.readHumidity(); //%RH
  if ((timeStamp - lastStamp) > 3000) {
    Serial.print("Temp=");            //PRINTS FOR TEMP ON SM
    Serial.print(tempF);
    Serial.println("°F");
    Serial.print("pressure=");        //PRINTS FOR PRESSURE ON SM
    Serial.print(pressinHg / 100.0F);
    Serial.println("hPa"); Serial.print("Humidity=");        //PRINTS FOR HUMIDITY ON SM
    Serial.print(bme.readHumidity());
    Serial.println("%");
    lastStamp = millis();
  }
  oledtexttemp();        //OLED DISPLAY TEXT STYLE
  if ((timeStamp - lastStamp) > 3000) {       //TEMP SET HUE BLUE
    if (tempF < 68 ) {
      setMyHue(true,HueBlue);
    
    }
    else if (tempF >= 69 && tempF <= 80 ) {    //TEMP SET HUE YELLOW
    setMyHue(true,HueYellow);
    }
    else {
      (tempF <= 81);                           //TEMP SET HUE RED
     setMyHue(true,HueRed);
    }
    lastStamp = millis();                     //MILLIS TIMESTAMP
  }
  // Read the digital interface
  digitalVal = digitalRead(digitalPin);
  if (digitalVal == HIGH) // if flame is detected
  {
    digitalWrite(led, HIGH); // turn ON Arduino's LED
  }
  else
  {
    digitalWrite(led, LOW); // turn OFF Arduino's LED
  }
  // Read the analog interface
  heat = analogRead(analogPin);
  if ((timeStamp - lastStamp) > 3000) {
    Serial.println(heat); // print analog value to serial
    lastStamp = millis();
  }
  if (tempF >= 80 && buttonState == false) {
    myWemo.switchON (0);                          //temp over 80 "fan" on
    myWemo.switchON (2);
  }
  if (buttonState == true && tempF > 80) {         //TEMP OVER 80 "FAN" OFF
    myWemo.switchOFF(0);
    myWemo.switchOFF(2);
  }
  if (tempF >= 69 && tempF <= 79 && buttonStateHold == false) {     //If temp is between 69 and 80 "heater" on
    myWemo.switchON (1);
    myWemo.switchON (3);
  }
  if (tempF >= 69 && tempF <= 79 && buttonStateHold == true) {    //If temp is between 69 and 80 "heater" OFF
    myWemo.switchOFF (1);
    myWemo.switchOFF (3);
  }
}
void oledtexttemp (void) {            //VOID FOR OLED DISPLAY TEXT
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);   // Draw 'inverse' text
  display.setTextSize(1.5);
  display.setCursor(0, 0);    //PRINT F BELOW PRINTS EVERYTHING TO SCREEN,TEMP,PRESSURE,HUMIDITY,HEAT READING
  display.printf("Temperature is=%f \n Current Pressure=%f \n Humidity Level=%f\n Heat Level=%i\n", tempF, pressinHg / 100.0F, bme.readHumidity(), heat);
  display.display();
}
void click() {                   //VOID FOR BUTTON CLICK
  buttonState = !buttonState;
  Serial.println("Single Press");
}
void longPress() {        //void for BUTTON HOLD CLICK
  buttonStateHold = !buttonStateHold;
  Serial.println("Hold Press");
}
void setMyHue(bool Enabled, int color)  {                                 //HUE FUNCTIONN WORKS<3
  int Sat = 247;
  int Bri = 103;
  int i;
  for ( i = 1; i <= 5; i++) {
    setHue(i, Enabled, color, Bri, Sat);
  }
}
