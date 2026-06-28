#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static constexpr char const* TASK_NAME = "ServoTask";
static constexpr int STACK_SIZE = 4096;

inline TaskHandle_t* handle = nullptr;

namespace tasks {
  void StartServoTask();
  void CancelServoTask();
} // namespace tasks

void ServoTask(void* pvParameters);
esp_err_t TryConfigureLEDC();
uint32_t TimeToDuty(uint32_t us);
void WriteToServo(uint32_t us);
