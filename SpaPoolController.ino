/*********
  Spa pool controller with web interface
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESP32Time.h>
#define FORMAT_LITTLEFS_IF_FAILED true

#define DEBUG_ON 1
#define DEBUG_OFF 0

byte debugMode = DEBUG_ON;
//#define DBG(...) debugMode == DEBUG_ON ? Serial.println(__VA_ARGS__) : NULL
#define DBG(...) Serial.println(__VA_ARGS__)

/*

   GPIO0 = OUTPUT = JETS PUMP
   GPIO2 = OUTPUT = STATUS LED
   GPIO15 = OUTPUT = HEATER

   GPIOa = INPUT = PUMP ON/OFF (manual pump on/off toggle)
   GPIOb = INPUT = HEATER ON/OFF (manual heater on/off toggle)
   GPIOc = INPUT = Temperature sensor
   GPIOd = INPUT = Water sensor

   Strapping pins, be careful using these as inputs as they affect boot behavior:
   GPIO 0
   GPIO 2
   GPIO 4
   GPIO 5 (must be HIGH during boot)
   GPIO 12 (must be LOW during boot)
   GPIO 15 (must be HIGH during boot)

   The following go HI during boot. Be careful how they're used:
   GPIO 1
   GPIO 3
   GPIO 5
   GPIO 6 to GPIO 11 (connected to the ESP32 integrated SPI flash memory – not recommended to use).
   GPIO 14
   GPIO 15

   Don't use, used for FLASH:
   GPIO 6 (SCK/CLK)
   GPIO 7 (SDO/SD0)
   GPIO 8 (SDI/SD1)
   GPIO 9 (SHD/SD2)
   GPIO 10 (SWP/SD3)
   GPIO 11 (CSC/CMD)

   Default I2C in Arduino IDE:
   GPIO 21 (SDA)
   GPIO 22 (SCL)

   Default SPI (VSPI):
   GPIO 23 = MOSI
   GPIO 19 = MISO
   GPIO 18 = CLK
   GPIO 5 = CS

   Default SPI (HSPI):
   GPIO 13 = MOSI
   GPIO 12 = MISO
   GPIO 14 = CLK
   GPIO 15 = CS

   Digital output channels:
   DAC1 (GPIO25)
   DAC2 (GPIO26)

   Note ADC2 can't be used if WiFi is active

*/

#define JETPUMP_GPIO 0
#define STATUS_GPIO 2
#define HEATER_GPIO 15
#define TEMPSENSOR_GPIO 36
#define WATERSENSOR_GPIO 39

/*
   Globals shared between mulitple INO files. Note that the order Ardiuno IDE compiles them is alphabetical, so we
   can't define these in the relevant INO file if that variable is called from another module that starts with a
   lower letter
*/

float temperature = 0.0;
float setpoint = 38.0;
byte controllerState = 0;
byte heaterState = 0;
byte jetPumpState = 0;
byte circulationPumpState = 0;
byte waterSensor = 0;

bool rtcManuallySet = false;
bool apMode = false;
char userSSID[20] = "";
char userPass[20] = "";
char userIP[20] = "0.0.0.0";        //xxx.xxx.xxx.xxx\0
char userGateway[20] = "0.0.0.0";   //xxx.xxx.xxx.xxx\0
char userProxy[20] = "0.0.0.0";     //xxx.xxx.xxx.xxx\0
int userUTCOffset = 43200;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  if (!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
    DBG("LittleFS Mount Failed");
    return;
  }
  loadSettings();
  initIO();
  startWiFi();
  if (apMode == false)
  {
    startNTP();
  }
  initWebServer();
}

void loop() {
  flashStatus();
  controlLoop();
  setOutputs();
  delay(900);
}

void flashStatus()
{
  if (apMode == 0)
  {
    digitalWrite(STATUS_GPIO, HIGH);
    delay(100);
    digitalWrite(STATUS_GPIO, LOW);
  }
  else
  {
    digitalWrite(STATUS_GPIO, HIGH);
    delay(33);
    digitalWrite(STATUS_GPIO, LOW);
    delay(33);
    digitalWrite(STATUS_GPIO, HIGH);
    delay(33);
    digitalWrite(STATUS_GPIO, LOW);
  }
}
