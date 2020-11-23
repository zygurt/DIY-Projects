//see https://github.com/dadamachines/doppler for doppler info

void setupBoard(void){
  //Pin setup
  //Digital Pins 0-9 are key inputs
  //A3 is breath sensor
  //A5 is Octave buttons [0.33, 1.65, 2.97]V
  //A7 is MIDI out

  //Button setup
  for (int n=0;n<10;n++){
    pinMode(n,INPUT);
  }
  
  
}

uint16_t readButtons(void){
  //Input - Button
//  0 - B
//  1 - A (C)
//  2 - G
//  3 - G sharp
//  4 - F
//  5 - E
//  6 - D
//  7 - D#
//  8 - Bb
//  9 - C low
//  A5 - Octave [0.33, 1.65, 2.97]V

byte n = 0;
uint16_t note = 0;
for(n=0;n<10;n++){
  note = note | (digitalRead(n)<<n);
}
return note;

}
