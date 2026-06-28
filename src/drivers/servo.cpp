#include <algorithm>
#include <drivers/servo.hpp>
#include <esp_check.h>
#include <esp_log.h>

#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0

Servo::Servo(int pin, uint32_t minTime, uint32_t maxTime) : mPin(pin), mMinTime(minTime), mMaxTime(maxTime) {
  ESP_ERROR_CHECK(Configure());
}

esp_err_t Servo::Configure() {
  ledc_timer_config_t timer = {
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .duty_resolution = PWM_RESOLUTION,
      .timer_num = LEDC_TIMER,
      .freq_hz = PWM_FREQUENCY,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  esp_err_t timerConfigRet = ledc_timer_config(&timer);
  // TODO: MAKE A NOTE ABOUT THIS
  ESP_RETURN_ON_ERROR(timerConfigRet, "TODO:NAME", "Found error when configuring ledc timer");

  ledc_channel_config_t channel = {
      .gpio_num = mPin,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .channel = LEDC_CHANNEL,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER,
      .duty = 0,
      .hpoint = 0,
  };

  esp_err_t channelConfigRet = ledc_channel_config(&channel);
  ESP_RETURN_ON_ERROR(channelConfigRet, "TODO:NAME", "Found an error when configuring ledc channel");

  return ESP_OK;
}

void Servo::Write(uint32_t microseconds) {
  microseconds = std::clamp(microseconds, mMinTime, mMaxTime);
  mTime = TimeToDuty(microseconds);

  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL, mTime);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL);
}

// Converts pulse width in microseconds(us) to a raw LEDC duty cycle count.
// LEDC doesn't use percentages to define duty cycle.
// Duty cycle is a counter that goes from zero to the highest
// resolution value. At 14-bit resolution the counter goes from 0 -> 16383;
// the output is high until the counter reaches the duty value, then low for the rest of the period.
//   duty = (us << 14) / 20000
// Example: 1000 us -> duty ~819 -> high for 819/16384 of the 20 ms period
uint32_t Servo::TimeToDuty(uint32_t microseconds) {
  return (microseconds << PWM_RESOLUTION) / (PWM_PERIOD * 1000); //* 1000 because period is defined in ms
}
