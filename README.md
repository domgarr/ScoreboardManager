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
![alt text](https://github.com/domgarr/ScoreboardManager/blob/master/images/volleyball_scoreboard.jpg)

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

# Libraries needed and where to download.
- Arduino IDE setup (https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/setup)
- BLE library (https://learn.adafruit.com/adafruit-feather-32u4-bluefruit-le/installing-ble-library)

