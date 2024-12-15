#include <Arduino_LED_Matrix.h>
//#include <gallery.h>

#include <FastLED.h>
//#include "Arduino_LED_Matrix.h"   // Include the LED_Matrix library
#include "lcdgfx.h"
#include "lcdgfx_gui.h"


/* LED panel attached to I2C */
#define NUM_LEDS 63
#define DATA_PIN 5
#define CLOCK_PIN 13

/* Direct attached LEDs */
#define LED_B_PWM_PIN 11
#define LED_R_PWM_PIN 10
#define LED_G_PWM_PIN 9

/* Button */
#define BUTTON_PIN 12
#define LATCH_THRESHOLD 5000
#define HOLD_THRESHOLD 4294967294
#define SHORT_PRESS 5000
#define LONG_PRESS 140000

/* Smiley face for internal LED matrix on Arduino Uno */
const uint32_t happy[] = {
	0x19819,
	0x80000001,
	0x81f8000
};

DisplaySSD1306_128x64_I2C display(-1);

CRGB leds[NUM_LEDS];

int myMatrix[64][3] = { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},  {0,0,0},{200,4,23},{0,0,0},{200,4,23},{0,0,0},{200,4,23},{200,4,23},{200,4,23},  
     {0,0,0},{200,4,23},{0,0,0},{0,0,0},{200,4,23},{0,0,0},{200,4,23},{0,0,0},  {0,0,0},{200,4,23},{200,4,23},{200,4,23},{0,0,0},{0,0,0},{200,4,23},{0,0,0},  {0,0,0},{200,4,23},{0,0,0},{0,0,0},{200,4,23},{0,0,0},{200,4,23},{0,0,0},  {0,0,0},{200,4,23},{0,0,0},{200,4,23},{0,0,0},{0,0,0},{200,4,23},{0,0,0},  {200,4,23},{200,4,23},{200,4,23},{0,0,0},{200,4,23},{0,0,0},{200,4,23},{0,0,0},  {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

int myMatrix2[64][3] = { {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},  {0,0,0},{0,50,50},{0,0,0},{0,50,50},{0,0,0},{0,50,50},{0,50,50},{0,50,50},  {0,0,0},{0,50,50},{0,0,0},{0,0,0},{0,50,50},{0,0,0},{0,50,50},{0,0,0},
  {0,0,0},{0,50,50},{0,50,50},{0,50,50},{0,0,0},{0,0,0},{0,50,50},{0,0,0},  {0,0,0},{0,50,50},{0,0,0},{0,0,0},{0,50,50},{0,0,0},{0,50,50},{0,0,0},  {0,0,0},{0,50,50},{0,0,0},{0,50,50},{0,0,0},{0,0,0},{0,50,50},{0,0,0},  {0,50,50},{0,50,50},{0,50,50},{0,0,0},{0,50,50},{0,0,0},{0,50,50},{0,0,0},  {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};



int globalState = 0;
int nextGlobalState = 0;

ArduinoLEDMatrix matrix;

bool latchState = false;
unsigned long latchCount = 0;
int switchState = 0; 
unsigned long holdCount = 0;
bool inLongHold = false;



void setup() {
    // sanity check delay - allows reprogramming if accidently blowing power wleds
      Serial.begin(9600);
     Serial.println("wait 1 sec!");
     pinMode(LED_R_PWM_PIN, OUTPUT);
     pinMode(LED_G_PWM_PIN, OUTPUT);
     pinMode(LED_B_PWM_PIN, OUTPUT);

     pinMode(BUTTON_PIN, INPUT_PULLUP);
     digitalWrite(LED_R_PWM_PIN, HIGH);     
     digitalWrite(LED_G_PWM_PIN, HIGH); 
     digitalWrite(LED_B_PWM_PIN, HIGH); 
    delay(1000);
digitalWrite(LED_R_PWM_PIN, LOW);     
digitalWrite(LED_G_PWM_PIN, LOW); 
digitalWrite(LED_B_PWM_PIN, LOW); 
    // Uncomment/edit one of the following lines for your leds arrangement.
    // ## Clockless types ##
    // FastLED.addLeds<SM16703, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1829, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1812, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1809, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1804, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<TM1803, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1903B, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS1904, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<UCS2903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2852, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<GS1903, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SK6812, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA106, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<PL9823, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SK6822, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2813, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<APA104, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2811_400, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GE8822, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<GW6205_400, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD1886, DATA_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<LPD1886_8BIT, DATA_PIN, RGB>(leds, NUM_LEDS);
    // ## Clocked (SPI) types ##
    // FastLED.addLeds<LPD6803, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<LPD8806, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical
    // FastLED.addLeds<WS2801, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<WS2803, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<SM16716, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);
    // FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical
    // FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS);  // BGR ordering is typical

    // FastLED.setBrightness(CRGB(255,255,255));
    FastLED.setBrightness(12);
    matrix.begin(); 
    
    display.setFixedFont( ssd1306xled_font6x8 );

    display.begin();
    display.clear();

    globalState = 0;

}

void loop() {
    int i;
     int pwmCount = 0;
   
    processButtons();

    switch (globalState) {
      case 0: /* Bootup state - any init that needs to happen outside of setup */
        Serial.print("State "); Serial.println(globalState);

        matrix.loadFrame(happy); /* Internal matrix diplay - only for Arduino Uno R4 */

        /* Display test information on I2C attached SSD1306 display */
        display.setFixedFont( ssd1306xled_font6x8 );
        display.clear();
        //display.invertColors();
        display.printFixed(0,  8, " F L A S H L I G H T", STYLE_BOLD);
        display.invertColors();
        /* display.printFixed(0, 16, "Bold text", STYLE_BOLD); */
        display.printFixed(0, 24, "Please wait...", STYLE_ITALIC);
        display.printFixed(0, 32, "", STYLE_ITALIC);
        display.invertColors();
        display.printFixed(0, 40, "BOOT", STYLE_BOLD);
        

        fadeLED(LED_R_PWM_PIN, 5, 5);
        fadeLED(LED_R_PWM_PIN, -5, 5);
        
        nextGlobalState = 9;
        break;
      case 9:  /* Standby initialization */
        Serial.print("State "); Serial.println(globalState);
        display.clear();
        //display.invertColors();
        display.printFixed(0,  8, " F L A S H L I G H T", STYLE_BOLD);
        //display.invertColors();
        /* display.printFixed(0, 16, "Bold text", STYLE_BOLD); */
        display.printFixed(0, 24, "    Press and hold", STYLE_ITALIC);
        display.printFixed(0, 32, "   button for power", STYLE_ITALIC);
        display.invertColors();
        display.printFixed(0, 56, "        STANDBY      ", STYLE_BOLD);
        display.invertColors();

        nextGlobalState = 10;
        break;

      case 10: /* Standby state - off, waiting for button */
        // Serial.print("State "); Serial.println(globalState);
        processButtons();
        
        /* process the global latch state here */

        nextGlobalState = 10;
        break;
      
      default:
        nextGlobalState = 0;
    }
         

    globalState = nextGlobalState;
    


}


void fadeLED(int fadePin, int fadeStep, int fadeDelay) {
    int pwmCount = 0;
    if (fadeStep > 0) {
      for ( pwmCount = 0 ; pwmCount < 255 ; pwmCount += fadeStep )
      {
        analogWrite(fadePin, pwmCount); 
        delay(fadeDelay);         
      }
      analogWrite(fadePin, 255);
    }
    else if (fadeStep < 0) {
      for ( pwmCount = 255 ; pwmCount > 0 ; pwmCount += fadeStep ) { 
        analogWrite(fadePin, pwmCount); 
        delay(fadeDelay);    
      }
      analogWrite(fadePin, 0);
    }
}


void setLatch() {
  int pwmCount = 0;
  holdCount = 1;
  fadeLED(LED_G_PWM_PIN, 5, 5);

  return;
}

void unsetLatch() {
  int pwmCount = 0;

  

  Serial.print("  hold count ");  Serial.println(holdCount);

  if ( (holdCount < LONG_PRESS) ) { buttonShortPress(); }
  else if ( holdCount > LONG_PRESS ) { buttonLongPress(); }

  holdCount = 0;
  inLongHold = false;
  
 if (inLongHold) { analogWrite(LED_G_PWM_PIN,0); } else { fadeLED(LED_G_PWM_PIN, -5, 5); }
analogWrite(LED_R_PWM_PIN,0);
  return;
}

void processButtons() {

 // Read physical switch
  switchState = !digitalRead(BUTTON_PIN);
  
  if ( latchState ) {
    // Button is latched
    //digitalWrite(ESP8266_LED_C, HIGH);
    if (holdCount >= HOLD_THRESHOLD) {
      Serial.println("Hold count exceeded and reset");
      holdCount = 0;      
    }
    else if ( (holdCount > LONG_PRESS) && (inLongHold == false) ) {
      //Serial.println("* long hold");
      inLongHold = true;
      buttonLongHold();
    }
    holdCount++;

    if ( switchState == LOW ) {
      // Switch has been released
      latchCount++;
      
      if (latchCount > LATCH_THRESHOLD) {
        latchState = false;
        Serial.println("unsetLatch");
        inLongHold = false;
        unsetLatch();
      }
    }
    else {
      //if (latchCount>0){Serial.println(latchCount);}
      latchCount = 0; 
    }
  }
  else {
    // Button not currently latched
    //digitalWrite(ESP8266_LED_C, LOW);    

    if  ( switchState == HIGH ) { 
      // But switch is pressed
      //analogWrite(ESP8266_LED_C, 80);
        // Serial.println("close");
      latchCount++;
      
      if (latchCount > LATCH_THRESHOLD) {
        latchState = true;
        Serial.println("setLatch");
        setLatch();
      }
    }
    else {
      //if (latchCount>0){Serial.print("-"); Serial.println(latchCount); }
      latchCount = 0;
    }
    
  }


  return;

}


void buttonShortPress() {
  Serial.println("  * short press");
}

void buttonLongPress() {
  Serial.println("  * long press");
}

void buttonLongHold() {
  Serial.println("  * long hold");
  analogWrite(LED_R_PWM_PIN, 255);
}

 /*
     for (i =0; i < 64; i++)
    {  
      leds[i] = CRGB(myMatrix[i][0],myMatrix[i][1],myMatrix[i][2]);
    }
    FastLED.show(); 

  for ( pwmCount = 0 ; pwmCount < 255 ; pwmCount += 5 )
  { analogWrite(LED_B_PWM_PIN, pwmCount); delay(5); }

    delay(1000);

        for (i =0; i < 64; i++)
    {  
      leds[i] = CRGB(myMatrix2[i][0],myMatrix2[i][1],myMatrix2[i][2]);
    }
    FastLED.show(); 

  
    delay(1000);
    
    for (i =0; i < 64; i++)
    {  
      leds[i] = CRGB(0,0,128);
    }
    FastLED.show(); 
      for ( pwmCount = 255 ; pwmCount > 0 ; pwmCount -= 5 )
  { analogWrite(LED_G_PWM_PIN, pwmCount); delay(5); 
    digitalWrite(LED_G_PWM_PIN, LOW);}

    delay(250);


  //FastLED.setBrightness(255);
  //FastLED.show();
  //delay(250);

  //FastLED.setBrightness(12);
  //FastLED.show();
  //delay(250);

  */