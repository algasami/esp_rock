#include "utils.hpp"

constexpr uint32_t DMODE_SWITCH_FREQ = 523;
constexpr uint32_t DWEB_CHANGED_FREQ = 800;

volatile Modes current_mode = WebMode;

char Web::message[100] = "\0";

void display() {

    static Modes dmode = EnumLength;
    static float dbat_perc = 0.0;
    static bool dwifi_connected = false;

    // mode-agnostic changes
    static uint32_t ma_web_total_visits = 0;

    // Battery life
    if (dbat_perc != bat_perc || !startup) {
        M5.Lcd.fillRect(0, 0, WIDTH, 14, BACKGROUND);
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.printf("%.2f%%", bat_perc * 100.0f);
        M5.Lcd.fillRect(0, 10, static_cast<uint32_t>(static_cast<float>(WIDTH) * bat_perc), 4,
                        TFT_BLUE);
    }

    // Wifi
    if (dwifi_connected != Web::wifi_connected || !startup) {
        M5.Lcd.setCursor(0, 16);
        M5.Lcd.fillRect(0, 16, WIDTH, 40 - 16, BACKGROUND);
        if (Web::wifi_connected) {
            M5.Lcd.printf("SSID=%s\r\n", Web::currentSSID);
            M5.Lcd.printf("LOIP=%s", Web::currentIP);
        } else {
            M5.Lcd.printf("Bad Wifi");
        }
    }

    // Mode
    // ! I'm using if else instead of switch
    // ! because I want to declare some vars on the fly

    bool mode_changed = false;
    if (dmode != current_mode) {
        send_buzzer(DMODE_SWITCH_FREQ);
        mode_changed = true;
    }

    M5.Lcd.setCursor(0, 40);
    if (current_mode == WebMode) {
        if (ma_web_total_visits != Web::total_visits || mode_changed) {
            M5.Lcd.fillRect(0, 40, WIDTH, HEIGHT - 40, BACKGROUND);
            M5.Lcd.setTextSize(2);
            M5.Lcd.printf("VIS=%u\r\n", Web::total_visits);
            if (ma_web_total_visits != Web::total_visits) {
                send_buzzer(DWEB_CHANGED_FREQ);
                M5.Lcd.printf("%s", Web::message);
            }
        }
    } else if (current_mode == BatteryMode) {
        if (dbat_perc != bat_perc || mode_changed) {
            M5.Lcd.fillRect(0, 40, WIDTH, HEIGHT - 40, BACKGROUND);
            M5.Lcd.setTextSize(2);
            M5.Lcd.printf("BAT=%.2f%%", bat_perc * 100.0f);
        }
    } else if (current_mode == IMUMode) {
        static uint32_t last = millis();
        uint32_t now = millis();
        if (now - last > 200) {
            last = now;
            M5.Lcd.fillRect(0, 40, WIDTH, HEIGHT - 40, BACKGROUND);
            M5.Lcd.printf("GYRO=(%.2f,%.2f,%.2f)\r\n", IMUData::gyro_x, IMUData::gyro_y,
                          IMUData::gyro_z);
            M5.Lcd.printf("ACCE=(%.2f,%.2f,%.2f)\r\n", IMUData::acc_x, IMUData::acc_y,
                          IMUData::acc_z);
            M5.Lcd.printf("PITC=%.2f\r\nROLL=%.2f\r\nYAW=%.2f\r\n", IMUData::pitch, IMUData::roll,
                          IMUData::yaw);
            M5.Lcd.printf("TEMP=%.2f C", IMUData::temp);
        }
    } else if (current_mode == LogoMode) {
        if (mode_changed) {
            M5.Lcd.fillRect(0, 40, WIDTH, HEIGHT - 40, BACKGROUND);
            M5.Lcd.setTextSize(2);
            M5.Lcd.printf("ESP ROCK\r\nVER=0.0.1\r\n");
            M5.Lcd.printf("WE ROCK\r\nTHE NET!!\r\n^_^\r\nUwU\r\nOwO");
        }
    }
    M5.Lcd.setTextSize(1);

    ma_web_total_visits = Web::total_visits;
    dbat_perc = bat_perc;
    dwifi_connected = Web::wifi_connected;
    dmode = current_mode;
    startup = true;
}
