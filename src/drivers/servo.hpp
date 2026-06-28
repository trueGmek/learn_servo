#pragma once

#include "driver/ledc.h"

class Servo {
private:
  static constexpr int PWM_FREQUENCY = 50; // 50 Hz
  static constexpr ledc_timer_bit_t PWM_RESOLUTION = ledc_timer_bit_t::LEDC_TIMER_14_BIT;

  int mPin;
  uint32_t mMinTime;
  uint32_t mMaxTime;
  uint32_t mTime;

public:
  static constexpr int PWM_PERIOD = 20; // milliseconds

private:
  static uint32_t TimeToDuty(uint32_t microseconds);
  esp_err_t Configure();

public:
  Servo(int pin, uint32_t minTime = 800, uint32_t maxTime = 2200);
  void Write(uint32_t microseconds);
};
