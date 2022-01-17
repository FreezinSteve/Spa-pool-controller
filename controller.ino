
const float hysteresis = 0.3;

void initIO()
{
  pinMode(STATUS_GPIO, OUTPUT);
  digitalWrite(STATUS_GPIO, HIGH);  // Default ON    
  pinMode(JETPUMP_GPIO, OUTPUT);
  digitalWrite(JETPUMP_GPIO, LOW);
  pinMode(HEATER_GPIO, OUTPUT);
  digitalWrite(HEATER_GPIO, LOW);
  pinMode(TEMPSENSOR_GPIO, INPUT);
  pinMode(WATERSENSOR_GPIO, INPUT);
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
}

void setOutputs()
{
  if (heaterState == 0)
  {
    digitalWrite(HEATER_GPIO, LOW);
  }
  else
  {
    digitalWrite(HEATER_GPIO, HIGH);
  }
  if (jetPumpState == 0)
  {
    digitalWrite(JETPUMP_GPIO, HIGH);
  }
  else
  {
    digitalWrite(JETPUMP_GPIO, LOW);
  }
}

void readTemperature()
{
  // default 0-3300mv = 0-4095
  temperature = analogRead(36);
}

bool readWaterSensor()
{
  //TODO: Read water sensor
  waterSensor = 1;
  return true;
}