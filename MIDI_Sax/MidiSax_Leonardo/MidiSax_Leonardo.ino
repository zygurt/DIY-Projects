/*
 * MIDIUSB_test.ino
 *
 * Created: 4/6/2015 10:47:08 AM
 * Author: gurbrinder grewal
 * Modified by Arduino LLC (2015)
 */ 

#include "MIDIUSB.h"

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void PANIC(void) {
  int n = 0,k = 0;
  for(k=0;k<16;k++){
    for(n=0;n<128;n++){
    midiEventPacket_t noteOff = {0x08, 0x80 | k, n, 0};
    MidiUSB.sendMIDI(noteOff);
  }
  }
  
  
}

// Button define etc.
#define B_Button_Pin 2
#define A_Button_Pin 3
#define G_Button_Pin 4
#define Gs_Button_Pin 5
#define F_Button_Pin 6
#define E_Button_Pin 7
#define D_Button_Pin 8
#define Ds_Button_Pin 9

#define Breath_Pin A0
#define Breath_LED_Pin 11
#define Octave_Pin A1
#define Octave_LED_Pin 10

//#define MIDI_TX_Pin 1
//#define MIDI_RX_Pin 0

#define threshold 5 //Breath on/off threshold

int prev_note = -1;
float breath_rest = 0;
float prev_breath = 0;
bool note_on = 0;
void pinSetup(void){
  int n = 0;
  for(n=B_Button_Pin;n<Ds_Button_Pin;n++){
    pinMode(n,INPUT);
  }
  pinMode(Breath_LED_Pin, OUTPUT);
  pinMode(Octave_LED_Pin, OUTPUT);
  
}


int readNote(void){
  //Notes for later
//  Pin 2(B): Port PD1
//  Pin 3(A): Port PD0
//  Pin 4(G): Port PD4
//  Pin 5(Gs): Port PC6
//  Pin 6(F): Port PD7
//  Pin 7(E): Port PE6
//  Pin 8(D): Port PB4
//  Pin 9(Ds): Port PB5
  byte note_byte = 0;
  int note = -1;
  note_byte = note_byte | (PIND & B00000010)>>1; //B
  note_byte = note_byte | (PIND & B00000001)<<1; //A
  note_byte = note_byte | (PIND & B00010000)>>2; //G
  note_byte = note_byte | (PINC & B01000000)>>3; //Gs
  note_byte = note_byte | (PIND & B10000000)>>3; //F
  note_byte = note_byte | (PINE & B01000000)>>1; //E
  note_byte = note_byte | (PINB & B00010000)<<2; //D
  note_byte = note_byte | (PINB & B00100000)<<2; //Ds
//  Serial.print(note_byte);

  switch (note_byte){
    case 0:
    //C#
      note = 73;
      break;
    case 2:
    //C
      note = 72;
      break;
    case 1:
    //B
      note = 71;
      break;
    case 19:
    //A#
      note = 70;
      break;
    case 3:
    //A
      note = 69;
      break;
    case 15:
    //G#
      note = 68;
      break;
    case 7:
    //G
      note = 67;
      break;
    case 39:
    //F#
      note = 66;
      break;
    case 23:
    //F
      note = 65;
      break;
    case 55:
    //E
      note = 64;
      break;
    case 247:
    //D#
      note = 63;
      break;
    case 119:
    //D
      note = 62;
      break;
    default:
      note = -1;
  }
//  Serial.print("\t");
//  Serial.println(note);

int octave_read = 0;
octave_read = analogRead(Octave_Pin);


if (octave_read>512){
  //top octave
  note += 24;
  analogWrite(255, Octave_LED_Pin);
}else if(octave_read>50){
  //middle octave
  note += 12;
  analogWrite(64, Octave_LED_Pin);
}else{
  //bottom octave
  //note +=0;
}



return note;

}


void breathSetup(void){
  breath_rest = analogRead(Breath_Pin);
  int n = 0, num_run = 100;
  for(n=0;n<num_run;n++){
    breath_rest = (breath_rest+analogRead(Breath_Pin))/2;
  }
}


void setup() {
//  Serial.begin(9600);
  pinSetup();
  breathSetup();
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void loop() {
  int note = -1;
  float breath = 0;
  //Read the breath sensor value
  breath = (analogRead(Breath_Pin)-breath_rest); //Convert to 0-127 range
  //turn off notes if below threshold
//  if (breath<threshold && note_on == 1){
//    noteOff(0, prev_note, breath);  // Channel 0, middle C, normal velocity
//    note_on = 0;
//    MidiUSB.flush(); //This forces the note to be sent
//  }
//  Serial.println(breath);

//Panic code
  if (breath<-20){
    PANIC();
    MidiUSB.flush(); //This forces the note to be sent
  }
  Serial.println(breath);



  analogWrite(Breath_LED_Pin,min(int(breath)>>2,255));
  controlChange(0, 2, min(int(breath),127)); // Set the value of controller 2 (breath) on channel 0 to breath value
  controlChange(0, 74, min(int(breath),127)); // Set the value of controller 74 (mpe) on channel 0 to breath value
//  controlChange(0, 7, min(breath,127)); // Set the value of controller 7 (volume) on channel 0 to breath value
  MidiUSB.flush();
  //Read the note value
  note = readNote();
  //Change note if needed
  if (note!=prev_note && note!=-1){ //&& breath>threshold
    //turn off previous note
    noteOff(0, prev_note, breath);  // Channel 0, middle C, normal velocity
    MidiUSB.flush(); //This forces the note to be sent
//    note_on = 0;
  //turn on new note
    noteOn(0, note, breath);   // Channel 0, middle C, normal velocity
    MidiUSB.flush();
//    note_on = 1;
//    Serial.println(note);
    prev_note = note;
  }
  delay(2);
  
  
}
