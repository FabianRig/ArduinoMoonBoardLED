#include <HardwareBLESerial.h>
#include <NeoPixelBus.h>

// constants
#define LED_BRIGHTNESS 64            // 0 to 255
#define ADDITIONAL_LED_BRIGHTNESS 32 // 0 to 255
#define STATE_WAIT_CONFIGURATION 0
#define STATE_READ_CONFIGURATION 1
#define STATE_WAIT_PROBLEM 2
#define STATE_READ_PROBLEM 3
#define STATE_RENDER_PROBLEM 4
const int c_boardStandard = 0;
const int c_boardMini = 1;

// custom settings
int board = c_boardMini;       // Define the board type : mini or standard (to be changed depending of board type used)
const uint8_t c_pixelPin = 2;  // Use pin D2 of Arduino Nano 33 BLE (to be changed depending of your pin number used)
const int c_ledOffset = 2;     // Use every "c_ledOffset" LED on the string
const bool c_checkLeds = true; // Test the led sysem at boot if true

// variables used by the project
int ledsByBoard[] = {200, 150};                                           // Led: usually 150 for MoonBoard Mini, 200 for a standard MoonBoard
int rowsByBoard[] = {18, 12};                                             // Rows: usually 12 for MoonBoard Mini, 18 for a standard MoonBoard
String namesByBoard[] = {"Moonboard Standard", "Moonboard Mini"};         // Board names
uint16_t ledCount = ledsByBoard[board] * c_ledOffset;                     // Number of LEDs in the LED strip (usually 150 for MoonBoard Mini, 200 for a standard MoonBoard)
HardwareBLESerial &bleSerial = HardwareBLESerial::getInstance();          // BLE serial instance
NeoPixelBus<NeoRgbFeature, Neo800KbpsMethod> strip(ledCount, c_pixelPin); // Rgb or Grb, depending on your LED strip
RgbColor red(LED_BRIGHTNESS, 0, 0);                                       // Red color
RgbColor green(0, LED_BRIGHTNESS, 0);                                     // Green color
RgbColor blue(0, 0, LED_BRIGHTNESS);                                      // Blue color
RgbColor violet(ADDITIONAL_LED_BRIGHTNESS, 0, ADDITIONAL_LED_BRIGHTNESS); // Violet color
RgbColor black(0);                                                        // Black color
int *ledMapping = NULL;                                                   // Led coordinates
int *additionalLedMapping = NULL;                                         // Additional led offset from ledMapping
int state = STATE_WAIT_CONFIGURATION;                                     // Variable to store the current state of the problem string parser
String problemstring = "";                                                // Variable to store the current problem string
bool useAdditionalLed = false;                                            // Variable to store the additional LED setting

/**
 * @brief Initialize mapping variables
 *
 */
void setupVariables()
{
  int rows = rowsByBoard[board];

  ledMapping = new int[ledCount];
  additionalLedMapping = new int[ledCount];
  for (int i = 0; i < ledCount; i++)
  {
    ledMapping[i] = i;
    int cell = i + 1;
    int column = (cell / rows) + 1;

    if ((cell % rows == 0) || ((cell - 1) % rows == 0)) // start or end of the column
      additionalLedMapping[i] = 0;
    else if (column % 2 == 0) // even column
      additionalLedMapping[i] = -1;
    else if (column % 2 == 1) // odd column
      additionalLedMapping[i] = 1;
    else
      additionalLedMapping[i] = 9;
  }
}

/**
 * @brief Initialze builtin leds of arduino
 *
 */
void setupBuiltinLed()
{
  // set LED's pin to output mode
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LEDG, HIGH); // will turn the LED green OFF
  digitalWrite(LEDR, HIGH); // will turn the LED red OFF
  digitalWrite(LEDB, HIGH); // will turn the LED blue OFF
}

/**
 * @brief Test LEDs by cycling through the colors red, green, blue, violet and then turning the LEDs off again
 *
 */
void setupBoardLeds()
{
  strip.Begin(); // Initialize LED strip
  strip.Show();  // Good practice to call Show() in order to clear all LEDs

  if (c_checkLeds)
  {
    strip.SetPixelColor(0, red);
    for (int i = 0; i < ledCount; i++)
    {
      strip.ShiftRight(1 * c_ledOffset);
      strip.Show();
      delay(10);
    }
    strip.SetPixelColor(0, green);
    for (int i = 0; i < ledCount; i++)
    {
      strip.ShiftRight(1 * c_ledOffset);
      strip.Show();
      delay(10);
    }
    strip.SetPixelColor(0, blue);
    for (int i = 0; i < ledCount; i++)
    {
      strip.ShiftRight(1 * c_ledOffset);
      strip.Show();
      delay(10);
    }
    strip.SetPixelColor(0, violet);
    for (int i = 0; i < ledCount; i++)
    {
      strip.ShiftRight(1);
      strip.Show();
      delay(10);
    }
  }

  strip.ClearTo(black);
  strip.Show();
}

/**
 * @brief Initialize the BLE module
 *
 */
void setupBle()
{
  if (!bleSerial.beginAndSetupBLE("MoonBoard A"))
  { // Initialize BLE UART and check if it is successful
    // This should never happen as it means that the BLE setup failed and the program cannot run!
    while (true)
    {
      Serial.println("BLE setup failed!");
      delay(1000);
    }
  }
}

/**
 * @brief render the received problem on the moonboard
 *
 */
void showProblem()
{
  strip.ClearTo(black); // Turn off all LEDs in LED string

  String problemstringstore = problemstring; // store copy of problem string

  if (useAdditionalLed)
  { // only render additional LEDs in first loop
    Serial.println("Additional LEDs:");
    while (true)
    {
      int pos = problemstring.indexOf(','); // Hold descriptions are separated by a comma (,)

      String hold;
      if (pos > 0)
      {                                         // Still holds left in the problem string
        hold = problemstring.substring(0, pos); // Extract one hold description
      }
      else
      { // Last hold in the problem string
        hold = problemstring;
      }

      char holdtype = hold.charAt(0);             // Hold descriptions consist of a hold type (S, P, E) ...
      int holdnumber = hold.substring(1).toInt(); // ... and a hold number
      int lednumber = ledMapping[holdnumber];
      int additionalLedNumber = lednumber + additionalLedMapping[holdnumber];
      if (additionalLedMapping[holdnumber] != 0)
      {
        Serial.print(holdtype);
        Serial.print(holdnumber);
        Serial.print(" --> ");
        Serial.print(additionalLedNumber);
        if (holdtype == 'S' || holdtype == 'P')
        { // Start or Progress hold
          strip.SetPixelColor(additionalLedNumber * c_ledOffset, violet);
          Serial.println(" (violet)");
        }
        // Finish holds don't get an additional LED!
      }

      if (pos == -1)
      { // Last hold has been processed!
        Serial.println("");
        break;
      }

      problemstring = problemstring.substring(pos + 1, problemstring.length()); // Remove processed hold from string
    }

    problemstring = problemstringstore; // Restore problem string for rendering normal hold LEDs
  }

  Serial.println("Problem LEDs:");
  while (true)
  {                                       // render all normal LEDs (possibly overriding additional LEDs)
    int pos = problemstring.indexOf(','); // Hold descriptions are separated by a comma (,)

    String hold;
    if (pos > 0)
    {                                         // Still holds left in the problem string
      hold = problemstring.substring(0, pos); // Extract one hold description
    }
    else
    { // Last hold in the problem string
      hold = problemstring;
    }

    char holdtype = hold.charAt(0);             // Hold descriptions consist of a hold type (S, P, E) ...
    int holdnumber = hold.substring(1).toInt(); // ... and a hold number
    int lednumber = ledMapping[holdnumber];
    Serial.print(holdtype);
    Serial.print(holdnumber);
    Serial.print(" --> ");
    Serial.print(lednumber);
    if (holdtype == 'S')
    { // Start hold
      strip.SetPixelColor(lednumber * c_ledOffset, green);
      Serial.println(" (green)");
    }
    if (holdtype == 'P')
    { // Progress hold
      strip.SetPixelColor(lednumber * c_ledOffset, blue);
      Serial.println(" (blue)");
    }
    if (holdtype == 'E')
    { // End hold
      strip.SetPixelColor(lednumber * c_ledOffset, red);
      Serial.println(" (red)");
    }

    if (pos == -1)
    {                                   // Last hold has been processed!
      strip.Show();                     // Light up all hold (and additional) LEDs
      problemstring = "";               // Reset problem string
      useAdditionalLed = false;         // Reset additional LED option
      state = STATE_WAIT_CONFIGURATION; // Switch to state 0 (wait for new problem string or configuration)
      Serial.println("---------\n");
      break;
    }

    problemstring = problemstring.substring(pos + 1, problemstring.length()); // Remove processed hold from string
  }
}


/**
 * @brief Setup the project
 *
 */
void setup()
{
  delay(1000);
  Serial.begin(9600);

  setupVariables();
  setupBle();
  setupBoardLeds();

  Serial.println("");
  Serial.print("Setup done for the board: ");
  Serial.println(namesByBoard[board]);
  Serial.println("Waiting for bluetooth connection from mobile app...");
}


void loop()
{
  if (!bleSerial)
  {
    delay(100);
    return;
  }

  // Read messages from the BLE UART
  bleSerial.poll();

  // Check if message parts are available on the BLE UART
  while (bleSerial.available() > 0)
  {
    char c = bleSerial.read();

    // State 0: wait for configuration instructions (sent only if V2 option is enabled in app) or beginning of problem string
    if (state == STATE_WAIT_CONFIGURATION)
    {
      if (c == '~')
      {
        state = STATE_READ_CONFIGURATION; // Switch to state 1 (read configuration)
        continue;
      }
      if (c == 'l')
      {
        state = STATE_WAIT_PROBLEM; // Switch to state 2 (read problem string)
        continue;
      }
    }

    // State 1: read configuration option (sent only if V2 option is enabled in app)
    if (state == STATE_READ_CONFIGURATION)
    {
      if (c == 'D')
      {
        useAdditionalLed = true;
        state = STATE_WAIT_PROBLEM; // Switch to state 2 (read problem string)
        continue;
      }
      if (c == 'l')
      {
        state = STATE_WAIT_PROBLEM; // Switch to state 2 (read problem string)
        continue;
      }
    }

    // State 2: wait for the second part of the beginning of a new problem string (# after the lower-case L)
    if (state == STATE_WAIT_PROBLEM)
    {
      if (c == '#')
      {                             // problem string starts with #
        state = STATE_READ_PROBLEM; // Switch to state 3
        continue;
      }
    }

    // State 3: store hold descriptions in problem string
    if (state == STATE_READ_PROBLEM)
    {
      if (c == '#')
      {                               // problem string ends with #
        state = STATE_RENDER_PROBLEM; // Switch to state 4 (start parsing and show LEDs)
        continue;
      }
      problemstring.concat(c); // add current character to problem string
    }
  }

  // State 4: complete problem string received, start parsing
  if (state == STATE_RENDER_PROBLEM)
  {
    Serial.println("\n---------");
    Serial.print("Problem string: ");
    Serial.println(problemstring);
    Serial.println("");

    showProblem();
  }
  delay(100);
}
