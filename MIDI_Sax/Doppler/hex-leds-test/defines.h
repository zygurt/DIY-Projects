//This file contains all of the register defines for the dadamachines doppler
// that are being used in this project

#define PORTA 0x40000000
#define PORTB 0x41000000
#define PORTC 0x42000000
#define PORTD 0x43000000

//GPIO PORT OFFSETS
#define PORT_DIR 0x00   //Set the Port Pin direction
#define PORT_OUT 0x10   //Holds the values on the pins
#define PORT_OUTCLR 0x14  //Set an output low
#define PORT_OUTSET 0x18  //Set an output high
#define PORT_OUTTGL 0x1C  //Toggle an output value
#define PORT_IN 0x20   //Holds the input value on a pin
#define PORT_CRTL 0x24  //On demand (0) or Continuous (1) input sampling
#define PORT_WRCONFIG 0x28 //Used for setting configuration of multiple pins at a time p.904 datasheet
