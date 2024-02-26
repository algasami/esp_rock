#include "utils.hpp"
#include <HTTPClient.h>

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
  if (strcmp(Web::currentSSID, WiFi.SSID().c_str())) {
    strcpy(Web::currentSSID, WiFi.SSID().c_str());
    strcpy(Web::currentIP, WiFi.localIP().toString().c_str());
    strcpy(Web::hostname, WiFi.getHostname());
  }
  if (!Web::server_created) {
    create_server();
  }
  Web::wifi_connected = true;
}

void create_server() {
  {
    char sbuffer[200];
    sprintf(sbuffer,
            "System Initialized! Connect to access the system.\n```\nLocal "
            "IP:%s\nSSID:%s\n```",
            Web::currentIP, Web::currentSSID);
    send_hook("ESP ROCK - SYSTEM", sbuffer);
  }
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

  Web::server.on("/msg", HTTP_GET, []() {
    strcpy(Web::message, Web::server.arg("msg").c_str());
    Web::server.send(200, "text/plain", "Successfully sent");
    ++Web::total_visits;
    send_hook("ESP ROCK - MESSAGE", Web::message);
  });

  Web::server.begin();
  Web::server_created = true;
}

HTTPClient http;
char discord_buffer[200];
void send_hook(const char name[], const char content[]) {
  JsonDocument discord_hook;
  discord_hook["username"] = name;
  discord_hook["content"] = content;
  serializeJsonPretty(discord_hook, discord_buffer);
  http.begin(Web::discord_hook_url);
  http.addHeader("Content-Type", "application/json");
  http.POST(discord_buffer);
  http.end();
}