#include <tasks/servo_tash.hpp>

extern "C" void app_main(void) {
  vTaskDelay(pdMS_TO_TICKS(5000));
  tasks::StartServoTask();
}
