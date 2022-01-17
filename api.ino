// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//------------------------------------------------------------
// Handlers for HTTP requests
//------------------------------------------------------------
void initWebServer()
{
  //======================================================================
  // File handlers with compression. Must be an easier way of handling multiple files...
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html.gz", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  // If client not on a local address then return version with no Settings page (blocked in the API as well)
  server.on("/app.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (isLocalRequest(request, false))
    {
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/app.html.gz", "text/html");
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
    else
    {
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/app-no-settings.html.gz", "text/html");
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
  });

  server.on("/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/bootstrap.bundle.min.js.gz", "text/javascript");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/favicon.ico.gz", "image/x-icon");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/feather.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/feather.min.js.gz", "text/javascript");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/highcharts.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/highcharts.js.gz", "text/javascript");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/highcharts-more.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/highcharts-more.js.gz", "text/javascript");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/index.html.gz", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/jquery.min.js.gz", "text/javascript");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/logdata.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/logdata.html.gz", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/logdatawind.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/logdatawind.html.gz", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/spa.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/spa.png.gz", "text/plain");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (isLocalRequest(request, true))
    {
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/settings.html.gz", "text/html");
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    }
  });

  server.on("/solid-gauge.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/solid-gauge.js.gz", "text/javascript");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/styles.css.gz", "text/css");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/summary.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/summary.html.gz", "text/html");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  //======================================================================
  // Load / Save settings
  // Block anything not on a 192.168 address
  server.on("/update-settings", HTTP_POST, [](AsyncWebServerRequest * request) {}, NULL, handleUpdateSettings);

  server.on("/read-settings", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (isLocalRequest(request, true))
    {
      if (LittleFS.exists("/api-settings.txt"))
      {
        AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/api-settings.txt", "text/plain");
        request->send(response);
      }
      else
      {
        request->send(200);
      }
    }
  });

  server.on("/set-time", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (isLocalRequest(request, true))
    {
      String newTime = request->getParam("time")->value();
      setRTCTime(newTime);
      request->send(200);
    }
  });
  //======================================================================
  // Control API
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    if (!request->hasParam("param"))
    {
      // Send error
      request->send_P(400, "text/plain", "ERROR: invalid query string");
      return;
    }
    String param = request->getParam("param")->value();
    String value = request->getParam("value")->value();
    if (param == "setpoint")
    {
      setpoint = value.toFloat();
      request->send_P(200, "text/plain", "OK");
    }
    else if (param == "heater")
    {
      heaterState = value.toInt();
      request->send_P(200, "text/plain", "OK");
    }
    else if (param == "jets")
    {
      jetPumpState = value.toInt();
      request->send_P(200, "text/plain", "OK");
    }
    else if (param = "auto")
    {
      controllerState = value.toInt();
      request->send_P(200, "text/plain", "OK");
    }
    else
    {
      // Send error
      request->send_P(400, "text/plain", "ERROR: invalid query string");
    }
  });

  //======================================================================
  // Diagnostics API

  server.on("/reboot", HTTP_POST, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", "Rebooting module, wait for module to connect to WiFi before continuing");
    reboot();
  });

  server.on("/heapfree", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getMemInfo().c_str());
  });

  server.on("/files", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", listFiles().c_str());
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", getStatus().c_str());
  });

  server.on("/rssi", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(WiFi.RSSI()).c_str());
  });

  server.on("/resets", HTTP_GET, []
  (AsyncWebServerRequest * request) {
    if (LittleFS.exists("/resets.txt"))
    {
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/resets.txt", "text/plain");
      request->send(response);
    }
    else
    {
      request->send(200);
    }
  });

  server.begin();
}

// Return TRUE if request is from a local address. If false AND handleResponse==TRUE then error 404 is returned
bool isLocalRequest(AsyncWebServerRequest * request, bool handleResponse)
{
  IPAddress source = request->client()->remoteIP();
  //TODO: Check the proxy address in settings
  if (source[0] == 192 && source[1] == 168 && source[2] == 1 && source[3] != 130)
  {
    return true;
  }
  else
  {
    if (handleResponse)
    {
      request->send(404);
    }
    return false;
  }
}

// Request is chunked, store data in temp object until we have the complete response
void handleUpdateSettings(AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
  if (isLocalRequest(request, true))
  {
    if (request->_tempObject == NULL)
    {
      request->_tempObject = new char[total + 1];
    }
    char *tempObject = (char *)request->_tempObject;
    for (int i = 0; i < len; i++)
    {
      tempObject[index + i] = (char)data[i];
    }
    if (index + len >= total)
    {
      tempObject[total] = '\0';
      if (parseSettings(tempObject))
      {
        // Save to LittleFS as JSON
        saveSettings();
        request->send(200);
      }
      else
      {
        request->send(500);
      }
    }
  }
}

//------------------------------------------------------------
// Return file list and used/free bytes
//------------------------------------------------------------
String listFiles()
{
  String str = "";
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  while (file) {
    if (!file.isDirectory())
    {
      str += file.name();
      str += " [";
      str += file.size();
      str += "]\r\n\r\n";
    }
    file = root.openNextFile();
  }
  str += "Bytes Used : " + String(LittleFS.usedBytes()) + "\r\n";
  str += "Bytes Total: " + String(LittleFS.totalBytes()) + "\r\n";
  str += "Bytes Free : " + String(LittleFS.totalBytes() - LittleFS.usedBytes()) + "\r\n";
  return str;
}

String getStatus()
{
  DBG("Start getStatus()");
  String response = "{\"dt\": \"";
  response += getISO8601Time(false);
  response += "\", \"te\": \"";
  response += temperature;
  response += "\", \"sp\": \"";
  response += setpoint;
  response += "\", \"ts\": \"";
  response += "43200";  // getTimeTillAtSetpoint()
  response += "\", \"cs\": \"";
  response += controllerState;
  response += "\", \"js\": \"";
  response += jetPumpState;
  response += "\", \"hs\": \"";
  response += heaterState;
  response += "\", \"ws\": \"";
  response += waterSensor;
  response += "\"}";
  DBG(response);
  return response;
}

String getMemInfo()
{
  String response = "Free Heap: ";
  response += ESP.getFreeHeap();
  return response;
}

void reboot()
{
  delay(500);
  ESP.restart();
}
