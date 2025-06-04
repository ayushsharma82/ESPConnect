#ifndef ESPConnect_h
#define ESPConnect_h

#warning ESPConnect has been deprecated and will not be maintained further. Consider switching to newer NetWizard library from same author which is much better and comes with configuration portal: https://github.com/ayushsharma82/NetWizard

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

/* Library Default Settings */
#define ESPCONNECT_DEBUG 1

#define DEFAULT_CONNECTION_TIMEOUT 30000
#define DEFAULT_PORTAL_TIMEOUT 180000


#if ESPCONNECT_DEBUG == 1
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

    boolean _configured = false;
    boolean _blocking = true;
    unsigned long _portalStartedAt; 

    AsyncWebHandler* _indexGET;
    AsyncWebHandler* _scanGET;
    AsyncWebHandler* _connectPOST;
    AsyncWebRewrite* _portalRewrite;

  private:
    void load_sta_credentials();

    // Start Captive portal
    bool start_portal();

    // Stop Captive portal
    void stop_portal();

  public:
    // Check if ESPConnect was configured before
    bool isConfigured();

    // Set Custom AP
    void autoConnect(const char* ssid, const char* password = "", unsigned long timeout = DEFAULT_PORTAL_TIMEOUT);

    // Connect to Saved WiFi Credentials
    bool begin(AsyncWebServer* server, unsigned long timeout = DEFAULT_CONNECTION_TIMEOUT);

    // Erase Saved WiFi Credentials
    bool erase();

    // When set to false. process() method must be called inside loop
    void setBlocking(boolean value);

    // Run in loop for non blocking mode
    bool process();

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
