#include <algorithm>

#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/ledc_types.h"

#define SERVO_GPIO 18 // any output-capable pin (avoid 34-39, input-only)
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_RES_BITS ledc_timer_bit_t::LEDC_TIMER_14_BIT // -> 16384 steps
#define LEDC_FREQ_HZ 50                                   // 20 ms period
#define SMALLEST_DELAY_MILS 20

static inline uint32_t us_to_duty(uint32_t us) {
  return (us << LEDC_RES_BITS) / 20000; // 20000 us = full period
}

static void servo_write_us(uint32_t us) {
  us = std::clamp(us, uint32_t(800), uint32_t(2200)); // clamp to HD-1900A safe range

  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, us_to_duty(us));
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

extern "C" void app_main(void) {
  vTaskDelay(pdMS_TO_TICKS(5000));

  ledc_timer_config_t timer = {
      .speed_mode = LEDC_MODE,
      .duty_resolution = LEDC_RES_BITS,
      .timer_num = LEDC_TIMER,
      .freq_hz = LEDC_FREQ_HZ,
      .clk_cfg = LEDC_AUTO_CLK,
  };

  ESP_ERROR_CHECK(ledc_timer_config(&timer));

  ledc_channel_config_t channel = {
      .gpio_num = SERVO_GPIO,
      .speed_mode = LEDC_MODE,
      .channel = LEDC_CHANNEL,
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = LEDC_TIMER,
      .duty = 0,
      .hpoint = 0,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&channel));

  int increment = 10;
  while (true) {

    for (int i = 800; i < 2200; i += increment) {
      servo_write_us(i); // ~one end of 90° travel
      vTaskDelay(pdMS_TO_TICKS(SMALLEST_DELAY_MILS));
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
