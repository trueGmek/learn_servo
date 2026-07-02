#pragma once

#include "driver/ledc.h"

class Servo {
private:
  static constexpr int PWM_FREQUENCY = 50; // 50 Hz
  static constexpr ledc_timer_bit_t PWM_RESOLUTION = ledc_timer_bit_t::LEDC_TIMER_14_BIT;

  uint32_t mTime;

  const int mPin;
  const uint32_t mMinTime;
  const uint32_t mMaxTime;
  const float mMaxAngle;

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  char mTag[16]{};

public:
  static constexpr int PWM_PERIOD = 20; // milliseconds

private:
  static uint32_t TimeToDuty(uint32_t microseconds);
  esp_err_t Configure();

public:
  Servo(int pin, float maxAngle, uint32_t minTime = 800, uint32_t maxTime = 2200);

  // Emit PWM with up time specified as argument microseconds
  void Write(uint32_t microseconds);

  // Moves the servo to a specific angle defined in degrees
  void Move(float angle);
};
