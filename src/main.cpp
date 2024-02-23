#include "utils.hpp"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <M5StickCPlus.h>
#include <WebServer.h>

constexpr uint16_t BACKGROUND = TFT_BLACK;
constexpr uint32_t WIDTH = 135;
constexpr uint32_t HEIGHT = 240;

WiFiMulti wifi_multi;
WebServer server(80);

void handle_battery();
void handle_wifi();
void display();
void audio();

inline void clear() { M5.Lcd.fillScreen(BACKGROUND); }

void setup() {
  Serial.begin(9600);
  M5.begin(); // By default, "M5.begin()" will initialize AXP192 chip
  M5.Lcd.fillScreen(BACKGROUND);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(1);

  // initializes wifi
  for (size_t i = 0; i < ROUTERS; i++) {
    wifi_multi.addAP(routers[i].ssid, routers[i].password);
  }
}

void loop() {
  M5.update();
  server.handleClient();
  handle_battery();
  handle_wifi();
  display();
  audio();
  delay(100);
}

bool startup = false;
float bat_perc = 0.0;
char currentSSID[20] = "\0";
char currentIP[20] = "\0";
char hostname[20] = "\0";
bool wifi_connected = true;
bool server_created = false;
bool buzz_req = false;
uint32_t pitch = 2500;

inline void send_buzzer(uint32_t _pitch) {
  pitch = _pitch;
  buzz_req = true;
}

void create_server() {
  server.on("/", HTTP_GET, []() {
    char buffer[100];
    JsonDocument res;
    res["hostname"] = hostname;
    res["con_ssid"] = currentSSID;
    serializeJsonPretty(res, buffer);
    server.send(200, "application/json", buffer);
    send_buzzer(523);
  });

  server.begin();
  server_created = true;
}

void handle_battery() {
  float volt = M5.Axp.GetBatVoltage();
  float perc_now = (volt < 3.2) ? 0 : (volt - 3.2);
  bat_perc = perc_now;
}

void handle_wifi() {
  if (wifi_multi.run() != WL_CONNECTED) {
    wifi_connected = false;
    server_created = false;
    server.close();
    return;
  }
  wifi_connected = true;
  if (!server_created) {
    create_server();
  }
  if (strcmp(currentSSID, WiFi.SSID().c_str())) {
    strcpy(currentSSID, WiFi.SSID().c_str());
    strcpy(currentIP, WiFi.localIP().toString().c_str());
    strcpy(hostname, WiFi.getHostname());
  }
}

void display() {

  static float dbat_perc = 0.0;
  static bool dwifi_connected = true;

  bool changed = !startup;
  if (!startup) {
    startup = true;
  }
  if (dbat_perc != bat_perc || dwifi_connected != wifi_connected) {
    dbat_perc = bat_perc;
    dwifi_connected = wifi_connected;
    changed = true;
  }

  if (!changed) {
    return;
  }

  clear();

  // Battery life
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("%.2f%%", dbat_perc * 100.0f);
  M5.Lcd.fillRect(0, 10,
                  static_cast<uint32_t>(static_cast<float>(WIDTH) * dbat_perc),
                  // * We have to static cast to uin32_t since if we
                  // * cast to int32_t, it will return negative.
                  4, TFT_BLUE);

  // Wifi
  M5.Lcd.setCursor(0, 16);
  if (dwifi_connected) {
    M5.Lcd.printf("%s\r\n", currentSSID);
    M5.Lcd.setTextSize(2);
    M5.Lcd.printf("%s", currentIP);
    M5.Lcd.setTextSize(1);
  } else {
    M5.Lcd.printf("Bad Wifi");
  }

  // if (gif.open((uint8_t *)ucHomer, sizeof(ucHomer), GIFDraw)) {
  //  Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n",
  //                gif.getCanvasWidth(), gif.getCanvasHeight());
  //  while (gif.playFrame(true, NULL)) {
  //  }
  // gif.close();
  // }
}

void audio() {
  M5.Beep.update();
  static uint32_t buzzer_start = 0;
  if (buzz_req) {
    buzzer_start = millis();
    M5.Beep.tone(pitch);
    buzz_req = false;
  }
  if (millis() - buzzer_start > 50) {
    M5.Beep.mute();
  }
}