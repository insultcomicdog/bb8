#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "SoftwareSerial.h"
#include "MP3FLASH16P.h"
MP3FLASH16P myPlayer;
//#include <Servo.h>
#include <Adafruit_TiCoServo.h>
 
RF24 radio(5,10);
// Pipes for reading/writing { Controller, Head, Body }
const uint64_t pipes[3] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL, 0xF0F0F0F0B4LL };
 
typedef struct{
  int forward;
  int lean;
  int swivel;
  int nod;
  int look;
  int sound;
  long timestamp;
}
rD;
rD receive_data;
typedef struct{
  long timestamp;
}
tD;
tD transmit_data;


int audioState = 0;

//PSI,LOGICS AND HOLOJECTOR NEOPIXELS
#define PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(13, PIN, NEO_GRB + NEO_KHZ800);
uint16_t currentPixel = 0;// what pixel are we operating on
uint16_t currentColour = 0;

//RADAR EYE
int value ;
//int radarEyeLEDPin = 5;
int holoLEDPin = 3;
long time=0;
int periode = 2000;

int speakerPin = 0;
int voiceBrightness;
int currentServoPos = 180;
int lastServoPos = 0;    // // variable to store the servo position

//Servo myservo;  // create servo object to control a servo 
Adafruit_TiCoServo myservo;
int servoPin = 9;

void setup(void)
{
    Serial.begin(115200);
    //Serial.begin(9600);
    Serial.println("BB-8 Head");
   
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(15,15);
   
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
   
    radio.startListening();
    //radio.printDetails();
    
    myPlayer.init(2);
    
    //pinMode(radarEyeLEDPin, OUTPUT); 
    pinMode(holoLEDPin, OUTPUT); 
    pinMode(speakerPin, INPUT);
    
    // put your setup code here, to run once:
    strip.begin();
    setFrontLogics(255);
    setSideLogics(255);
    strip.show(); // Initialize all pixels to 'off'
    
    myservo.attach(servoPin);
    
    //play startup sound
    if(!myPlayer.isBusy()){
        myPlayer.playFile(37);
        delay(100);
     }
    
}
 
void loop(void)
{
    
    if ( radio.available() )
    {
        //Serial.println("Data available");
        // Dump the payloads until we've gotten everything
        long transmit_values[7];
        
        // Fetch the payload, and see if this was the last one.
        while(radio.available()){
            
            radio.read( &receive_data, sizeof(receive_data) );
   
            // Spew it
            Serial.print("Got payload: ");
            Serial.print(receive_data.forward);Serial.print(", ");
            Serial.print(receive_data.lean);Serial.print(", ");
            Serial.print(receive_data.swivel);Serial.print(", ");
            Serial.print(receive_data.nod);Serial.print(", ");
            Serial.print(receive_data.look);Serial.print(", ");
            Serial.print(receive_data.sound);Serial.print(", ");
            Serial.println(receive_data.timestamp);
           
            delay(20);
        }
 
        // First, stop listening so we can talk
        radio.stopListening();
 
        // Send the final one back.
        transmit_data.timestamp = receive_data.timestamp;
        radio.write( &transmit_data, sizeof(transmit_data) );
        //Serial.println("Sent response.");
 
        // Now, resume listening so we catch the next packets.
        radio.startListening();
        
        int reading = receive_data.sound;
        
        if(receive_data.look >=80 && receive_data.look <=90){
          currentServoPos = 93;
        } else {
          currentServoPos = receive_data.look;
        }
        
        //Serial.println(currentServoPos);

        if(lastServoPos!=currentServoPos){           
           Serial.println("currentServoPos");
           Serial.println(currentServoPos);
           Serial.println("lastServoPos");
           Serial.println(lastServoPos);

           myservo.write(currentServoPos);
           lastServoPos = currentServoPos;

        }
 
        // if the button state has changed:
        if (reading != audioState) {
    
          audioState = reading;
              
      //    int randSnd = random(1, 20);
      //
      //    while(randSnd % 2==0){
      //         randSnd = random(1, 20);
      //    }
          
          int randSnd = random(0,19)*2+1;
          
          if (audioState == 1) {
                  
             Serial.println(randSnd); 
             Serial.println(myPlayer.isBusy()); 
             if(!myPlayer.isBusy()){
                myPlayer.playFile(randSnd);
                delay(100);
             }
          }
        }
    }
    
    time = millis();
    value = 128+127*cos(2*PI/periode*time);
    //analogWrite(radarEyeLEDPin, value);           // sets the value (range from 0 to 255)
    analogWrite(holoLEDPin, value);
    //myservo.write(currentServoPos);
    
    voiceBrightness = constrain(map(analogRead(speakerPin), 700, 1024, 0, 255), 0, 255);
    //analogWrite(PSILEDPin, voiceBrightness);
    
    setFrontLogics(value);
    setSideLogics(value);
    setPixelColor(0,255,255,51, voiceBrightness);
    strip.show();
    
      //rainbowCycleCustom();
    
    //voiceBrightness = constrain(map(analogRead(speakerPin), 700, 1024, 0, 255), 0, 255);
    //setPixelColor(0,255,255,51,voiceBrightness);
    //strip.show();
    
//    if(myPlayer.isBusy()){
//
//      // Sets the brightness of the light based on the loudness of the voice
//
//      voiceBrightness = constrain(map(analogRead(speakerPin), 700, 1024, 0, 255), 0, 255);
// 
//      //analogWrite(PIN_voice_LED, voiceBrightness);
//      setPixelColor(0,255,255,51,voiceBrightness);
//      strip.show();
//
//      }else{
//  
//          // No voice playing, light is off
//  
//          //analogWrite(PIN_voice_LED, 0);
//          setPixelColor(0,0,0,0,0);
//          strip.show();
//      }
  
    
}

void setFrontLogics(uint16_t brightness) {
  uint16_t i ;

  for (i=1; i<5; i++) {
      setPixelColor(i, 57, 100, 195, brightness);
   }
   
   for (i=5; i<9; i++) {
      setPixelColor(i, 0, 255, 0, brightness);
   }
}

void setSideLogics(uint16_t brightness) {
   uint16_t i ;
   
   for (i=9; i<14; i++) {
      setPixelColor(i, 255, 255, 51, brightness);
   }
  
}

void setPixelColor( uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t brightness) {
   strip.setPixelColor(n, (brightness*r/255) , (brightness*g/255), (brightness*b/255));  
}

void rainbowCycleCustom() {
  strip.setPixelColor(currentPixel, Wheel(((currentPixel * 256 / strip.numPixels())+currentColour) & 255, 1 ));
  strip.show();;

  currentColour++;
  if(currentColour >= 256){
    currentColour = 0;
  }
  
  currentPixel++;
  if(currentPixel == strip.numPixels()){
    currentPixel = 0;
  }
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, float opacity) {
  if(WheelPos < 85) {
   return strip.Color((WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color((255 - WheelPos * 3) * opacity, 0, (WheelPos * 3) * opacity);
  } else {
   WheelPos -= 170;
   return strip.Color(0, (WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity);
  }
}
