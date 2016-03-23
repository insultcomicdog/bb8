#include <Adafruit_NeoPixel.h>


//RADAR EYE
int value ;
int radarEyeLEDPin = 5;                           // light connected to digital pin 10
long time=0;
int periode = 2000;

//PSI AND HOLOPROJECTOR NEOPIXELS
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(4, PIN, NEO_GRB + NEO_KHZ800);


const int buttonPin = 2; 
int speakerPin = 4;
int voiceBrightness;

int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers


#include "SoftwareSerial.h"
#include "MP3FLASH16P.h"
MP3FLASH16P myPlayer;


//Pin connected to ST_CP of 74HC595
int latchPin = 9;
//Pin connected to SH_CP of 74HC595
int clockPin = 8;
////Pin connected to DS of 74HC595
int dataPin = 7;

byte patterns[30] = {
B00000001, 100,
B00000010, 100,
B00000100, 100,
B00001000, 100,
B00010000, 100,
B00100000, 100,
B01000000, 100,
B10000000, 100,
B01000000, 100,
B00100000, 100,
B00010000, 100,
B00001000, 100,
B00000100, 100,
B00000010, 100
};

int index = 0;
int count = sizeof(patterns) / 2;


void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
//  
//  //strip.setPixelColor(0, strip.Color(255,255,51)); // yellow/white color
//  strip.setPixelColor(2, strip.Color(255, 255, 255)); // indigo blue color
//  strip.setPixelColor(3, strip.Color(255, 255, 255)); // indigo blue color
  strip.show();

  pinMode(radarEyeLEDPin, OUTPUT); 
  pinMode(buttonPin, INPUT);
  pinMode(speakerPin, INPUT);
  
  myPlayer.init(3);
  
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}


void loop() {
  // put your main code here, to run repeatedly:

  time = millis();
  value = 128+127*cos(2*PI/periode*time);
  analogWrite(radarEyeLEDPin, value);           // sets the value (range from 0 to 255) 
  
  setPixelColor(1,57, 100, 195,value);
  setPixelColor(2,255, 0, 0,value);
  setPixelColor(3,255, 0, 0,value);

  strip.show();
  
  
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {

      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      
      int randSnd = random(1, 11); 
      
      if (buttonState == HIGH) {
              
         Serial.println(randSnd); 
         Serial.println(myPlayer.isBusy()); 
         if(!myPlayer.isBusy()){
            myPlayer.playFile(randSnd);
            delay(100);
        }
      }
    }
  }

//  // save the reading.  Next time through the loop,
//  // it'll be the lastButtonState:
  lastButtonState = reading;
  
  
  // Voice light

    if(myPlayer.isBusy()){

        // Sets the brightness of the light based on the loudness of the voice

        voiceBrightness = constrain(map(analogRead(speakerPin), 700, 1024, 0, 255), 0, 255);
   
        //analogWrite(PIN_voice_LED, voiceBrightness);
        setPixelColor(0,255,255,51,voiceBrightness);
        strip.show();

    }else{

        // No voice playing, light is off

        //analogWrite(PIN_voice_LED, 0);
        setPixelColor(0,0,0,0,0);
        strip.show();
    }
    
    
digitalWrite(latchPin, LOW);
shiftOut(dataPin, clockPin, MSBFIRST, patterns[index * 2]);
digitalWrite(latchPin, HIGH);
//delay(patterns[(index * 2) + 1]);
index++;
if (index >= count){
index = 0;
}


// Serial.print(reading); 
// Serial.println(""); 
}

void setPixelColor( uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t brightness) {
   strip.setPixelColor(n, (brightness*r/255) , (brightness*g/255), (brightness*b/255));
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
