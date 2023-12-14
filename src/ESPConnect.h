#ifndef ESPConnect_h
#define ESPConnect_h


#include <functional>
#include <Arduino.h>

#if defined(ESP8266)
  #include "ESP8266WiFi.h"
  #include "WiFiClient.h"
  #include "ESPAsyncTCP.h"
#elif defined(ESP32)
  #include "WiFi.h"
  #include "WiFiClient.h"
  #include "AsyncTCP.h"
  #include "Preferences.h"
#endif

#include "ESPAsyncWebServer.h"
#include "DNSServer.h"
#include "espconnect_webpage.h"

#define DEFAULT_CONNECTION_TIMEOUT 30000
#define DEFAULT_PORTAL_TIMEOUT 180000


#ifdef ESPCONNECT_DEBUG
  #define ESPCONNECT_SERIAL(x) Serial.print("[ESPConnect]["+String(millis())+"] "+x)
#else
  #define ESPCONNECT_SERIAL(x)
#endif


class ESPConnectClass {

  private:
    DNSServer* _dns = nullptr;
    AsyncWebServer* _server = nullptr;

    String _auto_connect_ssid = "";
    String _auto_connect_password = "";
    unsigned long _auto_connect_timeout = DEFAULT_PORTAL_TIMEOUT;

    String _sta_ssid = "";
    String _sta_password = "";
    bool _apMode = false;

  private:
    void load_sta_credentials();

    // Start Captive portal
    bool start_portal();

    int8_t wifiSignalQuality(int32_t rssi);


  public:
    // Check if ESPConnect was configured before
    bool isConfigured();

    // Set Custom AP
    void autoConnect(const char* ssid, const char* password = "", unsigned long timeout = DEFAULT_PORTAL_TIMEOUT);

    // Connect to Saved WiFi Credentials
    bool begin(AsyncWebServer* server, unsigned long timeout = DEFAULT_CONNECTION_TIMEOUT);

    // Erase Saved WiFi Credentials
    bool erase();

    /*
      Data Getters
    */

    // Return true / false depending of connection status
    bool isConnected();

    // Gets SSID of connected endpoint
    String getSSID();
    String getPassword();
};

extern ESPConnectClass ESPConnect;

#endif