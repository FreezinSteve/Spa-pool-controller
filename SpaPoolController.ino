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

#define JETPUMP_RELAY 0
#define HEATER_RELAY 1

#define STATUS_GPIO 2 
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

uint64_t heaterStopTime = 0;             // time in microseconds to stop (RTC independant)
uint64_t jetsStopTime = 0;               // time in microseconds to stop (RTC independant)
const uint64_t HEATER_MAX_TIME = 7200ULL * 1000000ULL;    // 2 hours when on manual
const uint64_t JETS_MAX_TIME = 7200ULL * 1000000ULL;       // 2 hours

bool rtcManuallySet = false;
bool apMode = false;
char userSSID[20] = "";
char userPass[20] = "";
char userIP[20] = "0.0.0.0";        //xxx.xxx.xxx.xxx\0
char userGateway[20] = "0.0.0.0";   //xxx.xxx.xxx.xxx\0
char userProxy[20] = "0.0.0.0";     //xxx.xxx.xxx.xxx\0
int userUTCOffset = 43200;


HardwareSerial MODBUS(2);


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  MODBUS.begin(9600, SERIAL_8N1, 16, 17);
  //MODBUS.begin(9600, SERIAL_8N1, 5, 18);
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
