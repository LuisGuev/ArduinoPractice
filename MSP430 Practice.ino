

/*
    Luis Guevara
    CSCE 3612 Embedded Systems
    Lab 5
    4/13/21
*/




// Core library for code-sense
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad MSP430, Stellaris and Tiva, Experimeter Board FR5739 specific
#include "Energia.h"
#elif defined(CORE_TEENSY) // Teensy specific
#include "WProgram.h"
#elif defined(ARDUINO) && (ARDUINO >= 100) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#elif defined(ARDUINO) && (ARDUINO < 100) // Arduino 23 specific
#include "WProgram.h"
#else // error
#error Platform not defined
#endif

// Include application, user and local libraries
#include "SPI.h"

// Screen selection
#define HX8353E // HX8353E K35 HI32 W32 ILI9225B HY28A ST7735 PicasoSPE PicasoSGC

#if defined(ILI9225B)
#include "screen_ILI9225B.h"
Screen_ILI9225B myScreen;

#elif defined(HX8353E)
#include "Screen_HX8353E.h"
Screen_HX8353E myScreen;

#elif defined(W32)
#include "screen_W32.h"
Screen_W32 myScreen;

#elif defined(K35)
#include "screen_K35.h"
Screen_K35 myScreen;

#elif defined(HY28A)
#include "screen_HY28A.h"
Screen_HY28A myScreen;

#elif defined(HI32)
#include "screen_HI32.h"
Screen_HI32 myScreen;

#elif defined(ST7735)
#include "screen_ST7735.h"
Screen_ST7735 myScreen(ST7735R_RED_TAB);

#elif defined(PicasoSPE)
#include "screen_PicasoSPE.h"

#if defined(__LM4F120H5QR__)
//#include "SoftwareSerial.h"
//SoftwareSerial mySerial(PB_0, PB_1);
#define mySerial Serial1

Screen_PicasoSPE myScreen(PA_2, &mySerial);
#else
#define mySerial Serial1
Screen_PicasoSPE myScreen(4, &mySerial);
#endif

#elif defined(PicasoSGC)
#include "screen_PicasoSGC.h"
#define mySerial Serial1
Screen_PicasoSGC myScreen(20, &mySerial);

#else
#error Unknown screen
#endif







//Lux and temperature definitions
#include <Wire.h>
#include "Adafruit_TMP006.h"
#define USE_USCI_B1 
#include "OPT3001.h"
#define USE_USCI_B1 
opt3001 opt3001;
Adafruit_TMP006 tmp006;
void printFloat(float value, int places);
void printmenu();
void protocolText();

//accelerometer definitions
const int xpin = 23;                  // x-axis of the accelerometer
const int ypin = 24;                  // y-axis
const int zpin = 25;                  // z-axis (only on 3-axis models)
const int buzzerPin = 40;    //the number of the buzzer pin


//input definitions
int i = 0;
int incomingByte = 0; // for incoming serial data
char letter; //for string manipulation
int words = 0;
int startup = 0;
int freq = 800;               //frequency set for a buzz
const int joystickX = 2;       // the number of the joystick X-axis analog
const int joystickY =  26;     // the number of the joystick Y-axis analog
int joystickXState, joystickYState ; 
char str[20];
int sentenceloop=0;

int ledRed = 39, ledBlue = 37, ledGreen = 38;    // LED connected to digital pin 9


void setup()
{
  // By default MSP432 has analogRead() set to 10 bits. 
  // This Sketch assumes 12 bits. Uncomment to line below to set analogRead()
  // to 12 bit resolution for MSP432.
 // analogReadResolution(12);
  
  // initialize the serial communications:
  
  
  unsigned int readings = 0;//readings for the lux
  Serial.begin(9600);
  delay(100);
  #if defined(PicasoSPE)
    mySerial.begin(9600);
#endif
    
    myScreen.begin();
  
  
  
  
  
  
  delay(1000);
  opt3001.begin(); 
  
  // get manufacturer ID from OPT3001. Default = 0101010001001001
  readings = opt3001.readManufacturerId();  


  // get device ID from OPT3001. Default = 0011000000000001
  readings = opt3001.readDeviceId();  

  
  // read config register from OPT3001. Default = 1100110000010000
  readings = opt3001.readConfigReg();  


  // read Low Limit register from OPT3001. Default = 0000000000000000
  readings = opt3001.readLowLimitReg();  
 
  
  // read High Limit register from OPT3001. Default = 1011111111111111
  readings = opt3001.readHighLimitReg();  
  
  
  if (! tmp006.begin(TMP006_CFG_8SAMPLE)) {//initialize temp sensor
    Serial.println("No sensor found");
    while (1);
  }
  

  
  
}

void loop()
{
  uint32_t readings;//for the lux readings
  printmenu();
  


  while (!Serial.available()); {//while there is no input
    
  
    // read the incoming byte:
    incomingByte = Serial.read();
   
    if(incomingByte == 49)//if selection 1 was made 
    {
      Serial.println("You have selected option 1");
      float objt = tmp006.readObjTempC();
      Serial.print("Object Temperature: "); Serial.print(objt); Serial.println("*C");
      delay(5000);
    }
   
   else if(incomingByte == 50)//if selection 2 was made 
    {
      Serial.println("You have selected option 2");
      Serial.print(((int) analogRead(xpin)) - 2048);
      Serial.print(", ");
      Serial.println(((int) analogRead(ypin)) - 2048);
      delay(5000);
    }
    
    else if(incomingByte == 51)//if selection 3 was made 
    {
      Serial.println("You have selected option 3");
      // Read OPT3001
      readings = opt3001.readResult();  
      Serial.print("LUX:");
      Serial.println(readings, DEC);
      delay(5000);
    }
    
     else if(incomingByte == 52)//if selection 4 was made 
    {
      Serial.println("You have selected option 4");
      joystickXState = analogRead(joystickX);
      joystickYState = analogRead(joystickY);
      joystickXState = map(joystickXState, 0, 4096, -20, 20);
      joystickYState = map(joystickYState, 0, 4096, -20, 20);
      Serial.print(joystickXState);
      Serial.print(", ");
      Serial.println(joystickYState); 
      delay(5000);
    }
    
    else if(incomingByte == 53)//if selection 5 was made 
    {
      Serial.println("You have selected option 5");
      Serial.print("Enter your string you wish to output");
      
       while (Serial.available()); {
    // read the incoming byte:
    letter = Serial.read();
    str[sentenceloop] = letter;//store in array
    sentenceloop++;
       }
      
      
      uint32_t chrono1, chrono2;
      uint16_t colour;
      myScreen.clear(whiteColour);
      myScreen.setFontSolid(false);
      colour = redColour;
      myScreen.dRectangle(0, 0, myScreen.screenSizeX(), myScreen.screenSizeY(), colour);
      myScreen.setOrientation(0);
      myScreen.setFontSize(3);
      myScreen.gText(15, 4, str, colour);   
      delay(5000);
    }
    
     else if(incomingByte == 54)//if selection 6 was made 
    {
      Serial.println("You have selected option 6");
        analogWrite(ledRed, 75);
        analogWrite(ledBlue, 130);
        analogWrite(ledGreen, 0);   
        delay(5000);
    }
   
   else if(incomingByte == 55)//if selection 7 was made 
    {
      Serial.println("You have selected option 7");
      tone(buzzerPin, freq, 1000);//make a tone
      freq++;//add to frequency so that the tone plays after another selection
      delay(5000);
    }
   
   else{Serial.println("Invalid Input");
   }  
 
}



}
void printmenu(){
  Serial.println("Troubleshooting Program");
  Serial.println(" 1. Read the object temperature and print the value in Celsius");
  Serial.println(" 2. Read the accelerometer and print the x and y angle");
  Serial.println(" 3. Read the ambient light intensity and print the Lux value");
  Serial.println(" 4. Read the joystick x and y and print the x and y angle");
  Serial.println(" 5. Write the input text to the LCD screen by capitalizing the first alphabet of the input text");
  Serial.println("    along with the number of spaces, alphabets, words, and vowels");
  Serial.println(" 6. Generate a color on the LED, indigo or tan, with a brightness 0 to 100");
  Serial.println(" 7. Generate a beep tone on the buzzer");
  Serial.println(" Please input the selection 1 through 7");
}


