#ifndef UTILS_HPP
#define UTILS_HPP
#include <WiFi.h>
#include <WiFiMulti.h>
#include <stddef.h>

struct wifi_router {
  char ssid[20];
  char password[30];
};

constexpr size_t ROUTERS = 2;
extern wifi_router routers[ROUTERS];
#endif