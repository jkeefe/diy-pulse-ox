/*
 
 Original example code notes for the sensor. 
 See: https://learn.sparkfun.com/tutorials/sparkfun-pulse-oximeter-and-heart-rate-monitor-hookup-guide
 -------
 This example sketch gives you exactly what the SparkFun Pulse Oximiter and
 Heart Rate Monitor is designed to do: read heart rate and blood oxygen levels.
 This board requires I-squared-C connections but also connections to the reset
 and mfio pins. When using the device keep LIGHT and CONSISTENT pressure on the
 sensor. Otherwise you may crush the capillaries in your finger which results
 in bad or no results. A summary of the hardware connections are as follows: 
 SDA -> SDA
 SCL -> SCL
 RESET -> PIN 4
 MFIO -> PIN 5

 Author: Elias Santistevan
 Date: 8/2019
 SparkFun Electronics

 If you run into an error code check the following table to help diagnose your
 problem: 
 1 = Unavailable Command
 2 = Unavailable Function
 3 = Data Format Error
 4 = Input Value Error
 5 = Try Again
 255 = Error Unknown

 ----------
 NeoMatrix code developed from original Adafruit example code.
 See: https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library

 ---------
 Final code crafted from the above code by John Keefe in April 2020.
 See: https://johnkeefe.net/building-a-pulse-oximeter

 v2: tinker with Neomatrix library alone
 v3: merge sensor and matrix code
 
*/

#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// --- Sensor Globals --- //

// Reset pin, MFIO pin
int resPin = 4;
int mfioPin = 5;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;  
// ^^^^^^^^^
// What's this!? This is a type (like int, byte, long) unique to the SparkFun
// Pulse Oximeter and Heart Rate Monitor. Unlike those other types it holds
// specific information on your heartrate and blood oxygen levels. BioData is
// actually a specific kind of type, known as a "struct". 
// You can choose another variable name other than "body", like "blood", or
// "readings", but I chose "body". Using this "body" varible in the 
// following way gives us access to the following data: 
// body.heartrate  - Heartrate
// body.confidence - Confidence in the heartrate value
// body.oxygen     - Blood oxygen level
// body.status     - Has a finger been sensed?


// --- Neopixel Matrix globals --- //

#define MATRIX_PIN 3
int dot_dance = 1;

// for deets about this next line, see:
// https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library
// if the display looks backwards or mirrored, this is the issue!
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(5, 8, MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

// declare color variables
// Thanks to Corey Jones's cool example at 
// http://www.hackster.io/jones_corey/christmas-lights-neopixel-led-matrix-glass-block-wall
//uint32_t PEACH = matrix.Color(221,171,127);
//uint32_t ROSY = matrix.Color(255,176,193);
//uint32_t LIGHTBLUE = matrix.Color(94,185,247);
uint32_t RED = matrix.Color(255,0,0);
uint32_t GREEN = matrix.Color(0,255,0);
//uint32_t GRAY = matrix.Color(150,150,150);
//uint32_t BROWN = matrix.Color(117,76,41);
//uint32_t PURPLE = matrix.Color(152,5,229);
//uint32_t DARKGREEN = matrix.Color(12,158,17);
uint32_t PINK = matrix.Color(250,101,148);
//uint32_t ORANGE = matrix.Color(200,90,41);
uint32_t YELLOW = matrix.Color(255,242,0);
uint32_t BLACK = matrix.Color(0,0,0);
//uint32_t WHITE = matrix.Color(255,255,255);
//uint32_t BLUE = matrix.Color(0,0,255);
uint32_t AQUA = matrix.Color(2,132,130);
//uint32_t DARKBLUE = matrix.Color(0,0,125);

// --- Setup code ... this runs once --- //

void setup(){

  /// clear LED matrix
  matrix.begin();
  matrix.setBrightness(40);
  matrix.setCursor(0, 0); // sets the corner where you want to start
  drawDancingDot(AQUA);

  Serial.begin(115200);

  Wire.begin();
  int result = bioHub.begin();
  if (result == 0) {// Zero errors!
    Serial.println("Sensor started!");
    drawDancingDot(AQUA);
  }
  else {
    Serial.println("Could not communicate with the sensor!!!");
  }
 
  Serial.println("Configuring Sensor...."); 
  int error = bioHub.configBpm(MODE_ONE); // Configuring just the BPM settings. 
  if(error == 0){ // Zero errors!
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. Should be about 4 seconds.
  Serial.println("Loading up the buffer with data....");

  for (int i=0; i < 4; i++) {
    drawDancingDot(AQUA);
    delay(1000); 
  }

  
}

// --- main looping function --- //

void loop(){

    // Status codes:
    // 0   No Object Detected
    // 1   Object Detected
    // 2   Object Other Than Finger Detected
    // 3   Finger Detected

    // Information from the readBpm function will be saved to our "body"
    // variable.  
    body = bioHub.readBpm();

    switch (body.status) {
      case 0:
        Serial.println("Got nothing ...");
        matrix.fillScreen(0); // clears the screen
        matrix.show();
        delay(500);
        break;
      case 1:
        Serial.println("Something is happening ...");
        drawDancingDot(GREEN);
        delay(500);
        break;
      case 2:
        Serial.println("Hmmm ... not a finger.");
        drawDancingDot(YELLOW);
        delay(500);
        break;
      case 3:
        Serial.println("Looking good!");
        if (body.confidence > 80 && body.oxygen > 0) {
           
          drawHeart();
          delay(3000);
          Serial.print("Heartrate: ");
          Serial.println(body.heartRate);
          displayValue(body.heartRate, RED);

          drawLungs();
          delay(3000);
          Serial.print("Oxygen: ");
          Serial.println(body.oxygen); 
          displayValue(body.oxygen, PINK);
          
        } else {
          drawFace();
        }
    }

}



// This function takes an integer from 0 to 199 and
// uses drawDigit to display that value on the Neopixel Matrix
void displayValue(int value, uint32_t text_color) {

  matrix.fillScreen(0);

  if (value < 0 || value > 199) {
    Serial.println("Value out of range");
    drawDancingDot(YELLOW);
    return;
  }

  // calculate and draw the ones digit
  int ones_digit = value % 10; // value modulo 10 ... so 24 becomes 4
  drawDigit(ones_digit, 5, text_color); // send the digit, the LED column and the color
  
  // calculate and draw the tens digit,
  // provided value is  greater than 10
  if (value >= 10) {
    int tens_digit = value % 100 / 10; // all are integers, so 124 => 2.4 becomes 2
    drawDigit(tens_digit, 1, text_color); // send the digit, the LED column and the color
  }
 
  matrix.show(); // light up the digits so far!

  // if the value >= 100, flash a 1 in the first (zeroth) led row
  // (because we don't have space to let it stand alone!)
  if (value >= 100) {
    for (int i=0; i < 5; i++){
        matrix.drawLine( 0,0, 4,0, BLACK );
        matrix.show();
        delay(300);
        matrix.drawLine( 0,0, 4,0, text_color );
        matrix.show();
        delay(750);
      }

  } else {
    delay(3000);
  }
  
}

// This function draws a based on the passed value of "digit", 
// the y (led column) position, and the color.
void drawDigit(int digit, int y, uint32_t textcolor) {
    
    // this picks a draw function based on the value of digit.
    // if there is a more efficient way to do this, I'd love to hear it.
    switch (digit) {
        case 1:
            drawOne(y, textcolor);
            break;
        case 2:
            drawTwo(y, textcolor);
            break;
        case 3:
            drawThree(y, textcolor);
            break;
        case 4:
            drawFour(y, textcolor);
            break;
        case 5:
            drawFive(y, textcolor);
            break;
        case 6:
            drawSix(y, textcolor);
            break;
        case 7:
            drawSeven(y, textcolor);
            break;
        case 8:
            drawEight(y, textcolor);
            break;
        case 9:
            drawNine(y, textcolor);
            break;
        case 0:
            drawZero(y, textcolor);
            break;
        // handle the passing of a 10 or 11 in the
        // tens postion for 100+ values
        case 10: 
            drawZero(y, textcolor);
            break;
        case 11:
            drawOne(y, textcolor);
            break;
            
    }
    
}

void drawHeart() {
  matrix.fillScreen(0);
  matrix.drawLine(0,2, 0,6, RED);
  matrix.drawPixel(0,4, BLACK);
  matrix.drawLine(1,1, 1,7, RED);
  matrix.drawLine(2,2, 2,6, RED);
  matrix.drawLine(3,3, 3,5, RED);
  matrix.drawPixel(4,4, RED);
  matrix.show();
}

void drawLungs() {
  matrix.fillScreen(0);
  matrix.drawLine(2,1, 4,1, PINK);
  matrix.drawLine(1,2, 3,2, PINK);
  matrix.drawLine(0,3, 3,3, PINK);
  matrix.drawLine(0,5, 3,5, PINK);
  matrix.drawLine(1,6, 3,6, PINK);
  matrix.drawLine(2,7, 4,7, PINK);
  matrix.drawPixel(2,4, PINK);
  matrix.show();
}

void drawFace() {
  matrix.fillScreen(0);
  matrix.drawPixel(1,0, YELLOW);
  matrix.drawPixel(0,1, YELLOW);
  matrix.drawPixel(1,1, GREEN);
  matrix.drawPixel(0,6, YELLOW);
  matrix.drawPixel(1,7, YELLOW);
  matrix.drawPixel(1,6, GREEN);
  matrix.drawLine(2,3, 2,4, GREEN);
  matrix.drawPixel(3,1, GREEN);
  matrix.drawPixel(3,6, GREEN);
  matrix.drawLine(4,2, 4,5, GREEN);
  matrix.show();
}

void drawDancingDot(uint32_t dot_color) {
  matrix.fillScreen(0);
  dot_dance++;
  int dot_spot = dot_dance % 2;  // will be 1 or 0
  matrix.drawPixel(2,dot_spot+3, dot_color);
  matrix.show();
}


// FUNCTIONS FOR DRAWING THE ACTUAL NUMBERS

// each of the digit functions get passed
// the LED matrix column to start in 
// (which could be 0-7, but we only use 1 and 5)
// and the color to use. 
void drawOne(int y, uint32_t textcolor) {
    matrix.drawLine( 0,y+2, 4,y+2, textcolor ); // y is the upper-left corner's column
}

void drawTwo(int y, uint32_t textcolor) {
    matrix.fillRect( 0,y, 5, 3, textcolor); // make a box ... x,y, h, w
    matrix.drawLine( 1,y, 1,y+1, BLACK); // knock out a piece
    matrix.drawLine( 3,y+1, 3,y+2, BLACK);
}

void drawThree(int y, uint32_t textcolor) {
    matrix.fillRect (0,y, 5,3, textcolor);
    matrix.drawLine( 1,y, 1,y+1, BLACK);
    matrix.drawLine( 3,y, 3,y+1, BLACK);
}

void drawFour(int y, uint32_t textcolor) {
    matrix.fillRect( 0,y, 5, 3, textcolor);
    matrix.drawRect( 3,y, 2, 2, BLACK);
    matrix.drawLine( 0,y+1, 1,y+1, BLACK);
}

void drawFive(int y, uint32_t textcolor) {
    matrix.fillRect( 0,y, 5, 3, textcolor);
    matrix.drawLine( 1,y+2, 1,y+1, BLACK);
    matrix.drawLine( 3,y, 3,y+1, BLACK);
}

void drawSix(int y, uint32_t textcolor){
    matrix.fillRect( 0,y, 5, 3, textcolor);
    matrix.drawLine( 1,y+2, 1,y+1, BLACK);
    matrix.drawPixel( 3,y+1, BLACK);
}

void drawSeven(int y, uint32_t textcolor) {
    matrix.drawLine( 0,y, 0,y+2, textcolor);
    matrix.drawLine( 1,y+2, 4,y+2, textcolor);
}

void drawEight(int y, uint32_t textcolor) {
    matrix.fillRect( 0,y, 5, 3, textcolor);
    matrix.drawPixel( 1,y+1, BLACK);
    matrix.drawPixel( 3,y+1, BLACK);
}

void drawNine(int y, uint32_t textcolor) {
    matrix.fillRect( 0,y, 5, 3, textcolor);
    matrix.drawPixel( 1,y+1, BLACK);
    matrix.drawLine( 3,y, 3,y+1, BLACK);
}

void drawZero(int y, uint32_t textcolor) {
    matrix.drawRect( 0,y, 5, 3, textcolor);
}