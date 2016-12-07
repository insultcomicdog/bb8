#include <Adafruit_NeoPixel.h>
#include "RF24.h"
#include <SPI.h>
#include <Adafruit_TiCoServo.h>

RF24 radio(9,10);
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


int swivel = 512;
int currentServoPos = 90;

int RPWM=5;
int LPWM=6;
int L_EN=7;
int R_EN=8;


//2, 3, 5, 6, 7, 8, 11, 12, 13, 44, 45, 46
int headServoPin1 = 11;
int headServoPin2 = 12;

Adafruit_TiCoServo headServo1;
Adafruit_TiCoServo headServo2;

//PSI,LOGICS AND HOLOJECTOR NEOPIXELS
#define PIN 2
Adafruit_NeoPixel strip = Adafruit_NeoPixel(19, PIN, NEO_GRB + NEO_KHZ800);

int value ;
long time=0;
int periode = 2000;

void setup() {
  Serial.begin(115200);
    //Serial.begin(9600);
    Serial.println("BB-8 Body");
   
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(15,15);
   
    radio.openWritingPipe(pipes[2]);
    radio.openReadingPipe(1,pipes[0]);
   
    radio.startListening();
    //radio.printDetails(); 
    
    for(int i=5;i<9;i++){
     pinMode(i,OUTPUT);
    }
     for(int i=5;i<9;i++){
     digitalWrite(i,LOW);
    }
    
    digitalWrite(R_EN, HIGH);
    digitalWrite(L_EN, HIGH);
    
    headServo1.attach(headServoPin1);
    headServo2.attach(headServoPin2);
    
    strip.begin();
    setLedPanels(255);
    strip.show();

}

void loop() {
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
        
        swivel = receive_data.swivel;
        
        if(receive_data.nod >=80 && receive_data.nod <=90){
          currentServoPos = 90;
        } else {
          currentServoPos = receive_data.nod;
        }
        
        //Serial.println(currentServoPos);
    }
    
    //****************spin flywheel***************************

    int remappedswivel = map(swivel,0,1023,255,-255);
  
     
    //Serial.println(remappedswivel);

     if (remappedswivel <= -10)                                      // decide which way to turn the wheels based on deadSpot variable
     {
        remappedswivel = abs(remappedswivel);
        analogWrite(RPWM, remappedswivel);                                // set PWM pins 
        analogWrite(LPWM, 0);
        //Serial.println("right");
        //Serial.println(swivel);
        }
     else if (remappedswivel > 10)                          // decide which way to turn the wheels based on deadSpot variable
        { 
        remappedswivel = abs(remappedswivel);
        analogWrite(LPWM, remappedswivel);  
        analogWrite(RPWM, 0);
        //Serial.println("left");
        //Serial.println(swivel);
        }

     else {
        analogWrite(LPWM, 0);  
        analogWrite(RPWM, 0);
        //Serial.println("stop");
     }
     
//     Serial.println(currentServoPos);
//     Serial.println(180-currentServoPos);

     int remappedcurrentServoPos = 180 - currentServoPos;
     
     headServo1.write(remappedcurrentServoPos);
     headServo2.write(180-remappedcurrentServoPos);
     
     time = millis();
     value = 128+127*cos(2*PI/periode*time);
     setLedPanels(value);
     strip.show();

}

void setLedPanels(uint16_t brightness) {

   uint16_t i ;
   
   //panel 4

  for (i=0; i<1; i++) {
      setPixelColor(i, 255, 0, 0, brightness);
   }
   
   for (i=1; i<2; i++) {
      setPixelColor(i, 56, 8, 156, brightness);
   }
   
   //panel 2
   for (i=2; i<4; i++) {
      setPixelColor(i, 255, 0, 0, brightness);
   }
   
   for (i=4; i<5; i++) {
      setPixelColor(i, 255, 255, 255, brightness);
   }
   
   //panel 5
   for (i=5; i<6; i++) {
      setPixelColor(i, 255, 255, 255, brightness);
   }
   
   //panel 6
   for (i=6; i<7; i++) {
      setPixelColor(i, 255, 0, 0, brightness);
   }
   
   for (i=7; i<8; i++) {
      setPixelColor(i, 0, 0, 255, brightness);
   }
   
   for (i=8; i<9; i++) {
      setPixelColor(i, 255, 0, 0, brightness);
   }
   
   for (i=9; i<10; i++) {
      setPixelColor(i, 255, 0, 0, brightness);
   }
   
   for (i=10; i<11; i++) {
      setPixelColor(i, 255, 0, 0, brightness);
   }
   
   for (i=11; i<12; i++) {
      setPixelColor(i, 255, 0, 0, brightness);
   }
   
   //panel 1
   for (i=12; i<16; i++) {
      setPixelColor(i, 0, 255, 255, brightness);
   }
   
   //panel 3
   for (i=16; i<20; i++) {
      setPixelColor(i, 255, 255, 255, brightness);
   }
   
}

void setPixelColor( uint16_t n, uint8_t r, uint8_t g, uint8_t b, uint16_t brightness) {
   strip.setPixelColor(n, (brightness*r/255) , (brightness*g/255), (brightness*b/255));  
}
