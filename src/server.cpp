#include "utils.hpp"

WiFiMulti Web::wifi_multi;
WebServer Web::server(80);

uint32_t Web::total_visits = 0;
char Web::currentSSID[20] = "\0";
char Web::currentIP[20] = "\0";
char Web::hostname[20] = "\0";
bool Web::wifi_connected = false;
bool Web::server_created = false;

void handle_wifi() {
  static uint32_t last = millis();
  uint32_t now = millis();
  if (now - last > 1000) {
    last = now;
  } else {
    return;
  }
  if (Web::wifi_multi.run() != WL_CONNECTED) {
    Web::wifi_connected = false;
    if (Web::server_created) {
      Web::server_created = false;
      Web::server.close();
    }
    return;
  }
  if (!Web::server_created) {
    create_server();
  }
  if (strcmp(Web::currentSSID, WiFi.SSID().c_str())) {
    strcpy(Web::currentSSID, WiFi.SSID().c_str());
    strcpy(Web::currentIP, WiFi.localIP().toString().c_str());
    strcpy(Web::hostname, WiFi.getHostname());
  }
  Web::wifi_connected = true;
}

void create_server() {
  Web::server.on("/", HTTP_GET, []() {
    char buffer[100];
    JsonDocument res;
    res["hostname"] = Web::hostname;
    res["con_ssid"] = Web::currentSSID;
    res["total_vis"] = Web::total_visits;
    serializeJsonPretty(res, buffer);
    Web::server.send(200, "application/json", buffer);
    ++Web::total_visits;
  });

  Web::server.begin();
  Web::server_created = true;
}