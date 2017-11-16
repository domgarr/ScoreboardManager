
#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include <Adafruit_NeoPixel.h>

#include "BluefruitConfig.h"

#define RED_SCORE_ID "1"
#define BLUE_SCORE_ID "2"


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            5

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS     140

#define ROW 7
#define COL 5

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

/* The service information */
/* int32_t is a data type of exactly 32 bits no more no less. Int can be any size >= 16 bits. */
int32_t sbServiceId;
int32_t sbRedScoreCharId;
int32_t sbBlueScoreCharId;
int32_t connection;



int redScore = 0;
int blueScore = 0;
int prevRedScore = redScore;
int prevBlueScore = blueScore;


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

 const char S_left[] = "01110100011000001110000011000101110";
const char S_right[] = "01110100010000101110100001000101110";
const char P_left[] = "11110100011000110001111100000110000";
const char P_right[] = "10000000011111010001100011000111110";
const char I_left[] = "11111001000010000100001000010011111";
const char I_right[] = "11111001000010000100001000010011111";
const char N_left[] = "10001100011100110101100111000110001";
const char N_right[] = "10001100011001110101110011000110001";

const char *p_to_character; //This will point to the next pixel grid to be drawn.
/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  

  boolean success;
  /*Based off of Adafruit Bluefruit Heart Rate Monitor (HRM) Example */
  
  Serial.begin(9600);

  
  Serial.println(F("Adafruit Bluefruit Scoreboard Monitor (SbM)"));
  Serial.println(F("---------------------------------------------------"));

  randomSeed(micros()); /* Not sure of the purpose of this */

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );


 Serial.println("AT+FACTORYRESET");
  
  /* Perform a factory reset to make sure everything is in a known state 
     Keep this line in for now. Test later it might not be needed.
 */ 

  
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }
      
  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  // this line is particularly required for Flora, but is a good idea
  // anyways for the super long lines ahead!
  // ble.setInterCharWriteDelay(5); // 5 ms

  /* Change the device name to make it easier to find */
  Serial.println(F("Setting device name to 'Bluefruit SbM': "));

  if (! ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Bluefruit SbM")) ) {
    error(F("Could not set device name?"));
  }

  /* Checks if connected to another BLE device, if it is disconnect from it */
 
      ble.sendCommandCheckOK(F("AT+GAPDISCONNECT"));
     


   /* 
  *  This seems necessary to updating the GATT characteristics. Without the addition of this code, any changes
  *  made to GATTADDCHAR didn't seem to have an effect.
  */
  
 if (!ble.sendCommandCheckOK(F("AT+GATTCLEAR")) ) {
    error(F("Could not clear GATT"));
  }

  /* Add the Scoreboard Service definition */
  /* Service ID should be 1 */
  Serial.println(F("Adding the Scoreboard service definition (UUID = 0x2D68): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDSERVICE=UUID128=92-ee-e2-7c-33-3f-4f-49-a8-15-0b-74-5d-81-0c-5d"), &sbServiceId);
  if (! success) {
    error(F("Could not add SbM service"));
  }

  /* Add the Red Score characteristic */
  /* Chars ID for Red Score should be 1 */
  Serial.println(F("Adding the red score characteristic (UUID = 0x9c9c): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0xac3b, PROPERTIES=0x0A, MIN_LEN=1, MAX_LEN=4, VALUE=2, DESCRIPTION= Red Score"), &sbRedScoreCharId);
    if (! success) {
    error(F("Could not add RS characteristic"));
  }

  /* Add the Blue Score characteristic */
  /* Chars ID for Blue Team should be 2 */
  Serial.println(F("Adding the blue score characteristic (UUID = 0x2D70): "));
  success = ble.sendCommandWithIntReply( F("AT+GATTADDCHAR=UUID=0xac3c, PROPERTIES=0x0A, MIN_LEN=1, MAX_LEN=4, VALUE=1, DESCRIPTION= Blue Score"), &sbBlueScoreCharId);
    if (! success) {
    error(F("Could not add BS characteristic"));
  }

  /* Add the Heart Rate Service to the advertising data (needed for Nordic apps to detect the service) */

 
  Serial.print(F("Adding ScoreBoard management Service UUID to the advertising payload: "));
  ble.sendCommandCheckOK( F("AT+GAPSETADVDATA=11-07-5d-0c-81-5d-74-0b-15-a8-49-4f-3f-33-7c-e2-ee-92") );
          



  /* Reset the device for the new service setting changes to take effect */
  Serial.print(F("Performing a SW reset (service changes require a reset): "));
  ble.reset();


ble.setDisconnectCallback(disconnected);
  Serial.println();

   pixels.begin(); // This initializes the NeoPixel library.

  
   //Read as blue-left, blue-right. Parses out the 10s and 1s place. Ex. 25 is split to get 2 and 5.
   char b_l = (blueScore % 100 / 10) + '0';
   char b_r = (blueScore % 10 / 1) + '0';
  //Red-left, red-right
   char r_l = (redScore % 100 / 10) + '0';
   char r_r = (redScore % 10 / 1) + '0';

 
  drawBlock(0, b_l , false, 0, 0, 25);
  drawBlock(35, b_r , true, 0, 0, 25);

  drawBlock(70, r_l , false, 0, 25, 0);
  drawBlock(105, r_r , true, 0, 25, 0);
  pixels.show();
 


}

/** Send randomized heart rate data continuously **/
void loop(void)
{

    ble.print( F("AT+GATTCHAR=") );
    ble.println( sbRedScoreCharId );
   
  
  /* Command is sent when \n (\r) or println is called */
  /* AT+GATTCHAR=CharacteristicID,value */
  /*
  ble.print( F("AT+GATTCHAR=") );
  ble.print( sbRedScoreCharId );
  ble.print( F(",") );
  ble.println(sbBlueScoreCharId, HEX);
*/

 
 ble.sendCommandCheckOK("AT+GATTCHAR=" RED_SCORE_ID);
  

  redScore = atoi(ble.buffer);  

  if(redScore != prevRedScore){
    prevRedScore = redScore;

    if(redScore == 24 && redScore > blueScore + 2){
      drawBlock(0, 'S' , false, 0, 25, 0);
      drawBlock(35, 'P' , true, 0, 25, 0);
      drawBlock(70, 'I' , false, 0, 25, 0);
      drawBlock(105, 'N' , true, 0, 25, 0);
      pixels.show();
      delay(2500);
    }


char r_l = (redScore % 100 / 10) + '0';
   char r_r = (redScore % 10 / 1) + '0';
    drawBlock(70, r_l , false, 0, 25, 0);
  drawBlock(105, r_r , true, 0, 25, 0);
  pixels.show();
  }
  /* Check if command executed OK */
   ble.waitForOK();

   ble.sendCommandCheckOK("AT+GATTCHAR=" BLUE_SCORE_ID);
  

  blueScore = atoi(ble.buffer);  

  if(blueScore != prevBlueScore){
    prevBlueScore = blueScore;


char b_l = (blueScore % 100 / 10) + '0';
   char b_r = (blueScore % 10 / 1) + '0';
    drawBlock(0, b_l , false, 0, 0,25);
  drawBlock(35, b_r , true, 0, 0, 25);
  pixels.show();
  }
  /* Check if command executed OK */
   ble.waitForOK();

   
 
ble.sendCommandWithIntReply( F("AT+GAPGETCONN"),  &connection);

  if( connection == 0){
     ble.sendCommandCheckOK(F("AT+GAPDISCONNECT"));
  }




 
 

  /* Delay before next measurement update */
  delay(2000);
}


void drawBlock(int offset, char character, bool isRight, int g, int r, int b){
  charToDigitalLetterArray(character, isRight);
   for(int i = 0 ; i < 35; i++){
         Serial.print(i);
           if(p_to_character[i] == '1'){
           // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255 
           pixels.setPixelColor(i + offset, pixels.Color(g,r,b)); // Moderately bright green color.
            Serial.println("ON");
        }else{
             pixels.setPixelColor(i + offset, pixels.Color(0,0,0)); //Set LED off.
              Serial.println("OFF");
         }
}
}

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
    case 'S': if(isRight) p_to_character = S_right; 
              else  p_to_character = S_left;
              break;
    case 'P': if(isRight) p_to_character = P_right; 
              else  p_to_character = P_left;
              break;
    case 'I': if(isRight) p_to_character = I_right; 
              else  p_to_character = I_left;
              break;
    case 'N': if(isRight) p_to_character = N_right; 
              else  p_to_character = N_left;
              break;
  }
}

void disconnected(void)
{
  Serial.println( F("Disconnected") );
   ble.sendCommandCheckOK(F("AT+GAPDISCONNECT"));
}

