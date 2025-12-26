#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void Task_A(void *pv) {
  while (1) {
    Serial.println("Task A running");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void Task_B(void *pv) {
  while (1) {
    Serial.println("Task B running");
    vTaskDelay(pdMS_TO_TICKS(1500));
  }
}

void setup() {
  Serial.begin(115200);

  xTaskCreate(Task_A, "Task A", 2048, NULL, 1, NULL);
  xTaskCreate(Task_B, "Task B", 2048, NULL, 1, NULL);
}

void loop() {
  // Empty - RTOS handles everything
}
