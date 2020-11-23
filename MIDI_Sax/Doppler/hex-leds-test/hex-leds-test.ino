#include <ICEClass.h>
#include "hex_leds_FPGA.h"
#include "doppler_func.h"
//See https://github.com/dadamachines/doppler for doppler info
ICEClass ice40;
uint16_t hexmapFont[16] = { 0xF99F,0xF22F,0xF42F,0xF17F,0x1F99,0x7F8F,0xF9F8,0x111F,
                            0x7DBE,0x1F9F,0x9F9F,0xADAC,0xF88F,0xE99E,0xF8EF,0x8E8F };



void setup() { // put your setup code here, to run once:
  ice40.upload(hex_leds_FPGA_bin,sizeof(hex_leds_FPGA_bin)); // Upload BitStream Firmware to FPGA -> see variant.h
  delay(100);
  ice40.initSPI();  // start SPI runtime Link to FPGA

  setupBoard();
}

void loop() {  // put your main code here, to run repeatedly:

  //To test, read buttons and set lights on 4x4 grid
  uint16_t note;


  note = readButtons();
  ice40.sendSPI16(note); 
  delay(50);

  //Single Button Test
//  uint16_t button_b = 0;
//  button_b = digitalRead(0);
//  ice40.sendSPI16(button_b); 
//  delay(50);

//  Number output test
//  for(int i = 0 ; i < 10 ; i++){
//      ice40.sendSPI16(hexmapFont[i] );  
//      delay(800);
//  }

//  Counting test
//  uint16_t val = 0;
//  for(int i = 0 ; i < 65536 ; i++){
//      val = i;
//      ice40.sendSPI16(val);  
//      delay(10);
//  }
}
