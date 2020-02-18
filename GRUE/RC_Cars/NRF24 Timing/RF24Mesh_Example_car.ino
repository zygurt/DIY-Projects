
/** RF24Mesh_Example.ino by TMRh20
 *
 * This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
 * master node.
 * The nodes will refresh their network address as soon as a single write fails. This allows the
 * nodes to change position in relation to each other and the master node.
 */


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <printf.h>


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**
 * User Configuration: nodeID - A unique identifier for each radio. Allows addressing
 * to change dynamically with physical changes to the mesh.
 *
 * In this example, configuration takes place below, prior to uploading the sketch to the device
 * A unique value from 1-255 must be configured for each node.
 * This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.
 *
 **/
 //Change the nodeID for each arduino programmed.
// Currently, nano is 1, uno is 2.
#define nodeID 1


uint32_t displayTimer = 0;
uint32_t car = nodeID;
const int threshold = 250;
const int analogInPin = A0;
int sensorValue = 0;
int trigger = 0;
const int LEDPin = 9;
int num_read = 5;

struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

void setup() {
  
  Serial.begin(115200);
  //printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
  pinMode(analogInPin, INPUT);
  pinMode(LEDPin, OUTPUT);
  
}



void loop() {
 int i = 0;
  mesh.update();
  sensorValue = analogRead(analogInPin);
  for (i=0 ; i<(num_read-1) ; i++){
    sensorValue = (sensorValue + analogRead(analogInPin)) >> 1;
  }
  // Send to the master node every second
  if (sensorValue > threshold && trigger == 0) {
    //displayTimer = millis();
    trigger = 1;
    digitalWrite(LEDPin, HIGH);
    Serial.println("Lap");
    // Send an 'M' type message containing the car number
    if (!mesh.write(&car, 'M', sizeof(car))) {
      // If a write fails, check connectivity to the mesh network
      if ( ! mesh.checkConnection() ) {
        //refresh the network address
        Serial.println("Renewing Address");
        mesh.renewAddress();
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: "); Serial.println(displayTimer);
    }
  } else if(sensorValue < threshold){
    trigger = 0;
    digitalWrite(LEDPin, LOW);
  }

  while (network.available()) {
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.println(payload.ms);
  }
  delay(50);
}






