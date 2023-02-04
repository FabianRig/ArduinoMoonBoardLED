# ArduinoMoonBoardLED: A Simple MoonBoard BLE LED System

This project aims at providing an easy to use solution for building your own MoonBoard LED system. It is both compatible with a normal MoonBoard as well as with the MoonBoard Mini. You need an Arduino Nano 33 BLE, a WS2811 LED string with 25 cm wire length, and an appropriate power source. 

![mini_benchmark](https://user-images.githubusercontent.com/88741530/129411463-636b222b-e963-4542-a1dd-b33eae562bf2.jpg)

If you want a product that just works, please buy the one offered by Moon Climbing! This is a project which requires some work and still might not work as well as the original. To be absolutely clear: This project is provided as-is. I take absolutely no responsibility that it works as expected. In fact, it might break at any time. You have been warned!

## Thanks
All the heavy lifting in this project is done by two awesome libraries: NeoPixelBus (for the LED string) and HardwareBLESerial (for BLE functionality). They make it possible to keep this project quite short, easy to understand, and easily maintainable.

## Wiring

The Led strip has three wires: +5V, GND and data. Usually, blue is GND/negative, brown is positive, yellow/green is data. 
Please double-check! Connect the data line to pin D2 of the Arduino (unless you changed it in config.h). 
It might be a good idea to use a resistor (e.g. 330 ohms) in the data line!

## How to use (Windows)
1. Download and install Visual Studio Code.
2. Install PlatformIO in Visual Studio Code.
3. Download and open this project.
4. Adjust config.h settings to your needs (Moonboard type, brightness).
5. Compile and flash to an Arduino Nano 33 BLE.
6. Use the MoonBoard app to connect to the Arduino and show the problems on your board!

## How to use (Linux, command line)

1. Download and install platformio
```
python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"

```
2. Download, install and configure arduino-cli
```
mkdir ~/apps/arduino-cli; cd ~/apps/arduino-cli
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
~/apps/arduino-cli/bin/arduino-cli core install arduino:mbed_nano
```
3. Clone the repository
Press the greem "<> Code" button on github and copy the HTTPS url.
If you don't have `git` installed in your Linux distribution, please install it first.
```
mkdir ~/src; cd ~/src
git clone URL
cd ArduinoMoonBoardLED/
```
4. Adjust config.h
Follow the instructions in the file ~/src/ArduinoMoonBoardLED/src/config.h in order to adjust the configuration to your setup.

5. Compile
```
~/.platformio/penv/bin/platformio run
```

6. Attach the Arduino Nano BLE to your computer and upload the firmware
```
~/apps/arduino-cli/bin/arduino-cli upload -p /dev/ttyACM0 -i .pio/build/nano33ble/firmware.bin
```

7. Optional: connect to the console output of the Arduino to see connections and problems
```
~/apps/arduino-cli/bin/arduino-cli monitor -p /dev/ttyACM0
```


## LED Mapping
The most common LED wiring pattern (here for a MoonBoard Mini) goes like this (front view):
- start bottom left (A1),
- up the column (to A12),
- one column to the right (to B12),
- all the way down (to B1),
- one column to the right (to C1),
- and repeat.

The MoonBoard App encodes holds in the same way. Hold A1 is 0, hold A2 is 1, hold A3 is 2 and so on.

The array ledmapping[] is used to map hold numbers to LED numbers. You can use custom wiring patterns if necessary. The standard mapping is n-->n (e.g. hold #0 is LED #0, hold #1 is LED #1 and so on).

## Good to know
- Never power the Arduino only when it's connected to the LED string without powering the LED string! This might destroy the first LED!
- The Arduino does not need to be shutdown, you can simply unplug the power source! This is (at least for me) a big improvement when compared to a Raspberry Pi based solution.

## Pictures
![led_test](https://user-images.githubusercontent.com/88741530/129411527-84e11098-1192-4a29-a052-b712ad3ca17c.jpg)
![mini_back](https://user-images.githubusercontent.com/88741530/129411542-174241cc-c898-45f9-965f-cb75b34a1869.jpg)
![box](https://user-images.githubusercontent.com/88741530/129411548-5262a5c9-6b06-4bc8-9d68-e9e363823fdb.jpg)
![arduino_wiring](https://user-images.githubusercontent.com/88741530/129411556-ead82345-6687-478b-b9a4-95f1d1ac40c2.jpg)

