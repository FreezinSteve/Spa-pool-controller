
const float hysteresis = 0.3;

void initIO()
{
  pinMode(STATUS_GPIO, OUTPUT);
  digitalWrite(STATUS_GPIO, HIGH);  // Default ON
  //pinMode(JETPUMP_GPIO, OUTPUT);
  //digitalWrite(JETPUMP_GPIO, LOW);
  //pinMode(HEATER_GPIO, OUTPUT);
  //digitalWrite(HEATER_GPIO, LOW);
  pinMode(TEMPSENSOR_GPIO, INPUT);
  pinMode(WATERSENSOR_GPIO, INPUT);

  //2600 mV. (1V input = ADC reading of 1575).
  analogSetPinAttenuation(TEMPSENSOR_GPIO, ADC_11db);

}

void controlLoop()
{
  Serial.println("Loop");
  if (!readWaterSensor())
  {
    heaterState = 0;
    jetPumpState = 0;
    circulationPumpState = 0;
    return;
  }
  readTemperature();
  if (controllerState == 0) {
    checkManualTimers();
    return;
  }
  if (heaterState == 0)
  {
    // Heater curently off
    if ((setpoint - hysteresis) > temperature)
    {
      heaterState = 1;
    }
  }
  else
  {
    // Heater currently on
    if (setpoint < temperature)
    {
      heaterState = 0;
    }
  }
  // In automatic. Disable heater timer
  heaterStopTime = 0;
}

void checkManualTimers()
{
  // Check the heater and jets timer
  if (heaterStopTime > 0)
  {
    if (heaterStopTime < esp_timer_get_time())
    {
      heaterState == 0;
      heaterStopTime = 0;
    }
  }
  if (jetsStopTime > 0)
  {
    if (jetsStopTime < esp_timer_get_time())
    {
      jetPumpState = 0;
      jetsStopTime = 0 ;
    }
  }
}

void setHeaterState(int state)
{
  heaterState = state;
  if (state == 1)
  {
    // Set the time to turn the heater back off (will be overridden if in auto anyway)
    heaterStopTime = esp_timer_get_time() + HEATER_MAX_TIME;
  }
}

void setJetPumpState(int state)
{
  jetPumpState = state;
  if (state == 1)
  {
    // Set the time to turn the jet pumps back off
    jetsStopTime = esp_timer_get_time() + JETS_MAX_TIME;
  }
}

void setControllerState(int state)
{
  controllerState = state;
  if (controllerState ==0)
  {
    heaterState = 0;
    jetPumpState = 0;
    jetsStopTime = 0;
    heaterStopTime = 0;
  }
}
void setOutputs()
{
  if (heaterState == 0)
  {
    SetRelayState(HEATER_RELAY, 0);
    DBG("RELAY0 OFF");
    //digitalWrite(HEATER_GPIO, LOW);
  }
  else
  {
    SetRelayState(HEATER_RELAY, 1);
    DBG("RELAY0 ON");
    //digitalWrite(HEATER_GPIO, HIGH);
  }
  if (jetPumpState == 0)
  {
    SetRelayState(JETPUMP_RELAY, 0);
    DBG("RELAY1 OFF");
    //digitalWrite(JETPUMP_GPIO, LOW);
  }
  else
  {
    SetRelayState(JETPUMP_RELAY, 1);
    DBG("RELAY0 ON");
    //digitalWrite(JETPUMP_GPIO, HIGH);
  }
}

void readTemperature()
{
  // default 0-3300mv = 0-4095
  temperature = 0.05557 * analogReadMilliVolts(TEMPSENSOR_GPIO) - 17.8;
}

bool readWaterSensor()
{
  //TODO: Read water sensor
  waterSensor = 1;
  return true;
}
