#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


int joyX = A0; // analog pin used to connect the X - axis of Joystick
int joyY = A1; // analog pin used to connect the Y - axis of Joystick
int joyButton = 2;     // the number of the Joystick button pin

int x; // variables to read the values from the analog pins 
int y; 
int buttonState = 0;         // variable for reading the pushbutton status


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
tD;
tD transmit_data;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  Serial.println("BB-8 Controller");
 
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(15, 15);
   
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]); // Head
  //radio.openReadingPipe(2, pipes[2]); // Body
  radio.startListening();
  //radio.printDetails();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // First, stop listening so we can talk.
  radio.stopListening();
  
  x = joyX;    // reads the value of the Joystick's X - axis (value between 0 and 1023) 
  y = joyY;    // reads the value of the Joystick's Y - axis (value between 0 and 1023) 
  x = map(analogRead(joyX), 0, 1023, 0, 360); // scale it to use with the servo b/w 900 usec to 2100 usec
  y = map(analogRead(joyY), 0, 1023, 0, 360);
  
  if(digitalRead(joyButton)==0){
    buttonState=1;
  } else{
    buttonState=0;
  }
  
  //Serial.println(x);
  //Serial.println(y);
  //Serial.println(buttonState);
  
  // Take the time, and send it.  This will block until complete
    unsigned long time = millis();
    Serial.print("Now sending ");
    transmit_data.forward = 0;
    transmit_data.lean = 0;
    transmit_data.swivel = x;
    transmit_data.nod = y;
    transmit_data.look = 0;
    transmit_data.sound = buttonState;
    transmit_data.timestamp = millis();
    bool ok = radio.write( &transmit_data, sizeof(transmit_data) );
   
    if (ok)
        Serial.print("ok...");
    else
        Serial.println("failed!");
 
    // Now, continue listening
    radio.startListening();
   
    bool timeout = false;
    uint8_t pipe_num;
    while ( ! radio.available(&pipe_num) && ! timeout )
        if (millis() - transmit_data.timestamp > 250 )
            timeout = true;
 
    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    unsigned long got_time;
   
    if ( timeout ) {
        Serial.println("Failed, response timed out.");
    } else {
        radio.read( &got_time, sizeof(unsigned long) );
        Serial.print(pipe_num);
        Serial.print(" responded in ");
        Serial.print(millis()-got_time);
    }
    Serial.println("");
   
    delay(250);
}
