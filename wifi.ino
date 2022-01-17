
#define AP_SSID "Spa Pool"
#define AP_IP {192, 168, 1, 101}
#define AP_GATEWAY {192, 168, 1, 101}
#define AP_SUBNET {255, 255, 255, 0}

#define STA_SUBNET {255, 255, 255, 0}
#define STA_DNS1 {8, 8, 8, 8}
#define STA_DNS2 {8, 8, 4, 4}

void startWiFi()
{
  if (strlen(userSSID)==0)
  {
    connectAP();
  }
  else 
  {
    if (!connectSTA(15000))
    {
      connectAP();  
    }
  }
}

//================================================================
// Connection methods
//================================================================
bool connectSTA(int timeout) {

  byte sub[] = STA_SUBNET;
  byte addr1[] = STA_DNS1;
  byte addr2[] = STA_DNS2;

  IPAddress subnet(sub);
  IPAddress dns1(addr1);
  IPAddress dns2(addr2);

  DBG("Connecting in STA mode");

  // Clear previous settings
  WiFi.disconnect(true);

  // Connect to Wifi.
  IPAddress ip;

  int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0 ;
  if (sscanf(userIP, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4) == 4) {
    ip = IPAddress(ip1, ip2, ip3, ip4);
  }

  IPAddress gateway;
  if (sscanf(userGateway, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4) == 4) {
    gateway = IPAddress(ip1, ip2, ip3, ip4);
  }

  WiFi.mode(WIFI_STA);
  // Configures static IP address
  WiFi.config(ip, gateway, subnet, dns1, dns2);
  WiFi.begin(userSSID, userPass);
  unsigned long wifiConnectStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
    // Check to see if
    if (WiFi.status() == WL_CONNECT_FAILED) {
      DBG("Failed to connect in STA mode");
      return false;
    }

    delay(500);
    if (timeout > 0)
    {
      // Only try for 15 seconds.
      if (millis() - wifiConnectStart > timeout) {
        DBG("Failed to connect in STA mode");
        return false;
      }
    }
    yield();
  }
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  DBG("Connected in STA mode");
  DBG(WiFi.localIP());
  return true;
}

void connectAP()
{
  byte ip[] = AP_IP;
  byte gw[] = AP_GATEWAY;
  byte sub[] = AP_SUBNET;
  
  IPAddress apIP(ip);
  IPAddress apGateway(gw);
  IPAddress apSubnet(sub);

  DBG("Starting AP mode");
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_AP);  
  WiFi.softAP(AP_SSID);    // just pass SSID for open network
  delay(100);   // Wait for AP service to start 
  WiFi.softAPConfig(apIP, apGateway, apSubnet);  

  apMode = true;
  DBG("AP mode started on address");
  DBG(WiFi.softAPIP());
}
