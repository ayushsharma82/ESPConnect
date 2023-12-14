// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright (C) 2023 Mathieu Carbou and others
 */
#include "MycilaESPConnect.h"

#include <espconnect_webpage.h>

#include <AsyncJson.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <functional>

static const char* NetworkStateNames[] = {
  "NETWORK_DISABLED",
  "NETWORK_ENABLED",
  "STA_CONNECTING",
  "STA_TIMEOUT",
  "STA_CONNECTED",
  "STA_DISCONNECTED",
  "STA_RECONNECTING",
  "AP_CONNECTING",
  "AP_CONNECTED",
  "PORTAL_STARTING",
  "PORTAL_STARTED",
  "PORTAL_COMPLETE",
  "PORTAL_TIMEOUT",
};

#ifdef ESPCONNECT_DEBUG
static const char* WiFiEventNames[] = {
  "ARDUINO_EVENT_WIFI_READY",
  "ARDUINO_EVENT_WIFI_SCAN_DONE",
  "ARDUINO_EVENT_WIFI_STA_START",
  "ARDUINO_EVENT_WIFI_STA_STOP",
  "ARDUINO_EVENT_WIFI_STA_CONNECTED",
  "ARDUINO_EVENT_WIFI_STA_DISCONNECTED",
  "ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE",
  "ARDUINO_EVENT_WIFI_STA_GOT_IP",
  "ARDUINO_EVENT_WIFI_STA_GOT_IP6",
  "ARDUINO_EVENT_WIFI_STA_LOST_IP",
  "ARDUINO_EVENT_WIFI_AP_START",
  "ARDUINO_EVENT_WIFI_AP_STOP",
  "ARDUINO_EVENT_WIFI_AP_STACONNECTED",
  "ARDUINO_EVENT_WIFI_AP_STADISCONNECTED",
  "ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED",
  "ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED",
  "ARDUINO_EVENT_WIFI_AP_GOT_IP6",
  "ARDUINO_EVENT_WIFI_FTM_REPORT",
  "ARDUINO_EVENT_ETH_START",
  "ARDUINO_EVENT_ETH_STOP",
  "ARDUINO_EVENT_ETH_CONNECTED",
  "ARDUINO_EVENT_ETH_DISCONNECTED",
  "ARDUINO_EVENT_ETH_GOT_IP",
  "ARDUINO_EVENT_ETH_GOT_IP6",
  "ARDUINO_EVENT_WPS_ER_SUCCESS",
  "ARDUINO_EVENT_WPS_ER_FAILED",
  "ARDUINO_EVENT_WPS_ER_TIMEOUT",
  "ARDUINO_EVENT_WPS_ER_PIN",
  "ARDUINO_EVENT_WPS_ER_PBC_OVERLAP",
  "ARDUINO_EVENT_SC_SCAN_DONE",
  "ARDUINO_EVENT_SC_FOUND_CHANNEL",
  "ARDUINO_EVENT_SC_GOT_SSID_PSWD",
  "ARDUINO_EVENT_SC_SEND_ACK_DONE",
  "ARDUINO_EVENT_PROV_INIT",
  "ARDUINO_EVENT_PROV_DEINIT",
  "ARDUINO_EVENT_PROV_START",
  "ARDUINO_EVENT_PROV_END",
  "ARDUINO_EVENT_PROV_CRED_RECV",
  "ARDUINO_EVENT_PROV_CRED_FAIL",
  "ARDUINO_EVENT_PROV_CRED_SUCCESS",
  "ARDUINO_EVENT_MAX"};
#endif

const char* ESPConnectClass::getStateName() const {
  return NetworkStateNames[static_cast<int>(_state)];
}

const char* ESPConnectClass::getStateName(ESPConnectState state) const {
  return NetworkStateNames[static_cast<int>(state)];
}

IPAddress ESPConnectClass::getIPAddress() const {
  return WiFi.getMode() == WIFI_MODE_STA ? WiFi.localIP() : (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA ? WiFi.softAPIP() : IPAddress());
}

String ESPConnectClass::getWiFiSSID() const {
  return WiFi.getMode() == WIFI_MODE_STA ? WiFi.SSID() : (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA ? WiFi.softAPSSID() : emptyString);
}

String ESPConnectClass::getWiFiBSSID() const {
  return WiFi.getMode() == WIFI_MODE_STA ? WiFi.BSSIDstr() : (WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA ? WiFi.softAPmacAddress() : emptyString);
}

int8_t ESPConnectClass::getWiFiRSSI() const {
  return _state == ESPConnectState::STA_CONNECTED ? WiFi.RSSI() : -1;
}

int8_t ESPConnectClass::getWiFiSignalQuality() const {
  return _state == ESPConnectState::STA_CONNECTED ? _wifiSignalQuality(WiFi.RSSI()) : -1;
}

int8_t ESPConnectClass::_wifiSignalQuality(int32_t rssi) const {
  int32_t s = map(rssi, -90, -30, 0, 100);
  return s > 100 ? 100 : (s < 0 ? 0 : s);
}

void ESPConnectClass::begin(AsyncWebServer* httpd, const String& hostname, const String& apSSID, const String& apPassword) {
  if (_state != ESPConnectState::NETWORK_DISABLED)
    return;

  _autoSave = true;

  Preferences preferences;
  preferences.begin("espconnect", true);
  String ssid = preferences.isKey("ssid") ? preferences.getString("ssid", emptyString) : emptyString;
  String password = preferences.isKey("password") ? preferences.getString("password", emptyString) : emptyString;
  bool ap = preferences.isKey("ap") ? preferences.getBool("ap", false) : false;
  preferences.end();

  begin(httpd, hostname, apSSID, apPassword, {ssid, password, ap});
}

void ESPConnectClass::begin(AsyncWebServer* httpd, const String& hostname, const String& apSSID, const String& apPassword, const ESPConnectConfig& config) {
  if (_state != ESPConnectState::NETWORK_DISABLED)
    return;

  _httpd = httpd;
  _hostname = hostname;
  _apSSID = apSSID;
  _apPassword = apPassword;
  _config = config;

  _wifiEventListenerId = WiFi.onEvent(std::bind(&ESPConnectClass::_onWiFiEvent, this, std::placeholders::_1));

  httpd->on("/espconnect/scan", HTTP_GET, [&](AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = new AsyncJsonResponse(true);
    JsonArray json = response->getRoot();
    int n = WiFi.scanComplete();
    if (n == WIFI_SCAN_FAILED)
    {
      WiFi.scanNetworks(true);
      return request->send(202);
    }
    else if (n == WIFI_SCAN_RUNNING)
      return request->send(202);
    else
    {
      for (int i = 0; i < n; ++i)
      {
#if ARDUINOJSON_VERSION_MAJOR == 6
        JsonObject entry = json.createNestedObject();
#else
        JsonObject entry = json.add<JsonObject>();
#endif
        entry["name"] = WiFi.SSID(i);
        entry["rssi"] = WiFi.RSSI(i);
        entry["signal"] = _wifiSignalQuality(WiFi.RSSI(i));
        entry["open"] = WiFi.encryptionType(i) == WIFI_AUTH_OPEN;
      }
      WiFi.scanDelete();
      if (WiFi.scanComplete() == WIFI_SCAN_FAILED)
        WiFi.scanNetworks(true);
    }
    response->setLength();
    request->send(response); });

  httpd->on("/espconnect/connect", HTTP_POST, [&](AsyncWebServerRequest* request) {
    _config.apMode = (request->hasParam("ap_mode", true) ? request->getParam("ap_mode", true)->value() : emptyString) == "true";
    if (_config.apMode) {
      _setState(ESPConnectState::PORTAL_COMPLETE);
      request->send(200, "application/json", "{\"message\":\"Configuration Saved.\"}");
    } else {
      String ssid = request->hasParam("ssid", true) ? request->getParam("ssid", true)->value() : emptyString;
      String password = request->hasParam("password", true) ? request->getParam("password", true)->value() : emptyString;
      if (ssid.isEmpty())
        return request->send(403, "application/json", "{\"message\":\"Invalid SSID\"}");
      if (ssid.length() > 32 || password.length() > 64 || (!password.isEmpty() && password.length() < 8))
        return request->send(403, "application/json", "{\"message\":\"Credentials exceed character limit of 32 & 64 respectively, or password lower than 8 characters.\"}");
      _config.wifiSSID = ssid;
      _config.wifiPassword = password;
      _setState(ESPConnectState::PORTAL_COMPLETE);
      request->send(200, "application/json", "{\"message\":\"Configuration Saved.\"}");
    } });

  httpd->on("/espconnect", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", ESPCONNECT_HTML, sizeof(ESPCONNECT_HTML));
    response->addHeader("Content-Encoding", "gzip");
    request->send(response); });

  // this filter makes sure that the root path is only rewritten when captive portal is started
  httpd->rewrite("/", "/espconnect").setFilter([&](AsyncWebServerRequest* request) {
    return _state == ESPConnectState::PORTAL_STARTED;
  });

  _state = ESPConnectState::NETWORK_ENABLED;

  // blocks like the old behaviour
  if (_blocking) {
    while (_state != ESPConnectState::AP_CONNECTED && _state != ESPConnectState::STA_CONNECTED) {
      loop();
      delay(100);
    }
  }
}

void ESPConnectClass::end() {
  if (_state == ESPConnectState::NETWORK_DISABLED)
    return;
  _lastTime = -1;
  _autoSave = false;
  _setState(ESPConnectState::NETWORK_DISABLED);
  WiFi.removeEvent(_wifiEventListenerId);
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_MODE_NULL);
  _stopAccessPoint();
  _httpd = nullptr;
}

void ESPConnectClass::loop() {
  if (_dnsServer != nullptr)
    _dnsServer->processNextRequest();

  // first check if we have to enter AP mode
  if (_state == ESPConnectState::NETWORK_ENABLED && _config.apMode)
    _startAccessPoint(false);

  // otherwise, tries to connect to WiFi
  if (_state == ESPConnectState::NETWORK_ENABLED && !_config.wifiSSID.isEmpty())
    _startSTA();

  // connection to WiFi times out ?
  if (_state == ESPConnectState::STA_CONNECTING && _durationPassed(_wifiConnectTimeout)) {
    WiFi.disconnect(true, true);
    _setState(ESPConnectState::STA_TIMEOUT);
  }

  // check if we have to enter captive portal mode
  if (_state == ESPConnectState::STA_TIMEOUT || (_state == ESPConnectState::NETWORK_ENABLED && _config.wifiSSID.isEmpty()))
    _startAccessPoint(true);

  // portal duration ends ?
  if (_state == ESPConnectState::PORTAL_STARTED && _durationPassed(_portalTimeout)) {
    _setState(ESPConnectState::PORTAL_TIMEOUT);
  }
}

void ESPConnectClass::clearConfiguration() {
  Preferences preferences;
  preferences.begin("espconnect", false);
  preferences.clear();
  preferences.end();
}

void ESPConnectClass::toJson(const JsonObject& root) const {
  root["ip_address"] = getIPAddress().toString();
  root["mac_address"] = getMacAddress();
  root["state"] = getStateName();
  root["wifi_bssid"] = getWiFiBSSID();
  root["wifi_rssi"] = getWiFiRSSI();
  root["wifi_signal"] = getWiFiSignalQuality();
  root["wifi_ssid"] = getWiFiSSID();
}

void ESPConnectClass::_setState(ESPConnectState state) {
  ESPConnectState previous = _state;
  _state = state;

  if (_autoSave && _state == ESPConnectState::PORTAL_COMPLETE) {
    Preferences preferences;
    preferences.begin("espconnect", false);
    preferences.putBool("ap", _config.apMode);
    if (!_config.apMode) {
      preferences.putString("ssid", _config.wifiSSID);
      preferences.putString("password", _config.wifiPassword);
    }
    preferences.end();
  }

  // make sure callback is called before auto restart
  if (_callback != nullptr)
    _callback(previous, _state);

  if (_autoRestart && (_state == ESPConnectState::PORTAL_COMPLETE || _state == ESPConnectState::PORTAL_TIMEOUT)) {
    ESP.restart();
  }
}

void ESPConnectClass::_startMDNS() {
  MDNS.begin(_hostname.c_str());
}

void ESPConnectClass::_startSTA() {
  _setState(ESPConnectState::STA_CONNECTING);

  WiFi.setHostname(_hostname.c_str());
  WiFi.setSleep(false);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(_config.wifiSSID, _config.wifiPassword);
  _lastTime = esp_timer_get_time();
}

void ESPConnectClass::_startAccessPoint(bool captivePortal) {
  _setState(captivePortal ? ESPConnectState::PORTAL_STARTING : ESPConnectState::AP_CONNECTING);

  WiFi.setHostname(_hostname.c_str());
  WiFi.setSleep(false);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(false);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.mode(captivePortal ? WIFI_AP_STA : WIFI_AP);

  if (_apPassword.isEmpty() || _apPassword.length() < 8) {
    // Disabling invalid Access Point password which must be at least 8 characters long when set
    WiFi.softAP(_apSSID, emptyString);
  } else
    WiFi.softAP(_apSSID, _apPassword);

  _startMDNS();

  if (_dnsServer == nullptr) {
    const IPAddress ip = getIPAddress();
    _dnsServer = new DNSServer();
    _dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    _dnsServer->start(53, "*", ip);
  }

  if (captivePortal) {
    WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
    WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
    WiFi.scanNetworks(true);

    _httpd->begin();

    _httpd->onNotFound([](AsyncWebServerRequest* request) {
      AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", ESPCONNECT_HTML, sizeof(ESPCONNECT_HTML));
      response->addHeader("Content-Encoding", "gzip");
      request->send(response);
    });

    MDNS.addService("http", "tcp", 80);

    _lastTime = esp_timer_get_time();
  }
}

void ESPConnectClass::_stopAccessPoint() {
  _lastTime = -1;
  WiFi.softAPdisconnect(true);
  _httpd->end();
  mdns_service_remove("_http", "_tcp");
  if (_dnsServer != nullptr) {
    _dnsServer->stop();
    delete _dnsServer;
    _dnsServer = nullptr;
  }
}

void ESPConnectClass::_onWiFiEvent(WiFiEvent_t event) {
#ifdef ESPCONNECT_DEBUG
  Serial.printf("[%s] WiFiEvent: %s\n", getStateName(), WiFiEventNames[static_cast<int>(event)]);
#endif

  if (_state == ESPConnectState::NETWORK_DISABLED)
    return;

  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      if (_state == ESPConnectState::STA_CONNECTING || _state == ESPConnectState::STA_RECONNECTING) {
        _lastTime = -1;
        _startMDNS();
        _setState(ESPConnectState::STA_CONNECTED);
      }
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    case ARDUINO_EVENT_WIFI_STA_LOST_IP:
      if (_state == ESPConnectState::STA_CONNECTED) {
        _setState(ESPConnectState::STA_DISCONNECTED);
      }
      if (_state == ESPConnectState::STA_DISCONNECTED && WiFi.getMode() == WIFI_MODE_STA) {
        _setState(ESPConnectState::STA_RECONNECTING);
      }
      break;

    case ARDUINO_EVENT_WIFI_AP_START:
      if (_state == ESPConnectState::AP_CONNECTING) {
        _setState(ESPConnectState::AP_CONNECTED);
      } else if (_state == ESPConnectState::PORTAL_STARTING) {
        _setState(ESPConnectState::PORTAL_STARTED);
      }
      break;

    default:
      break;
  }
}

bool ESPConnectClass::_durationPassed(const uint32_t intervalSec) {
  if (_lastTime >= 0) {
    const int64_t now = esp_timer_get_time();
    if (now - _lastTime >= (int64_t)intervalSec * (int64_t)1000000) {
      _lastTime = now;
      return true;
    }
  }
  return false;
}

ESPConnectClass ESPConnect;
