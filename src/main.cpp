#include <HardwareBLESerial.h>
#include <NeoPixelBus.h>

HardwareBLESerial &bleSerial = HardwareBLESerial::getInstance();

const uint16_t PixelCount = 200; // Number of LEDs in the LED strip (usually 150 for MoonBoard Mini, 200 for normal MoonBoard)
const uint8_t PixelPin = 2;  // Use pin D2 of Arduino Nano 33 BLE
#define brightness 64 // 0 to 255
NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin); // Rgb or Grb, depending on your LED strip

RgbColor red(brightness, 0, 0);
RgbColor green(0, brightness, 0);
RgbColor blue(0, 0, brightness);
RgbColor black(0);

int state = 0; // Variable to store the current state of the problem string parser
String problemstring = ""; // Variable to store the current problem string
int ledmapping[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199};

void setup() {
  Serial.begin(9600);

  if (!bleSerial.beginAndSetupBLE("MoonBoard A")) { // Initialize BLE UART and check if it is successful
    // This should never happen as it means that the BLE setup failed and the program cannot run!
    while (true) {
      Serial.println("BLE setup failed!");
      delay(1000);
    }
  }

  strip.Begin(); // Initialize LED strip
  strip.Show(); // Good practice to call Show() in order to clear all LEDs

  // Test LEDs by cycling through the colors red, green, blue and the turning the LEDs off again
  strip.SetPixelColor(0, red);
  for (int i = 0; i < PixelCount; i++) {
    strip.ShiftRight(1);
    strip.Show();
    delay(10);
  }
  strip.SetPixelColor(0, green);
  for (int i = 0; i < PixelCount; i++) {
    strip.ShiftRight(1);
    strip.Show();
    delay(10);
  }
  strip.SetPixelColor(0, blue);
  for (int i = 0; i < PixelCount; i++) {
    strip.ShiftRight(1);
    strip.Show();
    delay(10);
  }

  strip.ClearTo(black);
  strip.Show();

  // Wait for the MoonBoard App to connect
  while (!bleSerial);
}

void loop() {
  // Read messages from the BLE UART
  bleSerial.poll();
  
  // Check if message parts are available on the BLE UART
  while (bleSerial.available() > 0) {
    char c = bleSerial.read();

    // State 0: wait for the beginning of a new problem string (starts with lower-case L)
    if (state == 0) { 
      if (c == 'l') {
        state = 1; // Switch to state 1
        continue;
      }
    }

    // State 1: wait for the second part of the beginning of a new problem string (# after the lower-case L)
    if (state == 1) {
      if (c == '#') {
        state = 2; // Switch to state 2
        continue;
      }
    }

    // State 2: store hold descriptions in problem string
    if (state == 2) { 
      if (c == '#') { // problem string ends with #
        state = 3; // Switch to state 3
        continue;
      }
      problemstring.concat(c); // add current character to problem string
    }
  }

  // State 3: complete problem string received, start parsing
  if (state == 3) {
    strip.ClearTo(black); // Turn off all LEDs in LED string
    Serial.println(problemstring);
    while(true){
      int pos = problemstring.indexOf(','); // Hold descriptions are separated by a comma (,)
      
      String hold;
      if (pos > 0) { // Still holds left in the problem string
        hold = problemstring.substring(0, pos); // Extract one hold description
      }
      else { // Last hold in the problem string
        hold = problemstring; 
      }

      char holdtype = hold.charAt(0); // Hold descriptions consist of a hold type (S, P, E) ...
      int holdnumber = hold.substring(1).toInt(); // ... and a hold number
      int lednumber = ledmapping[holdnumber];
      Serial.print(holdtype);
      Serial.print(holdnumber);
      Serial.print("-->");
      Serial.println(lednumber);
      if (holdtype == 'S') { // Start hold
        strip.SetPixelColor(lednumber, green);
      }
      if (holdtype == 'P') { // Progress hold
        strip.SetPixelColor(lednumber, blue);
      }
      if (holdtype == 'E') { // End hold
        strip.SetPixelColor(lednumber, red);
      }
      
      if (pos == -1) { // Last hold has been processed!
        strip.Show(); // Light up all necessary LEDs to show holds
        problemstring = ""; // Reset problem string
        state = 0; // Switch to state 0 (wait for new problem string)
        break;
      }

      problemstring = problemstring.substring(pos+1, problemstring.length()); // Remove processed hold from string
    }
  }
}