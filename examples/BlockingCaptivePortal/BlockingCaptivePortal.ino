#include <MycilaESPConnect.h>

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  while (!Serial)
    continue;

  // serve your logo here
  server.on("/logo", HTTP_GET, [&](AsyncWebServerRequest* request) {
    // AsyncWebServerResponse* response = request->beginResponse_P(200, "image/png", _binary_data_logo_icon_png_gz_start, _binary_data_logo_icon_png_gz_end - _binary_data_logo_icon_png_gz_start);
    AsyncWebServerResponse* response = request->beginResponse_P(200, "image/png", "", 0);
    response->addHeader("Content-Encoding", "gzip");
    response->addHeader("Cache-Control", "public, max-age=900");
    request->send(response);
  });

  // serve your home page here
  server.on("/home", HTTP_GET, [&](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Hello World!");
  });

  // clear persisted config
  server.on("/clear", HTTP_GET, [&](AsyncWebServerRequest* request) {
    ESPConnect.clearConfiguration();
    request->send(200);
  });

  // add a rewrite which is only applicable in AP mode and STA mode, but not in Captive Portal mode
  server.rewrite("/", "/home").setFilter([](AsyncWebServerRequest* request) { return ESPConnect.getState() != ESPConnectState::PORTAL_STARTED; });

  // network state listener
  ESPConnect.listen([](ESPConnectState previous, ESPConnectState state) {
    Serial.printf("NetworkState: %s => %s\n", ESPConnect.getStateName(previous), ESPConnect.getStateName(state));
    JsonDocument doc;
    ESPConnect.toJson(doc.to<JsonObject>());
    serializeJson(doc, Serial);
    Serial.println();
  });

  ESPConnect.setAutoRestart(true);
  ESPConnect.setBlocking(true);
  ESPConnect.setCaptivePortalTimeout(180);
  ESPConnect.setWiFiConnectTimeout(10);

  Serial.println("Trying to connect to saved WiFi or will start portal...");

  ESPConnect.begin(&server, "arduino", "Captive Portal SSID");

  Serial.println("ESPConnect completed, continuing setup()...");

  server.begin();
}

void loop() {
  ESPConnect.loop();
}
