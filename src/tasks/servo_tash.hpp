#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static constexpr char const* TASK_NAME = "ServoTask";
static constexpr int STACK_SIZE = 2048;

inline TaskHandle_t* handle = nullptr;

namespace tasks {
  void startServoTask();
  void cancelServoTask();
} // namespace tasks

void mServoTask(void* pvParameters);
esp_err_t mTryConfigureLEDC();
uint32_t mTimeToDuty(uint32_t us);
void mWriteToServo(uint32_t us);
