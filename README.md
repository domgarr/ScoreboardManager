# ScoreboardManager
A Bluetooth scoreboard run off of an Adafruit Feather 32u4 Bluefruit and developed in Arduino IDE

Here is v1 of the scoreboard. Each number is 5X7 and has a special feature that blinks "Spin to Win", a tradition we have at the pickup volleyball.
![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/scoreboard_example.jpg)

## Bluetooth Controllers
There are currently three applications I've developed to control the scoreboard from afar.

### eBoard (https://github.com/domgarr/eboard)
- A web application that utilized Chromes bluetooth API compatability. It only works on Chrome browsers and works for Android/Window. iPhone compatiability is a work in progress.
- hosted @ https://domgarr.github.io/eboard/
- Please note this application was built with mobile in mind. It is not useable otherwise, to view how I meant it to be used. Go to developer console and click Toggle Device Toolbar ( To the left of Elements)
![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/eboard.png)

### Volleyball Scoreboard (https://github.com/domgarr/Volleyball-ScoreKeeper)
- application native to Android.
<img src="https://github.com/domgarr/ScoreboardManager/blob/master/images/volleyball_scoreboard.jpg" width="270" height="555">

### ScoreKeeper? - Samsung Gear Watch (https://github.com/domgarr/ScoreKeeper)
- See the picture of scoreboard v1 above.

#### Note: Why is naming so hard?

# Materials used.
- Adafruit Feather 32u4 BlueFruit (https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/overview)
- ALITOVE WS2811 12mm Diffused Digital RGB LEDs
- Baomain 16mm Push Button Switch Momentary Square Cap (buttons)
- Casing of choice. I used a pine boards of differing size to construct.
- Portable USB charger to power (Note: Most chargers will shut off if not enough power is used. I just increased the intensity of the LEDS until the chragers didn't auto shut off.)
- Dupont Connector Kit and 22 AWG wires worked fine for me.
- Dupont Crimper is important for strong connections.
- Soldering iron
- Breadboard or Protoboard
- Hot glue gun if using a breadboard.
- Drill with 1/2 inch forstner bit.

# Libraries needed and where to download.
- Arduino IDE setup (https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/setup)
- BLE library (https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/installing-ble-library)

# Scoreboard v2
- Uses less LEDs. (4X5 numbers)
![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/front_0.jpg)

# Quick - not in-depth - Tutorial
I started by measuring out the layout of the scoreboard. 
- The holes for the LEDs are 1/2 inch in diameter
- The center of one LED to another is 1 inch. It may be possible to have a smaller length between two LEDs.
- The space between the 1st and 2nd digit of the number is 2 inches.
- The space between to opposing scores is 3 inches.
- There is 1.5 inches margin surrouding the numbers.

![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/outline_holes_drilled.jpg)

- I used 3/4 inch n ft X 2.5 inche wood for the outer casing. Next I drilled holes in the top part of the casing so that the pushbuttons fit in nicely and finish with a glue gun to ensure no movement occurs when pressed. 
- The LEDs fit really nice in the holes made by the forstner bit, no glue is required for holdin the LEDS in place.

Here is the final product with buttons, LEDs, microcontroller and breadboard in place.
The overall structure is 
![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/backside.jpg)

The button placement and spacing is determined by size of scoreboard wanted. To make the buttons more UI friendly a makeshift plus and minus indicators were made with tape :)
![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/top.jpg)

Spray painting the front-side with black non-glossy spraypaint does an okay job of preventing the light from spreading out.
![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/on_back.jpg)

Below are the front 
- Top right red wire: 3v output to the LEDs wired in series.
- To the left of 3v pin : Is the ground. The ground wires from the pushbutton and LEDs are connected to a breadboard, finally finishing the circuit here.
- Bottom: There is 1 white wire: controls the on/off state of the LEDS and the remaining red wires power the pushbuttons. 
![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/micro_front.jpg)

 and backsides of the microcontroller.
 ![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/micro_back.jpg)
 
 The breadboard as mentioned above, contains the grounded wires and one wire to the microcontroller ground pin.
  ![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/breadboard.jpg)
  
  Lastly, the push button.
  - On the C or Common pin : The ground wire is soldered.
  - On the NO or Normally Open: The power is soldered. 
  
  Here I am wrapping the exposed wire with electrical tape. Without any tape, contact made to another wire would make the button act inconsistently.
   ![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/pushbutton.jpg)
   

# Final Product : Scoreboard v2
 ![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/front_10.jpg)
  
  
 

 





