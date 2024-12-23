#include <ArduinoBLE.h>
#include <NeoPixelBus.h>
#include <config.h>

#ifdef GRB
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
#else
NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
#endif

BLEService uartService = BLEService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
BLECharacteristic receiveCharacteristic = BLECharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWriteWithoutResponse, 20);
BLECharacteristic transmitCharacteristic = BLECharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLENotify, 20);


RgbColor red(brightness, 0, 0);
RgbColor green(0, brightness, 0);
RgbColor blue(0, 0, brightness);
RgbColor yellow(additionalledbrightness, additionalledbrightness, 0);
RgbColor cyan(0, brightness, brightness);
RgbColor pink(brightness, 0, brightness/2);
RgbColor violet(brightness/2, 0, brightness);
RgbColor black(0);

int state = 0; // Variable to store the current state of the problem string parser
String problemstring = ""; // Variable to store the current problem string
bool useadditionalled = false; // Variable to store the additional LED setting


void ConnectHandler(BLEDevice central) {
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
  BLE.advertise();
}

void DisconnectHandler(BLEDevice central) {
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
  BLE.advertise();
}

void display(byte incoming[], int length){
// Check if message parts are available on the BLE UART
  for (int i = 0; i <length; i++) {
    char c = incoming[i];

    // State 0: wait for configuration instructions (sent only if V2 option is enabled in app) or beginning of problem string
    if (state == 0) {
      if (c == '~') {
        state = 1; // Switch to state 1 (read configuration)
        continue;
      }
      if (c == 'l') {
        state = 2; // Switch to state 2 (read problem string)
        continue;
      }
    }

    // State 1: read configuration option (sent only if V2 option is enabled in app)
    if (state == 1) { 
      if (c == 'D') {
        useadditionalled = true;
        state = 2; // Switch to state 2 (read problem string)
        continue;
      }
      if (c == 'l') {
        state = 2; // Switch to state 2 (read problem string)
        continue;        
      }
    }

    // State 2: wait for the second part of the beginning of a new problem string (# after the lower-case L)
    if (state == 2) {
      if (c == '#') {
        state = 3; // Switch to state 3
        continue;
      }
    }

    // State 3: store hold descriptions in problem string
    if (state == 3) { 
      if (c == '#') { // problem string ends with #
        state = 4; // Switch to state 4 (start parsing and show LEDs)
        continue;
      }
      problemstring.concat(c); // add current character to problem string
    }
  }
  

  // State 4: complete problem string received, start parsing
  if (state == 4) {
    strip.ClearTo(black); // Turn off all LEDs in LED string
    Serial.println("\n---------");
    Serial.print("Problem string: ");
    Serial.println(problemstring);
    Serial.println("");

    String problemstringstore = problemstring; // store copy of problem string

    if (useadditionalled) { // only render additional LEDs in first loop
      Serial.println("Additional LEDs:");
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
        int additionallednumber = lednumber + additionalledmapping[holdnumber];
        if (additionalledmapping[holdnumber] != 0) {
          Serial.print(holdtype);
          Serial.print(holdnumber);
          Serial.print(" --> ");
          Serial.print(additionallednumber);
          if (holdtype == 'S') { // Start hold
            strip.SetPixelColor(additionallednumber, yellow);
            Serial.println(" (yellow)");
          }
          // Right, left, match, or foot hold
          if (holdtype == 'R' || holdtype == 'L' || holdtype == 'M' || holdtype =='F' || holdtype =='P') {
            strip.SetPixelColor(additionallednumber, yellow);
            Serial.println(" (yellow)");
          }
          // Finish holds don't get an additional LED!
        }        

        if (pos == -1) { // Last hold has been processed!
          Serial.println("");
          break;
        }

        problemstring = problemstring.substring(pos+1, problemstring.length()); // Remove processed hold from string
      }

      problemstring = problemstringstore; // Restore problem string for rendering normal hold LEDs
    }

    Serial.println("Problem LEDs:");
    while(true){ // render all normal LEDs (possibly overriding additional LEDs)
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
      Serial.print(" --> ");
      Serial.print(lednumber);
      if (holdtype == 'S') { // Start hold
        strip.SetPixelColor(lednumber, green);
        Serial.println(" (green)");
      }
      if (holdtype == 'R' || holdtype =='P') { // Right hold
        strip.SetPixelColor(lednumber, blue);
        Serial.println(" (blue)");
      }
      if (holdtype == 'L') { // Left hold
        strip.SetPixelColor(lednumber, violet);
        Serial.println(" (violet)");
      }
      if (holdtype == 'M') { // Match hold
        strip.SetPixelColor(lednumber, pink);
        Serial.println(" (pink)");
      }
      if (holdtype == 'F') { // Foot hold
        strip.SetPixelColor(lednumber, cyan);
        Serial.println(" (cyan)");
      }
      if (holdtype == 'E') { // End hold
        strip.SetPixelColor(lednumber, red);
        Serial.println(" (red)");
      }
      
      if (pos == -1) { // Last hold has been processed!
        strip.Show(); // Light up all hold (and additional) LEDs
        problemstring = ""; // Reset problem string
        useadditionalled = false; // Reset additional LED option
        state = 0; // Switch to state 0 (wait for new problem string or configuration)
        Serial.println("---------\n");
        break;
      }

      problemstring = problemstring.substring(pos+1, problemstring.length()); // Remove processed hold from string
    }
  }
}

void characteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  int length = characteristic.valueLength();
  byte incoming[length];

  characteristic.readValue(incoming, length);

  for (int i = 0; i < length; i++) {
    Serial.print(incoming[i]);
    Serial.print("-");
  }
  Serial.println();

  display(incoming, length);

}

void setup() {
  Serial.begin(9600);


  strip.Begin(); // Initialize LED strip
  strip.Show(); // Good practice to call Show() in order to clear all LEDs

  // Test LEDs by cycling through the colors and then turning the LEDs off again
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
  strip.SetPixelColor(0, yellow);
  for (int i = 0; i < PixelCount; i++) {
    strip.ShiftRight(1);
    strip.Show();
    delay(10);
  }
  strip.SetPixelColor(0, cyan);
  for (int i = 0; i < PixelCount; i++) {
    strip.ShiftRight(1);
    strip.Show();
    delay(10);
  }
  strip.SetPixelColor(0, pink);
  for (int i = 0; i < PixelCount; i++) {
    strip.ShiftRight(1);
    strip.Show();
    delay(10);
  }
  strip.SetPixelColor(0, violet);
  for (int i = 0; i < PixelCount; i++) {
    strip.ShiftRight(1);
    strip.Show();
    delay(10);
  }
  strip.SetPixelColor(0, red);
  for (int i = 0; i < PixelCount; i++) {
    strip.ShiftRight(1);
    strip.Show();
    delay(10);
  }

  strip.ClearTo(black);
  strip.Show();

  if (!BLE.begin()) {
    while (1);
  }
  BLE.setLocalName("Moonboard");
  BLE.setDeviceName("Moonboard");

  BLE.setAdvertisedService(uartService);
  uartService.addCharacteristic(receiveCharacteristic);
  uartService.addCharacteristic(transmitCharacteristic);
  receiveCharacteristic.setEventHandler(BLEWritten, characteristicWritten);
  BLE.addService(uartService);
  BLE.setEventHandler(BLEConnected, ConnectHandler);
  BLE.setEventHandler(BLEDisconnected, DisconnectHandler);

  BLE.advertise();
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {

    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      BLE.poll();
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}