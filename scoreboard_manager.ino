#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include <Adafruit_NeoPixel.h>
#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

//Constants
#define RED_SCORE_ID "1"
#define BLUE_SCORE_ID "2"
#define BRIGHTNESS 25
#define GAMEPOINT 25

/* 
 *  This is the pin at which the Adressable LEDs are connected. Note the pin has to have a tilda, referring
 *  PWN, pulse width modulation, which controls the brightness of the LEDS.
 *  Domenic.
*/
#define PIN            9

/*
 * The amount of LEDs being used for the scoreboard is 140.
 */
#define NUMPIXELS   140

/*
 * Each number on the scoreboard is a 5 column by 7 row grid. 
 */

#define ROW 7
#define COL 5

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);

/*  ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* The service information 
* - int32_t is a data type of exactly 32 bits no more no less. Int can be any size >= 16 bits. 
* - Data in Bluetooth Low Energy devices are grouped by GATT (Generic Attribute Protocol) 
*   services, which can contain many GATT characteristics which contain the actual data.
* - If you want to read up on it more use : https://learn.adafruit.com/introduction-to-bluetooth-low-energy/gatt#services-and-characteristics
*/

/*
 * Here we instantiate a scoreboard Service id, it contains two characteristics to set or read either
 * the red or blue team's score.
 */
int32_t sbServiceId;
int32_t sbRedScoreCharId;
int32_t sbBlueScoreCharId;

/*
  This variable controls ButtonMode, that is instead of the scoreboard being controlled by the smartwatch
  buttons can now be used.
*/
int32_t watchConnected;

/*
 *  The following variables are used to set the score and see if the scores were changed. 
 *  That way we aren't iterating through pixels that haven't changed.
 */
int redScore = 0;
int blueScore = 0;
int prevRedScore = redScore;
int prevBlueScore = blueScore;

/*
 * References for ButtonMode
 */
 int blueAddPin = 6;
 int blueMinusPin = 5;

 int blueAddPrevState = HIGH;
 int blueMinusPrevState = HIGH;

/*
 *  The variables that store the information of whether a LED should be turned off or on.
 *  Note here that there are left and right options. The way I structured the LEDs on the scoreboard
 *  resulted in needing two different orientations. I wrote a help programming in java to reduce some
 *  of the work. It's on my github. 
 */
 const char zero_left[] =  "01110100011001110101110011000101110";
 const char zero_right[] = "01110100011100110101100111000101110";
 const char one_left[] =  "00010011000101001000000100100000010";          
 const char one_right[] = "00010010000001001000010100110000010";
 const char two_left[]= "01110100010000101000001000001011111";
 const char two_right[] = "11111000100010001000000011000101110";
 const char three_left[] = "01110100010000101100000011000101110";
 const char three_right[] = "01110100010000101100000011000101110";
 const char four_left[] = "00011101000100110001111111000000001";
 const char four_right[] = "00001100001111110001010011010000011";
 const char five_left[] = "11111000011000001111000011000011110";
 const char five_right[] = "11110100000000101111100000000111111";
 const char six_left[] ="01110100011000001111100011000101110";
 const char six_right[] = "01110100011000101111100001000101110";
 const char seven_left[] = "11111100000000101000001000010000100";
 const char seven_right[] = "00100001000010001000000011000011111";
 const char eight_left[] = "01110100011000101110100011000101110";
 const char eight_right[] = "01110100011000101110100011000101110"; 
 const char nine_left[] = "01110100011000111110000011000101110";
 const char nine_right[] = "01110100010000111110100011000101110";

//Game Point
const char g_left[] = "00000011101001001001011100100011100";
const char a_right[] = "01110010010111001000011000000000000";
const char m_left[] = "00000000001111010101101011010110101";
const char e_right[] = "01110000011111001001011000000000000";
const char p_left[] = "00000001111001001001111000000110000";
const char o_with_i_right[] = "01101110011001111001011010000000001";
const char n_left[] = "00000000001110001001100100100110010";
const char t_right[] = "00110000100100000010111100001000000";

//Spin to Win
const char S_left[] = "01110100011000001110000011000101110";
const char p_right[] = "10000000011110001001100100011100000";
const char i_left[] = "00100000000010000100001000010000100";
const char n_right[] = "10010010011001001001111000000000000";
const char t_left[] = "00000000101111000010010000001000110";
const char o_right[] = "01100010011001001001011000000000000";
const char W_left[] = "10001100011000110001101011101110001";
//i_with_n_right[]
const char z_right[] = "11001100111100110011111100000010000";

//A pointer that will reference the next number to display on the scoreboard.
const char *p_to_character; //This will point to the next pixel grid to be drawn.

void setup(void)
{
  delay(2000);

  boolean success;
  /*Based off of Adafruit Bluefruit Heart Rate Monitor (HRM) Example */
  
  Serial.begin(115200);

  
  //Serial.println(F("Adafruit Bluefruit Scoreboard Monitor (SbM)"));
 // Serial.println(F("---------------------------------------------------"));

  randomSeed(micros()); /* Not sure of the purpose of this */

  /* Initialise the module */
  //Serial.print(F("Initialising the Bluefruit LE module: "));

  ble.begin(VERBOSE_MODE) ;
 

   ble.info();
  Serial.println( F("OK!") );
  
  /* Perform a factory reset to make sure everything is in a known state 
     Keep this line in for now. Test later it might not be needed.
 */ 

  Serial.println("AT+FACTORYRESET");
  
  //Serial.println(F("Performing a factory reset: "));
 ble.factoryReset();
 
      
  /* Disable command echo from Bluefruit */
  ble.echo(false);

  /* Change the device name to make it easier to find */
  Serial.println(F("Setting device name to 'Bluefruit SbM': "));

   ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Bluefruit SbM"));

  /* Checks if connected to another BLE device, if it is disconnect from it */
       ble.sendCommandCheckOK(F("AT+BLEPOWERLEVEL=4"));
       ble.sendCommandCheckOK(F("AT+GAPDISCONNECT"));

     
   /* 
  *  This seems necessary to updating the GATT characteristics. Without the addition of this code, any changes
  *  made to GATTADDCHAR didn't seem to have an effect.
  */
  
 ble.sendCommandCheckOK(F("AT+GATTCLEAR")) ;
  

  /* Add the Scoreboard Service definition */
  /* Service ID should be 1 */
  Serial.println(F("Adding the Scoreboard service definition (UUID = 0x2D68): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDSERVICE=UUID128=92-ee-e2-7c-33-3f-4f-49-a8-15-0b-74-5d-81-0c-5d"), &sbServiceId);


  /* Add the Red Score characteristic */
  /* Chars ID for Red Score should be 1 */
  Serial.println(F("Adding the red score characteristic (UUID = 0x9c9c): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0xac3b, PROPERTIES=0x0A, MIN_LEN=1, MAX_LEN=4, VALUE=0, DESCRIPTION= Red Score"), &sbRedScoreCharId);
 
  /* Add the Blue Score characteristic */
  /* Chars ID for Blue Team should be 2 */
  Serial.println(F("Adding the blue score characteristic (UUID = 0x2D70): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0xac3c, PROPERTIES=0x0A, MIN_LEN=1, MAX_LEN=4, VALUE=0, DESCRIPTION= Blue Score"), &sbBlueScoreCharId);
  

  /* Add the Scoreboard managing service to the advertising payload */

 
 // Serial.print(F("Adding ScoreBoard management Service UUID to the advertising payload: "));
  ble.sendCommandCheckOK( F("AT+GAPSETADVDATA=11-07-5d-0c-81-5d-74-0b-15-a8-49-4f-3f-33-7c-e2-ee-92") );
          
  /* Reset the device for the new service setting changes to take effect */
 // Serial.print(F("Performing a SW reset (service changes require a reset): "));
  ble.reset();

  

  pixels.begin(); // This initializes the NeoPixel library.

  //By default the scoreboard initialized with four zero's this does that.
  drawBlock(0, '0' , false, 0, 0, BRIGHTNESS);
  drawBlock(35,'0' , true, 0, 0, BRIGHTNESS);
  drawBlock(70, '0' , false, 0, BRIGHTNESS, 0);
  drawBlock(105, '0' , true, 0, BRIGHTNESS, 0);
  
  pixels.show();

  //Instantiate Pins for ButtonMode incase it is used.
  pinMode(blueAddPin, INPUT_PULLUP);
  pinMode(blueMinusPin, INPUT_PULLUP);
}

/** Send randomized heart rate data continuously **/
void loop(void)
{
  
  ble.sendCommandWithIntReply(F("AT+GAPGETCONN"), &watchConnected);
  //If watch is connected to scoreboard, switch to button controls.
  if( watchConnected == 0){
    
    if(digitalRead(blueAddPin) == LOW){
      if(blueAddPrevState == HIGH){
        blueAddPrevState = LOW;
         blueScore++;
         delay(10);
         renderBlueScore();
      }
    }else{
      blueAddPrevState = HIGH;
    }

    if(digitalRead(blueMinusPin) == LOW){
      if(blueMinusPrevState == HIGH){
        blueMinusPrevState = LOW;
         blueScore--;
        delay(10);
        renderBlueScore();
      }
    }else{
       blueMinusPrevState = HIGH;
    }
    
  }else{
    //Start red side logic.
    //Check if Blue Score characteristic has changed.
     if(ble.sendCommandCheckOK("AT+GATTCHAR=" BLUE_SCORE_ID)){
        blueScore = atoi(ble.buffer);  
     }
  
      //Check if Red Score characteristic has changed.
    if(ble.sendCommandCheckOK("AT+GATTCHAR=" RED_SCORE_ID)){
      redScore = atoi(ble.buffer);  
    }

    renderBlueScore();
    renderRedScore();

  }
 }

/*
 * Sets a block - which is a one of the four sets of grids that display a character - to a desired character  
 * 
 * Scoreboard...
 * 
 *  [L] [R] m [L] [R]
 * 
 * m - is the center of the scoreboard
 * [] - denotes a block, contains a number or character
 * L/R - left or right needs to be specified, since due to wiring the four blocks differ.
 * 
 * @param offset An integer that should be set to either 0, 35, 70, 105 that represents the first to fourth character on the scoreboard starting at the left.
 * @param character A char representing a number or chracter.
 * @param isRight A boolean value that needs to specify wheter the block is on the left or right side of the scoreboard. 
 * @param r - Sets the intensity of red in the LED between 0 - 255  
 * @param g - Sets the intensity of green in the LED between 0 - 255  
 * @param b - Sets the intensity of blue in the LED between 0 - 255  
 */
void drawBlock(int offset, char character, bool isRight, int g, int r, int b){
  charToDigitalLetterArray(character, isRight);
   for(int i = 0 ; i < 35; i++){
           if(p_to_character[i] == '1'){
           // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255 
           pixels.setPixelColor(i + offset, pixels.Color(g,r,b)); // Moderately bright green color.
        }else{
             pixels.setPixelColor(i + offset, pixels.Color(0,0,0)); //Set LED off.
         }
  }
}

/*
 * This function is used by drawBlock to set the pointer's reference  (p_to_character)
 * to the next desired number or character. 
 * @param character  a character that matches with a case to a implemented character mapping
 * @param isRight  will draw the character for the left side is set to false, or the right side if set to true
 */
void charToDigitalLetterArray (char character, bool isRight){
  switch(character){
    case '0': if(isRight) p_to_character = zero_right; 
              else  p_to_character = zero_left;
              break;
    case '1': if(isRight) p_to_character = one_right; 
              else  p_to_character = one_left;
              break;
    case '2': if(isRight) p_to_character = two_right; 
              else  p_to_character = two_left;
              break;
    case '3': if(isRight) p_to_character = three_right; 
              else  p_to_character = three_left;
              break;
    case '4': if(isRight) p_to_character = four_right; 
              else  p_to_character = four_left;
              break;
    case '5': if(isRight) p_to_character = five_right; 
              else  p_to_character = five_left;
              break;
    case '6': if(isRight) p_to_character = six_right; 
              else  p_to_character = six_left;
              break;  
    case '7': if(isRight) p_to_character = seven_right; 
              else  p_to_character = seven_left;
              break;
    case '8': if(isRight) p_to_character = eight_right; 
              else  p_to_character = eight_left;
              break;
    case '9': if(isRight) p_to_character = nine_right; 
              else  p_to_character = nine_left;
              break;
    case 'S': p_to_character = S_left; 
              break;
    case 'i': p_to_character = i_left;
              break;
    case 'o': p_to_character = o_right; 
              break;
    case 'W': p_to_character = W_left; 
              break;
    case 'z': p_to_character = z_right; 
              break;
    case 'g':  p_to_character = g_left;
              break;
    case 'a':  p_to_character = a_right;          
              break;
    case 'm':  p_to_character = m_left;
              break;
    case 'e':  p_to_character = e_right; 
              break;
    case 'p': if(isRight) p_to_character = p_right;
              else p_to_character = p_left;
              break;
    case 'x': p_to_character = o_with_i_right;
              break;
    case 'n': if(isRight) p_to_character = n_right;
              else p_to_character = n_left;
              break;
    case 't':  if(isRight) p_to_character = t_right; 
              else p_to_character = t_left;
              break;
  }
}

/*
 * Displays game point on the scoreboard. Flashes game for two seconds than point for two seconds.
 * @param r - Sets the intensity of red in the LED between 0 - 255  
 * @param g - Sets the intensity of green in the LED between 0 - 255  
 * @param b - Sets the intensity of blue in the LED between 0 - 255 
 */
void drawGamePoint(int g, int r, int b){
      drawBlock(0, 'g' , false, g,r,b);
      drawBlock(35, 'a' , true, g,r,b);
      drawBlock(70, 'm' , false, g,r,b);
      drawBlock(105, 'e' , true, g,r,b);
      pixels.show();
      delay(2000);
      drawBlock(0, 'p' , false, g,r,b);
      drawBlock(35, 'o' , true, g,r,b);
      drawBlock(70, 'x' , false, g,r,b);
      drawBlock(105, 't' , true, g,r,b);
      pixels.show();
      delay(2000);
}

/*
 * Displays "Spin to Win" on the scoreboard. Flashes "Spin" for 1.5 seconds than "to Win" for 1.5 seconds.
 * @param r - Sets the intensity of red in the LED between 0 - 255  
 * @param g - Sets the intensity of green in the LED between 0 - 255  
 * @param b - Sets the intensity of blue in the LED between 0 - 255 
 */
void drawSPIN(int g, int r, int b){
      drawBlock(0, 'S' , false, g,r,b);
      drawBlock(35, 'p' , true, g,r,b);
      drawBlock(70, 'i' , false, g,r,b);
      drawBlock(105, 'n' , true, g,r,b);
      pixels.show();
      delay(1500);
      drawBlock(0, 't' , false, g,r,b);
      drawBlock(35, 'o' , true, g,r,b);
      drawBlock(70, 'W' , false, g,r,b);
      drawBlock(105, 'z' , true, g,r,b);
      pixels.show();
      delay(1500);
}

void renderBlueScore(){
 /*
   * Only if there was a change in the bluescore value. As of now only the Samsung S2 gear watch can do so.
   *  will the following run.
  */
  if(blueScore != prevBlueScore){
    prevBlueScore = blueScore;

 /*
  * This stops "Spin to Win" from displaying after the game is won.
  */
  if(blueScore >= GAMEPOINT && blueScore >= redScore + 2){
      //Game is won, do nothing. Temporary fix to "Spin to Win" flashing after the game was won.
  }else if(blueScore >= GAMEPOINT-1 && blueScore > redScore ){
      //Possible game, display the house rule of "Spin to Win"
      drawSPIN(0,0,BRIGHTNESS);
      //This is needed for the red score side to update, and return back to the score value.
      prevRedScore--;
    }

   //Using modulus operation here to seperate our double digit into single digit characters.
   char b_l = (blueScore % 100 / 10) + '0';
   char b_r = (blueScore % 10 / 1) + '0';
   //Alters the blue side pixels.
   drawBlock(0, b_l , false, 0, 0,BRIGHTNESS);
   drawBlock(35, b_r , true, 0, 0, BRIGHTNESS);
   
   pixels.show();
  }
}

void renderRedScore(){
  //Start of red side logic. It is exactly the same to the blue side.
  if(redScore != prevRedScore){
    prevRedScore = redScore;
    
    if(redScore >= GAMEPOINT && redScore >= blueScore + 2){
    //do nothing
    }else if(redScore >= GAMEPOINT - 1 && redScore > blueScore ){
     drawSPIN(0,BRIGHTNESS,0);
      prevBlueScore--;
    }
    
   char r_l = (redScore % 100 / 10) + '0';
   char r_r = (redScore % 10 / 1) + '0';
  
   drawBlock(70, r_l , false, 0, BRIGHTNESS, 0);
   drawBlock(105, r_r , true, 0, BRIGHTNESS, 0);
  
  pixels.show();
  }
}

