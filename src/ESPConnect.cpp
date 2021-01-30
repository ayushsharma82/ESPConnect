#include "ESPConnect.h"


/* 
  Loads STA Credentials into memory
*/

void ESPConnectClass::load_sta_credentials(){
  // Get saved WiFi Credentials
  #if defined(ESP8266)
    station_config config;
    wifi_station_get_config(&config);
    for(int i=0; i < strlen((char*)config.ssid); i++){
      _sta_ssid += (char)config.ssid[i];
    }
    for(int i=0; i < strlen((char*)config.password); i++){
      _sta_password += (char)config.password[i];
    }
  #elif defined(ESP32)
    wifi_sta_config_t config;
    esp_wifi_get_config(WIFI_MODE_STA, &config);
    for(int i=0; i < strlen((char*)config.ssid); i++){
      _sta_ssid += (char)config.ssid[i];
    }
    for(int i=0; i < strlen((char*)config.password); i++){
      _sta_password += (char)config.password[i];
    }
  #endif
}


/*
  Start Captive Portal and Attach DNS & Webserver
*/
void ESPConnectClass::start_portal(){
  bool _reboot = false;
  unsigned long _reboot_millis = 0;

  ESPCONNECT_SERIAL("Starting Captive Portal\n");
  // Try Connecting Station
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(_sta_ssid.c_str(), _sta_password.c_str());
  // Start Access Point
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(_auto_connect_ssid.c_str(), _auto_connect_password.c_str());

  // Start our DNS Server
  _dns = new DNSServer();
  _dns->setErrorReplyCode(DNSReplyCode::NoError);
  _dns->start(53, "*", IPAddress(192, 168, 4, 1));

  auto scanGET = _server->on("/espconnect/scan", HTTP_GET, [&](AsyncWebServerRequest *request){
    String json = "[";
    int n = WiFi.scanComplete();
    if(n == -2){
      WiFi.scanNetworks(true);
      return request->send(202);
    }else if(n == -1){
      return request->send(202);
    }else{
      for (int i = 0; i < n; ++i){
        json+="{";
        json+="\"name\":\""+WiFi.SSID(i)+"\",";
        json+="\"open\":"+String(WiFi.encryptionType(i) == ENC_TYPE_NONE ? "true": "false");
        json+="}";
        if(i != n-1) json+=",";
      }
      WiFi.scanDelete();
      if(WiFi.scanComplete() == -2){
        WiFi.scanNetworks(true);
      }
    }
    json += "]";
    request->send(200, "application/json", json);
    json = String();
  });

  // TODO: Accept incomming WiFi Credentials
  auto connectPOST = _server->on("/espconnect/connect", HTTP_POST, [&](AsyncWebServerRequest *request){
    // Get FormData
    String ssid = request->hasParam("ssid", true) ? request->getParam("ssid", true)->value().c_str() : "";
    String password = request->hasParam("password", true) ? request->getParam("password", true)->value().c_str() : "";

    if(ssid.length() <= 0){
      return request->send(403, "application/json", "{\"message\":\"Invalid SSID\"}");
    }

    if(ssid.length() > 32 || password.length() > 64){
      return request->send(403, "application/json", "{\"message\":\"Credentials exceed character limit of 32 & 64 respectively.\"}");
    }
    
    // Save WiFi Credentials in Flash
    bool ok = false;
    #if defined(ESP8266)
      struct station_config	config;
      ssid.toCharArray((char*)config.ssid, sizeof(config.ssid));
      password.toCharArray((char*)config.password, sizeof(config.password));
      config.bssid_set = 1;
      ok = wifi_station_set_config(&config);
    #elif defined(ESP32)
    #endif

    if(ok){
      request->send(200, "application/json", "{\"message\":\"Credentials Saved. Rebooting...\"}");
    }else{
      request->send(500, "application/json", "{\"message\":\"Error while saving WiFi Credentials\"}");
    }

    _reboot = true;
    _reboot_millis = millis();
  });
  
  auto indexGET = _server->on("/espconnect", HTTP_GET, [&](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ESPCONNECT_HTML, ESPCONNECT_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  _server->onNotFound([&](AsyncWebServerRequest *request){
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ESPCONNECT_HTML, ESPCONNECT_HTML_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  auto portalRewrite = _server->rewrite("/", "/espconnect").setFilter(ON_AP_FILTER);

  // Begin Webserver    
  _server->begin();

  unsigned long lastMillis = millis();
  while(WiFi.status() != WL_CONNECTED && (unsigned long)(millis() - lastMillis) < _auto_connect_timeout){
    _dns->processNextRequest();
    yield();

    if(_reboot && (unsigned long)(millis() - _reboot_millis) > 5000){
      yield();
      ESP.restart();
    }
  }

  if(WiFi.status() != WL_CONNECTED){
    ESPCONNECT_SERIAL("Portal timed out\n");
  }else{
    ESPCONNECT_SERIAL("Connected to STA\n");
  }

  _server->removeHandler(&indexGET);
  _server->removeHandler(&scanGET);
  _server->removeHandler(&connectPOST);
  _server->removeRewrite(&portalRewrite);
  _server->onNotFound([](AsyncWebServerRequest *request){
    request->send(404);
  });
  
  ESPCONNECT_SERIAL("Closed Portal\n");
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);

  _dns->stop();

  delete _dns;
  _dns = nullptr;
}


/*
  Set Custom AP Credentials
*/
void ESPConnectClass::autoConnect(const char* ssid, const char* password, unsigned long timeout){
  _auto_connect_ssid = ssid;
  _auto_connect_password = password;
  _auto_connect_timeout = timeout;
}


/*
  Connect to saved WiFi Credentials
*/
bool ESPConnectClass::begin(AsyncWebServer* server, unsigned long timeout){
  _server = server;

  load_sta_credentials();

  if(_sta_ssid != ""){
    ESPCONNECT_SERIAL("STA Pre-configured:\n");
    ESPCONNECT_SERIAL("SSID: "+_sta_ssid+"\n");
    ESPCONNECT_SERIAL("Password: "+_sta_password+"\n\n");
    ESPCONNECT_SERIAL("Connecting to STA\n");
    
    // Try connecting to STA
    WiFi.setAutoConnect(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(_sta_ssid.c_str(), _sta_password.c_str());

    // Check WiFi connection status till timeout
    unsigned long lastMillis = millis();
    while(WiFi.status() != WL_CONNECTED && (unsigned long)(millis() - lastMillis) < timeout){
      delay(100);
      yield();
    }

    if(WiFi.status() != WL_CONNECTED){
      ESPCONNECT_SERIAL("Connection to STA Falied [!]");
    }
  }

  // Start Captive Portal if not connected to STA
  if(WiFi.status() != WL_CONNECTED){
    // Start captive portal
    start_portal();
  }else{
    ESPCONNECT_SERIAL("Connected to STA\n");
  }

  return (WiFi.status() == WL_CONNECTED);
}


/*
  Erase Stored WiFi Credentials
*/
bool ESPConnectClass::erase(){
  return ESP.eraseConfig();
}


/*
  Return Connection Status
*/
bool ESPConnectClass::isConnected(){
  return (WiFi.status() == WL_CONNECTED);
}



ESPConnectClass ESPConnect;