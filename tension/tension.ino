

#include <ArduinoBLE.h>

const int ledPin = LED_BUILTIN; // set ledPin to on-board LED
const int buttonPin = 4; // set buttonPin to digital pin 4

BLEService connectionService("4488b571-7806-4df6-bcff-a2897e4953ff"); // create service
BLEService ledService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"); // create service

BLECharacteristic tensionCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWrite | BLENotify,"");
BLEDescriptor tensionDescriptor("00002902-0000-1000-8000-00805f9b34fb", "");


void setup() {
  Serial.begin(9600);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("Tension Vadim");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(connectionService);

  // add the characteristics to the service
  ledService.addCharacteristic(tensionCharacteristic);

  // add the service
  BLE.addService(connectionService);
  BLE.addService(ledService);

  // tensionCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    
  BLE.poll();
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if (tensionCharacteristic.valueUpdated()) {
                  Serial.println("Value updated!");

        // yes, get the value, characteristic is 1 byte so use byte value
        int32_t value = 0;

        tensionCharacteristic.readValue(value);

        if (value & 0x01) {
          // first bit corresponds to the right button
          Serial.println("1");
        }else if (value & 0x02) {
          // second bit corresponds to the left button
          Serial.println("2");
        }else if (value & 0x03) {
          // second bit corresponds to the left button
          Serial.println("3");
        } else {
          Serial.print("value: ");
          Serial.print(value);

        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
  
