// scrolltext demo for Adafruit RGBmatrixPanel library.
// Demonstrates double-buffered animation on our 16x32 RGB LED matrix:
// http://www.adafruit.com/products/420

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

//There is a runtime error somewhere causing the hourglass to need individual direction calibration.
//When the unit is first turned on, it is calibrated by rotating the hourglass 360 degrees
//The calibration must also be done for the hourglass, but only the first time.

#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT 10
#define OE  9
#define ACCELX A0
#define ACCELY A1
#define ACCELZ A2
#define A   A3
#define B   A4
#define C   A5

//Function prototypes
void rainbow_fill(int time_total);
void colour_fill(int time_total, int colour);
void up_fill(int pixels, int colour);
void down_fill(int pixels, int colour);
void timer_fill(int num_pixels, int colour);
void raindrops(void);
void bubbles_vis(void);
void adaptive_raindrops(int current_floor, int time_total);
void flash_red(void);
void helium_ball(void);
void gravity_ball(void);
void hourglass_vis(int current_floor);
//void change_timer(void);
void time_testing(void);
//Accelerometer functions
void update_min_max(int x, int y);
double calc_angle(int x, int y);
void update_gravity2(double world_angle);
void update_gravity4(double world_angle);
void update_gravity8(double world_angle);



// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);
// Double-buffered mode consumes nearly all the RAM available on the
// Arduino Uno -- only a handful of free bytes remain.
int num_pixels = 0;
int8_t time_total = 1; //1 = 15 sec
int8_t visualisation = 0, previous_visualisation = 0;
int max_x = 0, max_y = 0;
int min_x = 1024, min_y = 1024;
const int button1pin = 11;
const int button2pin = 12;

int8_t ball[6][4] = {
  {  -1,  0,  1,  0 }, // Initial X,Y pos & velocity for 3 bouncy balls
  { -16, 2,  1, 0 },
  { -31,  4, 1,  0 },
  { -24,  6, 1,  0 },
  { -8,  8, 1,  0 },
  { -5,  10, 1,  0 }
};

uint16_t ballcolor[6] = {
  0x0080, // Green=1 0x0080
  0x0002, // Blue=1 0x0002
  0x1000,  // Red=1 0x1000
  0x0080, // Green=1 0x0080
  0x0002, // Blue=1 0x0002
  0x1000  // Red=1 0x1000
};

void setup() {
  int i;
  matrix.begin();
  for (i = 0; i < 6; i++) {
    ballcolor[i] = matrix.ColorHSV(random(0, 1536), 255, 255, true);
  }
  pinMode(button1pin, INPUT);
  pinMode(button2pin, INPUT);
}

void loop() {
  int x, y, i;
  double angle_z;
  x = analogRead(ACCELX);
  y = analogRead(ACCELY);
  for (i = 0; i < 7; i++) {
    x = (x + analogRead(ACCELX)) / 2 ;
    y = (y + analogRead(ACCELY)) / 2;
  }

update_min_max(x, y);
  angle_z = calc_angle(x, y);
  //update_gravity2(angle_z);
  //update_gravity4(angle_z);
  if (visualisation < 10) {
    update_gravity8(angle_z);
  }

  if (digitalRead(button1pin)) {
    //Change Visualisation
    if (visualisation == 10) {
      visualisation = 2;
    } else {
      visualisation = (visualisation + 1) % 10; //increase this as visualisations are added
      if (visualisation < 3) {
        visualisation = 3;
      }
    }
    matrix.fillScreen(0);
    matrix.swapBuffers(true);
    matrix.fillScreen(0);
    matrix.swapBuffers(true);
    delay(200); //debounce
  }


  

  if (digitalRead(button2pin)) {
    delay(200);
    //Reset the timer
    num_pixels = 0;
    matrix.fillScreen(0);
    matrix.swapBuffers(true);
    matrix.fillScreen(0);
    matrix.swapBuffers(true);
    if (digitalRead(button1pin)) {
      visualisation = 2;
      delay(100); //debounce
    }
  }

  switch (visualisation) {
    //Use ball balancing to calibrate the accelerometer
    case 0:
      gravity_ball();
      break;

    case 1:
      flash_red();
      flash_red();
      flash_red();
      flash_red();
      visualisation = 10; //default
      //previous_visualisation = 2;
      //ball[0][0] = 8;
      //ball[0][1] = 8;
      break;

    case 2:
      while (digitalRead(button1pin) == 0) {
        matrix.fillScreen(0);
        timer_fill(time_total, 0); //red
        matrix.swapBuffers(false);
        if (digitalRead(button2pin)) {
          //Increase the timer length
          time_total = (time_total + 1);
          if (time_total == 61) {
            time_total = 1;
          }
          delay(200); //debounce
        }
      }
      num_pixels = 0;
      visualisation = previous_visualisation;
      delay(200); //debounce
      break;

    case 3:
      hourglass_vis((512 - num_pixels) >> 4);
      if (num_pixels > 256) {
        num_pixels = 0;
        visualisation = 1;
        previous_visualisation = 3;
      }
      matrix.swapBuffers(false);
      delay(63 * time_total - 34); //delay = 62.712x - 33.904
      break;

    case 4: //Random colour fill
      matrix.drawPixel(num_pixels >> 4, num_pixels % 16 , matrix.ColorHSV(random(0, 1536), 255, 255, true)); //Random colour
      matrix.swapBuffers(true);
      num_pixels++;
      if (num_pixels > 511 || num_pixels < 0) {
        num_pixels = 0;
        visualisation = 1;
        previous_visualisation = 4;
      }
      delay(118 * time_total - 8); //delay = 118.51x - 8.3836
      break;

    case 5: //Blue Fill
      matrix.drawPixel(num_pixels >> 4, num_pixels % 16 , 31);
      matrix.swapBuffers(true);
      num_pixels++;
      //time_testing();
      if (num_pixels > 511 || num_pixels < 0) {
        num_pixels = 0;
        visualisation = 1;
        previous_visualisation = 5;
      }
      delay(118 * time_total - 8); //delay = 118.51x - 8.3836
      break;

    case 6:
      adaptive_raindrops((512 - num_pixels) >> 4, time_total);
      if (num_pixels > 511) {
        num_pixels = 0;
        //time_testing();
        visualisation = 1;
        previous_visualisation = 6;
      }
      matrix.swapBuffers(true);
      delay(29 * time_total - 18); //delay = 29.67x - 18.875
      break;

    case 7:
      gravity_ball();
      break;
    case 8:
      helium_ball();
      break;

    case 9:
      raindrops();
      break;

    default:
      //raindrops();
      bubbles_vis();
      break;

  }

}



//------------------------------ Functions---------------------------


void rainbow_fill(int time_total) {
  int i, j, hue;
  // Clear background
  matrix.fillScreen(0);
  for (i = 0 ; i < 32 ; i++) {
    for (j = 0 ; j < 16 ; j++) {
      matrix.drawPixel(i, j, matrix.ColorHSV(hue, 255, 255, true));
      matrix.swapBuffers(true);
      //matrix.swapBuffers(false);
      hue += 3;
      if (hue >= 1536) hue -= 1536;
      delay(30 * time_total); //120~1min, 240~2mins
    }
  }
}

void colour_fill(int time_total, int colour) {
  int i, j;
  // Clear background
  matrix.fillScreen(0);
  for (i = 0 ; i < 32 ; i++) {
    for (j = 0 ; j < 16 ; j++) {
      matrix.drawPixel(i, j, colour);
      matrix.swapBuffers(true);
      //matrix.swapBuffers(false);
      delay(30 * time_total); //120~1min, 240~2mins
    }
  }
}

void add_pixel(int pixel_number, int colour) {
  matrix.drawPixel(pixel_number >> 4, pixel_number % 16 , matrix.ColorHSV(colour, 255, 255, true));
}

void up_fill(int num_pixels, int colour) {
  int i;
  for (i = 511 ; i > 511 - num_pixels ; i--) {
    matrix.drawPixel(i >> 4, i % 16 , colour);
  }
}

void down_fill(int num_pixels, int colour) {
  int i;
  for (i = 0 ; i < num_pixels ; i++) {
    matrix.drawPixel(i >> 4, i % 16 , matrix.ColorHSV(colour, 255, 255, true));
  }
}

void timer_fill(int total_time, int colour) {
  int i;
  for (i = 0 ; i < total_time ; i++) {
    matrix.drawPixel(i / 5, i % 5 , matrix.ColorHSV(i * 20, 255, 255, true));
  }
}

void raindrops() {
  int i, ball_size = 2;
  matrix.fillScreen(0);
  // Bounce three balls around
  for (i = 0; i < 6; i++) {
    // Draw 'ball'
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, ballcolor[i]); //Reading from the ball colour array
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, 0x0002); //Blue raindrops
    matrix.fillCircle(ball[i][0], ball[i][1], ball_size, ballcolor[i]);
    // Update X, Y position
    ball[i][0] += ball[i][2];
    ball[i][1] += ball[i][3];
    // Bounce off edges
    if (ball[i][0] >= matrix.width() + ball_size) {
      ball[i][0] = random(1, 16) - 17;
      ball[i][1] = random(16);
      ball[i][2] = random(1, 3);
      ballcolor[i] = matrix.ColorHSV(random(0, 1536), 255, 255, true); //uncomment this line if using the third fillCircle above
    }
  }
  matrix.swapBuffers(true);
  delay(60);
}

void bubbles_vis() {
  int i, ball_size = 2;
  matrix.fillScreen(0);
  // Bounce three balls around
  for (i = 0; i < 6; i++) {
    // Draw 'ball'
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, ballcolor[i]); //Reading from the ball colour array
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, 0x0002); //Blue raindrops
    matrix.fillCircle(ball[i][0], ball[i][1], ball_size, ballcolor[i]);
    // Update X, Y position
    ball[i][0] += ball[i][2];
    ball[i][1] += ball[i][3];
    // Bounce off edges
    if (ball[i][0] >= matrix.width() + ball_size) {
      ball[i][0] = random(1, 16) - 17;
      ball[i][1] = random(16);
      ball[i][2] = random(1, 3);
      ball[i][3] = 0;
      ballcolor[i] = matrix.ColorHSV(random(0, 1536), 255, 255, true); //uncomment this line if using the third fillCircle above
    }
  }
  matrix.swapBuffers(true);
  delay(60);
}

void adaptive_raindrops(int current_floor, int time_total) {
  int i;
  //draw
  // Clear background
  matrix.fillScreen(0);
  up_fill(num_pixels, 31);
  for (i = 0; i < 6; i++) {
    // Draw 'raindrop'
    matrix.drawPixel(ball[i][0], ball[i][1], 31);
    // Update X, Y position
    ball[i][0] += ball[i][2];
    ball[i][1] += ball[i][3];

    if ((ball[i][1] == - 1) || (ball[i][1] == (matrix.height()))) {
      //ball[i][3] *= -1;
      ball[i][1] -= ball[i][3];
    }

    // Add to the depth of the water
    if (ball[i][0] >= current_floor + 1) {
      ball[i][0] = random(1, 16) - 15;
      ball[i][1] = random(16);
      //ball[i][2] = random(1, 3);
      num_pixels++;
    }
  }
}

void flash_red(void) {
  matrix.fillScreen(63488);
  matrix.swapBuffers(false);
  delay(750);
  matrix.fillScreen(0);
  matrix.swapBuffers(false);
  delay(750);
}

void helium_ball(void) {
  int i, ball_size = 2;
  matrix.fillScreen(0);
  // Bounce three balls around
  for (i = 0; i < 1; i++) {
    // Draw 'ball'
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, ballcolor[i]); //Reading from the ball colour array
    matrix.fillCircle(ball[i][0], ball[i][1], ball_size, 63488); //Red balloon
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, matrix.ColorHSV(ballcolor[i], 255, 255, true));
    // Update X, Y position
    ball[i][0] -= ball[i][2];
    ball[i][1] -= ball[i][3];
    // Bounce off edges
    if ((ball[i][0] == ball_size - 1) || (ball[i][0] == (matrix.width() - ball_size))) {
      //ball[i][2] *= -1;
      ball[i][0] += ball[i][2];
    }
    if ((ball[i][1] == ball_size - 1) || (ball[i][1] == (matrix.height() - ball_size))) {
      //ball[i][3] *= -1;
      ball[i][1] += ball[i][3];
    }
    //Reset Ball
    if ((ball[i][0] < ball_size - 1) || (ball[i][0] > matrix.width() - ball_size)) {
      if ((ball[i][1] < ball_size - 1) || (ball[i][1] > matrix.height() - ball_size)) {
        ball[i][0] = 15;
        ball[i][1] = 8;
        //ball[i][2] = random(1, 3);
        //ballcolor[i] = random(0, 1536); //uncomment this line if using the third fillCircle above
      }
    }
  }
  matrix.swapBuffers(false);
  delay(40);
}

void gravity_ball(void) {
  int i, ball_size = 2;
  matrix.fillScreen(0);
  // Bounce three balls around
  for (i = 0; i < 1; i++) {
    // Draw 'ball'
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, ballcolor[i]); //Reading from the ball colour array
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, 0x0002); //Blue raindrops
    matrix.fillCircle(ball[i][0], ball[i][1], ball_size, 31); //Bright Blue raindrops
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, 65535); //Blue raindrops
    //matrix.fillCircle(ball[i][0], ball[i][1], ball_size, matrix.ColorHSV(ballcolor[i], 255, 255, true));
    // Update X, Y position
    ball[i][0] += ball[i][2];
    ball[i][1] += ball[i][3];
    // Bounce off edges
    if ((ball[i][0] == ball_size - 1) || (ball[i][0] == (matrix.width() - ball_size))) {
      //ball[i][2] *= -1;
      ball[i][0] -= ball[i][2];
    }
    if ((ball[i][1] == ball_size - 1) || (ball[i][1] == (matrix.height() - ball_size))) {
      //ball[i][3] *= -1;
      ball[i][1] -= ball[i][3];
    }
    //Reset Ball
    if ((ball[i][0] < ball_size - 1) || (ball[i][0] > matrix.width() - ball_size)) {
      if ((ball[i][1] < ball_size - 1) || (ball[i][1] > matrix.height() - ball_size)) {
        ball[i][0] = 15;
        ball[i][1] = 8;
        //ball[i][2] = random(1, 3);
        //ballcolor[i] = random(0, 1536/2); //uncomment this line if using the third fillCircle above
      }
    }
  }
  matrix.swapBuffers(false);
  delay(40);
}

void hourglass_vis(int current_floor) {
  //One of the section can go 1 line deeper, but wait until it's in the case.
  //Should figure out how to do the tipping over.  Maybe set the start as 'sand' down the bottom and start once it's flipped.
  int16_t i, sand_colour = 64706, wood_colour = 12288;
  //clear the screen
  matrix.fillScreen(0);

  //Draw the middle triangles
  matrix.fillTriangle(8,  0,  22,  0, 15,  7,  wood_colour);
  matrix.fillTriangle(8,  15,  22,  15, 15,  8,  wood_colour);

  for (i = 0; i < 2; i++) {
    if (ball[i][0] < 16) {
      ball[i][0] = 16;
      ball[i][1] = num_pixels % 2 + 7;
      //ball[i][2] = 1;
      //ball[i][3] = 0;
    }
    //    if ((ball[i][1] < 7) || (ball[i][1] > 8 )) {
    //      ball[i][1] = random(7, 8);
    //    }
    // Draw 'sand' grain
    matrix.drawPixel(ball[i][0], ball[i][1], sand_colour);
    // Update X, Y position
    ball[i][0] += ball[i][2];
    ball[i][1] += ball[i][3];
    //ball[i][0] += 1;
    //ball[i][1] += 0;

    if ((ball[i][1] == - 1) || (ball[i][1] == (matrix.height()))) {
      //ball[i][3] *= -1;
      ball[i][1] -= ball[i][3];
    }
    // Add to the depth of the sand
    if (ball[i][0] >= current_floor + 1) {
      ball[i][0] = 16;
      ball[i][1] = num_pixels % 2 + 7;
      //ball[i][2] = 1;
      //ball[i][3] = 0;
      num_pixels++;
    }
  }
  
  //Draw sand at the top
  matrix.fillRect(0, 0, 8, 16, sand_colour);
  matrix.fillTriangle(8,  1,  8,  7, 14,  7,  sand_colour);
  matrix.fillTriangle(8,  8,  8,  14, 14,  8,  sand_colour);
  //Blank the correct number of pixels
  for (i = 0 ; i < num_pixels + 2 ; i++) {
    if (i < 128) {
      matrix.drawPixel(i >> 4, i % 16 , 0);
    } else if (i >= 128 + 1 && i < 144 - 1) {
      matrix.drawPixel(i >> 4, (i % 16) , 0);
    } else if (i >= 144 + 2 && i < 160 - 2) {
      matrix.drawPixel(i >> 4, (i % 16) , 0);
    } else if (i >= 160 + 3 && i < 176 - 3) { //here
      matrix.drawPixel(i >> 4, (i % 16) , 0);
    } else if (i >= 176 + 4 && i < 192 - 4) {
      matrix.drawPixel(i >> 4, (i % 16) , 0);
    } else if (i >= 192 + 5 && i < 208 - 5) {
      matrix.drawPixel(i >> 4, (i % 16) , 0);
    } else if (i >= 208 + 6 && i < 224 - 6) {
      matrix.drawPixel(i >> 4, (i % 16) , 0);
    } else if (i >= 224 + 7 && i < 240 - 7) {
      matrix.drawPixel(i >> 4, (i % 16) , 0);
    }
  }


  //Draw blank sand at the bottom
  //matrix.fillRect(23, 0, 8, 16, 0);
  //matrix.fillTriangle(22,  1,  22,  7, 16,  7,  0);
  //matrix.fillTriangle(22,  8,  22,  14, 16,  8,  0);
  //Fill the right number of pixels
  for (i = 511 ; i > 511 - num_pixels ; i--) {  // was 495
    if (i > 367) {
      matrix.drawPixel(i >> 4, i % 16 , sand_colour);
    } else if (i >= 352 + 1 && i < 368 - 1) {
      matrix.drawPixel(i >> 4, (i % 16) , sand_colour);
    } else if (i >= 336 + 2 && i < 352 - 2) {
      matrix.drawPixel(i >> 4, (i % 16) , sand_colour);
    } else if (i >= 320 + 3 && i < 336 - 3) { //here
      matrix.drawPixel(i >> 4, (i % 16) , sand_colour);
    } else if (i >= 304 + 4 && i < 320 - 4) {
      matrix.drawPixel(i >> 4, (i % 16) , sand_colour);
    } else if (i >= 288 + 5 && i < 304 - 5) {
      matrix.drawPixel(i >> 4, (i % 16) , sand_colour);
    } else if (i >= 272 + 6 && i < 288 - 6) {
      matrix.drawPixel(i >> 4, (i % 16) , sand_colour);
    } else if (i >= 256 + 7 && i < 272 - 7) {
      matrix.drawPixel(i >> 4, (i % 16) , sand_colour);
    }
  }

//matrix.drawPixel(ball[0][2]+1, ball[0][3]+1, 0x0002);  troubleshooting direction indicator

  //Adjust for missing pixels
  if (num_pixels > 127 && num_pixels < 129) {
    num_pixels = 129;
  } else if (num_pixels > 142 && num_pixels < 146) {
    num_pixels = 146;
  } else if (num_pixels > 157 && num_pixels < 163) {
    num_pixels = 163;
  } else if (num_pixels > 172 && num_pixels < 180) {
    num_pixels = 180;
  } else if (num_pixels > 187 && num_pixels < 197) {
    num_pixels = 197;
  } else if (num_pixels > 202 && num_pixels < 214) {
    num_pixels = 214;
  } else if (num_pixels > 217 && num_pixels < 231) {
    num_pixels = 231;
  } else if (num_pixels > 232) {
    num_pixels = 512;
  }

}



void time_testing(void) {
  //This section is for testing the timing change without physical buttons connected.
  //It can be useful to change the delay multiplication factor before multiplication with time_total
  //to speed up the dev process.
  int i;
  delay(1000);
  time_total += 1;
  if (time_total > 6) {
    time_total = 0;
  }
  //Display number of minutes as dots
  matrix.fillScreen(0);
  matrix.swapBuffers(true);
  for (i = 0 ; i < time_total ; i++) {
    matrix.drawPixel(i >> 4, i % 16 , 0x1000);
    matrix.swapBuffers(true);
  }
  delay(2000);

}

void update_min_max(int x, int y) {
  //declare min_x, min_y, max_x, max_y globally
  if (x < min_x) {
    min_x = x;
  }
  if (y < min_y) {
    min_y = y;
  }
  if (x > max_x) {
    max_x = x;
  }
  if (y > max_y) {
    max_y = y;
  }
}

double calc_angle(int x, int y) {
  double x_rel, y_rel;
  x_rel = (double)map(x, min_x, max_x, -1024, 1024) / 1024; //map the input then fractional -1<x<1
  y_rel = (double)map(y, min_y, max_y, -1024, 1024) / 1024; //map the input then fractional -1<y<1
  return atan2(x_rel, y_rel); //x/y rotates wraparound by 90 degrees compared with y/x
}

void update_gravity2(double world_angle) {
  int i = 0;
  if (world_angle < 0) {
    //Fall down
    for (i = 0; i < 6; i++) {
      ball[i][2] = fabs(ball[i][2]);
    }
  } else {
    //Fall up
    for (i = 0; i < 6; i++) {
      ball[i][2] = fabs(ball[i][2]) * -1;
    }
  }
}

void update_gravity4(double world_angle) {
  //These values are inverted to the ones in the documentation video for code cohesion
  int i = 0;
  if (fabs(world_angle) < 0.785 ) { // pi/4
    //Left
    for (i = 0; i < 6; i++) {
      ball[i][2] = 0;
      ball[i][3] = -1;
    }
  } else if (fabs(world_angle) > 2.356) { // 3pi/4
    //right
    for (i = 0; i < 6; i++) {
      ball[i][2] = 0;
      ball[i][3] = 1;
    }
  } else if (0.785 < world_angle && world_angle < 2.356 ) {
    //down
    for (i = 0; i < 6; i++) {
      ball[i][2] = -1;
      ball[i][3] = 0;
    }
  } else {
    //up
    for (i = 0; i < 6; i++) {
      ball[i][2] = 1;
      ball[i][3] = 0;
    }
  }
}

void update_gravity8(double world_angle) {
  int i = 0;
  //These values are inverted to the ones in the documentation video for code cohesion
  if (fabs(world_angle) < 0.392 ) { // pi/4
    //Left
    for (i = 0; i < 6; i++) {
      ball[i][2] = 0;
      ball[i][3] = -1;
    }
  } else if (fabs(world_angle) > 2.748) { // 3pi/4
    //right
    for (i = 0; i < 6; i++) {
      ball[i][2] = 0;
      ball[i][3] = 1;
    }
  } else if (1.178 < world_angle && world_angle < 1.963 ) {
    //down
    for (i = 0; i < 6; i++) {
      ball[i][2] = -1;
      ball[i][3] = 0;
    }
  } else if (-1.963 < world_angle && world_angle < -1.178 ) {
    //up
    for (i = 0; i < 6; i++) {
      ball[i][2] = 1;
      ball[i][3] = 0;
    }
  } else if (0.392 < world_angle && world_angle < 1.178 ) {
    //down left
    for (i = 0; i < 6; i++) {
      ball[i][2] = -1;
      ball[i][3] = -1;
    }
  } else if (1.963 < world_angle && world_angle < 2.748 ) {
    //down right
    for (i = 0; i < 6; i++) {
      ball[i][2] = -1;
      ball[i][3] = 1;
    }
  } else if (-1.178 < world_angle && world_angle < -0.392 ) {
    //up left
    for (i = 0; i < 6; i++) {
      ball[i][2] = 1;
      ball[i][3] = -1;
    }
  } else if ( -2.748 < world_angle && world_angle < -1.963 ) {
    //up right
    for (i = 0; i < 6; i++) {
      ball[i][2] = 1;
      ball[i][3] = 1;
    }
  }
}

//Old code to reference

//Alternate filling of display
//  for (i = 0 ; i < 512 ; i++) {
//    matrix.drawPixel(i >> 4, i % 16 , 0x1000);
//    matrix.swapBuffers(true);
//    delay(50);
//  }

//  while (!time_reset) {
//    if (digitalRead(button1pin)) {
//      //Increase time
//      time_total += 5;
//      if (time_total > 60){
//        time_total = 0;
//      }
//      //Display number of minutes as dots
//      matrix.fillScreen(0);
//      matrix.swapBuffers(true);
//      for (i = 0 ; i < time_total ; i++) {
//        matrix.drawPixel(i >> 4, i % 16 , 0x1000);
//        matrix.swapBuffers(true);
//      }
//      delay(50);
//    }
//    if (digitalRead(button2pin)) {
//      time_reset = 1;
//    }
//  }
//  time_reset = 0;

