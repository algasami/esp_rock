#ifndef UTILS_HPP
#define UTILS_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <M5StickCPlus.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <stddef.h>

struct wifi_router {
  char ssid[20];
  char password[30];
};

enum Modes { WebMode, BatteryMode, IMUMode, EnumLength };

constexpr uint16_t BACKGROUND = TFT_BLACK;
constexpr uint32_t WIDTH = 135;
constexpr uint32_t HEIGHT = 240;
constexpr size_t ROUTERS = 2;

extern Modes current_mode;
extern bool startup;
extern float bat_perc;
extern bool buzz_req;
extern uint32_t pitch;

namespace Web {
extern uint32_t total_visits;
extern bool wifi_connected;
extern bool server_created;
extern char currentSSID[20];
extern char currentIP[20];
extern char hostname[20];
extern WiFiMulti wifi_multi;
extern WebServer server;
extern wifi_router routers[ROUTERS];
} // namespace Web

namespace IMUData {
extern float gyro_x, gyro_y, gyro_z;
extern float acc_x, acc_y, acc_z;
extern float pitch, roll, yaw;
extern float temp;
} // namespace IMUData

void display();
void handle_wifi();
void create_server();
void handle_imu();

// inline functions (macros)
inline void clear() { M5.Lcd.fillScreen(BACKGROUND); }
inline void send_buzzer(uint32_t _pitch) {
  pitch = _pitch;
  buzz_req = true;
}
#endif