#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
 
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
    }
}
