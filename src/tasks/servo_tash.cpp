#include "driver/ledc.h"
#include <algorithm>
#include <tasks/servo_tash.hpp>

#include <esp_check.h>
#include <esp_log.h>

#define LEDC_RES_BITS ledc_timer_bit_t::LEDC_TIMER_14_BIT // -> 16384 steps
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_FREQ_HZ 50 // 20 ms period
#define SERVO_GPIO 18   // any output-capable pin (avoid 34-39, input-only)
#define PERIOD 20

void tasks::startServoTask() {
  BaseType_t returnValue = xTaskCreate(mServoTask, TASK_NAME, STACK_SIZE, nullptr, 2, handle);
  if (returnValue != pdPASS) {
    ESP_LOGE(TASK_NAME, "Couldn't start a task with name: %s", TASK_NAME);
  }
}

void tasks::cancelServoTask() {
  if (handle == nullptr) {
    ESP_LOGE(TASK_NAME, "Trying to cancel a dead task!");
  }
  vTaskDelete(*handle);
}

esp_err_t mTryConfigureLEDC() {
  ledc_timer_config_t timer = {
      .speed_mode = LEDC_MODE,
      .duty_resolution = LEDC_RES_BITS,
      .timer_num = LEDC_TIMER,
      .freq_hz = LEDC_FREQ_HZ,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  esp_err_t timer_config_ret = ledc_timer_config(&timer);
  // TODO: MAKE A NOTE ABOUT THIS
  ESP_RETURN_ON_ERROR(timer_config_ret, TASK_NAME, "Found error when configuring ledc timer");

  ledc_channel_config_t channel = {
      .gpio_num = SERVO_GPIO,
      .speed_mode = LEDC_MODE,
      .channel = LEDC_CHANNEL,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER,
      .duty = 0,
      .hpoint = 0,
  };

  esp_err_t channel_config_ret = ledc_channel_config(&channel);
  ESP_RETURN_ON_ERROR(channel_config_ret, TASK_NAME, "Found an error when configuring ledc channel");

  return ESP_OK;
}

void mServoTask(void* pvParameters) {
  ESP_ERROR_CHECK(mTryConfigureLEDC());
  int increment = 10;
  while (true) {
    for (int i = 800; i < 2200; i += increment) {
      mWriteToServo(i);
      vTaskDelay(pdMS_TO_TICKS(PERIOD));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void mWriteToServo(uint32_t us) {
  us = std::clamp(us, uint32_t(800), uint32_t(2200)); // clamp to HD-1900A safe range

  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, mTimeToDuty(us));
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

// Converts pulse width in microseconds(us) to a raw LEDC duty cycle count.
// LEDC doesn't use percentages to define duty cycle.
// Duty cycle is a counter that goes from zero to the highest
// resolution value. At 14-bit resolution the counter goes from 0 -> 16383;
// the output is high until the counter reaches the duty value, then low for the rest of the period.
//   duty = (us << 14) / 20000
// Example: 1000 us -> duty ~819 -> high for 819/16384 of the 20 ms period
uint32_t mTimeToDuty(uint32_t us) {
  return (us << LEDC_RES_BITS) / PERIOD * 1000; //* 1000 because period is defined in ms
}
