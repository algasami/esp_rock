#include "utils.hpp"

Modes current_mode = WebMode;
bool startup = false;
float bat_perc = 0.0;
bool buzz_req = false;
uint32_t pitch = 2500;

void handle_battery();
void audio();

void setup() {
  Serial.begin(9600);
  M5.begin(); // By default, "M5.begin()" will initialize AXP192 chip
  M5.Imu.Init();
  M5.Lcd.fillScreen(BACKGROUND);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.setTextSize(1);

  // initializes wifi
  for (size_t i = 0; i < ROUTERS; i++) {
    Web::wifi_multi.addAP(Web::routers[i].ssid, Web::routers[i].password);
  }
}

// Changes mode (iram attr loads this to ram)
void IRAM_ATTR on_home_press() {
  static uint32_t last = millis();
  uint32_t now = millis();
  if (now - last < 300) {
    return;
  } else {
    last = now;
  }
  current_mode = static_cast<Modes>((current_mode + 1) % Modes::EnumLength);
}

void loop() {
  M5.update();
  if (Web::wifi_connected) {
    Web::server.handleClient();
  }
  handle_battery();
  handle_wifi();
  handle_imu();
  display();
  audio();
  attachInterrupt(M5_BUTTON_HOME, on_home_press, FALLING);
}

void handle_battery() {
  static uint32_t last = millis();
  uint32_t now = millis();
  if (now - last > 2000) {
    last = now;
  } else {
    return;
  }
  float volt = M5.Axp.GetBatVoltage();
  float perc_now = (volt < 3.2) ? 0 : (volt - 3.2);
  bat_perc = perc_now;
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