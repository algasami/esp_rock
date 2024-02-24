#include "utils.hpp"

namespace IMUData {
float gyro_x = 0.0f, gyro_y = 0.0f, gyro_z = 0.0f;
float acc_x = 0.0f, acc_y = 0.0f, acc_z = 0.0f;
float pitch = 0.0f, roll = 0.0f, yaw = 0.0f;
float temp = 0.0f;
} // namespace IMUData

void handle_imu() {
  if (current_mode != Modes::IMUMode) {
    return;
  }
  M5.Imu.getGyroData(&IMUData::gyro_x, &IMUData::gyro_y, &IMUData::gyro_z);
  M5.Imu.getAccelData(&IMUData::acc_x, &IMUData::acc_y, &IMUData::acc_z);
  M5.Imu.getAhrsData(&IMUData::pitch, &IMUData::roll, &IMUData::yaw);
  M5.Imu.getTempData(&IMUData::temp); // celsius
}