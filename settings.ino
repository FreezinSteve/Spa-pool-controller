
bool parseSettings(char *data)
{
  StaticJsonDocument<128> doc;      // ArduinoJSON suggested RAM for given input
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, data);

  // Test if parsing succeeds.
  if (error) {
    DBG("deserializeJson() failed: ");
    DBG(error.f_str());
    return false;
  }
  if (doc.containsKey("ssid"))
  {
    strcpy(userSSID, doc["ssid"]);
    DBG(userSSID);
  }
  if (doc.containsKey("password"))
  {
    strcpy(userPass, doc["password"]);
    DBG(userPass);
  }
  if (doc.containsKey("ipaddr"))
  {
    strcpy(userIP, doc["ipaddr"]);
    DBG(userIP);
  }
  if (doc.containsKey("gatewayaddr"))
  {
    strcpy(userGateway, doc["gatewayaddr"]);
    DBG(userGateway);
  }
  if (doc.containsKey("proxyaddr"))
  {
    strcpy(userProxy, doc["proxyaddr"]);
    DBG(userProxy);
  }
  if (doc.containsKey("utcoffset"))
  {
    userUTCOffset = doc["utcoffset"];
    DBG(userUTCOffset);
  }
  return true;
}

// Save as JSON
bool saveSettings()
{
  DBG("Saving settings");
  File file = LittleFS.open("/settings.txt", "w");
  if (!file) {
    DBG("Failed to save settings file");
    return false;
  }
  // {"ssid":"Casa de Lima","password":"turtle00","ipaddr":"192.168.1.100","gatewayaddr":"192.168.1.250","proxyaddr":"0.0.0.0","utcoffset":"720"}
  file.print("{\"ssid\":\"");
  file.print(userSSID);
  file.print("\",\"password\":\"");
  file.print(userPass);
  file.print("\",\"ipaddr\":\"");
  file.print(userIP);
  file.print("\",\"gatewayaddr\":\"");
  file.print(userGateway);
  file.print("\",\"proxyaddr\":\"");
  file.print(userProxy);
  file.print("\",\"utcoffset\":\"");
  file.print(userUTCOffset);
  file.print("\"}");
  file.close();

  DBG("Saving settings for API access");
  file = LittleFS.open("/api-settings.txt", "w");
  // Save to LittleFS without password for sending via API
  if (!file) {
    DBG("Failed to save settings file");
    return false;
  }
  file.print("{\"ssid\":\"");
  file.print(userSSID);
  file.print("\",\"ipaddr\":\"");
  file.print(userIP);
  file.print("\",\"gatewayaddr\":\"");
  file.print(userGateway);
  file.print("\",\"proxyaddr\":\"");
  file.print(userProxy);
  file.print("\",\"utcoffset\":\"");
  file.print(userUTCOffset);
  file.print("\"}");
  file.close();

  DBG("Save settings complete");
  return true;
}

// Load settings from file
bool loadSettings()
{
  DBG("Loading settings");
  if (LittleFS.exists("/settings.txt"))
  {
    DBG("Settings exists");
    File file = LittleFS.open("/settings.txt", "r");
    if (!file) {
      return false;
    }
    DBG("Settings open");
    if (file.available())
    {
      int len = file.size();
      char buffer[len + 1];
      file.readBytes(buffer, len);
      buffer[len + 1] = 0;
      DBG("Loaded settings: length=");
      DBG(len);
      parseSettings(buffer);
    }
  }
  return true;
}
