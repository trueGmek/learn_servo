#include "drivers/servo.hpp"
#include <tasks/servo_tash.hpp>

#include <esp_log.h>

void tasks::StartServoTask() {
  BaseType_t returnValue = xTaskCreate(ServoTask, TASK_NAME, STACK_SIZE, nullptr, 2, handle);
  if (returnValue != pdPASS) {
    ESP_LOGE(TASK_NAME, "Couldn't start a task with name: %s", TASK_NAME);
  }
}

void tasks::CancelServoTask() {
  if (handle == nullptr) {
    ESP_LOGE(TASK_NAME, "Trying to cancel a dead task!");
  }
  vTaskDelete(*handle);
}

void ServoTask(void* pvParameters) {
  Servo servo{18, 165.0};

  int increment = 3;

  while (true) {
    for (int i = 0; i < 165; i += increment) {
      servo.Move((float)i);
      vTaskDelay(pdMS_TO_TICKS(Servo::PWM_PERIOD));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
