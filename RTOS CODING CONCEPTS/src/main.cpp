#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

/* Handles */
QueueHandle_t dataQueue;
SemaphoreHandle_t signalSem;
SemaphoreHandle_t printMutex;
TimerHandle_t softTimer;

/* ---------------- TASK 1 : PRODUCER ---------------- */
void Task_Producer(void *pv) {
  int value = 0;
  while (1) {
    xQueueSend(dataQueue, &value, portMAX_DELAY);

    xSemaphoreTake(printMutex, portMAX_DELAY);
    Serial.printf("Producer sent: %d\n", value);
    xSemaphoreGive(printMutex);

    value++;
    vTaskDelay(pdMS_TO_TICKS(1000));   // Blocked state
  }
}

/* ---------------- TASK 2 : CONSUMER ---------------- */
void Task_Consumer(void *pv) {
  int rx;
  while (1) {
    xQueueReceive(dataQueue, &rx, portMAX_DELAY);

    xSemaphoreTake(printMutex, portMAX_DELAY);
    Serial.printf("Consumer received: %d\n", rx);
    xSemaphoreGive(printMutex);
  }
}

/* ---------------- TASK 3 : SIGNAL WAITER ---------------- */
void Task_Signal(void *pv) {
  while (1) {
    xSemaphoreTake(signalSem, portMAX_DELAY);

    xSemaphoreTake(printMutex, portMAX_DELAY);
    Serial.println("Semaphore signal received");
    xSemaphoreGive(printMutex);
  }
}

/* ---------------- TIMER CALLBACK ---------------- */
void TimerCallback(TimerHandle_t xTimer) {
  xSemaphoreGive(signalSem);   // Signal task
}

/* ---------------- SETUP ---------------- */
void setup() {
  Serial.begin(115200);

  /* Create RTOS objects */
  dataQueue  = xQueueCreate(5, sizeof(int));
  signalSem  = xSemaphoreCreateBinary();
  printMutex = xSemaphoreCreateMutex();

  softTimer = xTimerCreate(
    "SoftTimer",
    pdMS_TO_TICKS(3000),   // 3 seconds
    pdTRUE,                // Auto reload
    NULL,
    TimerCallback
  );

  /* Create Tasks */
  xTaskCreate(Task_Producer, "Producer", 2048, NULL, 2, NULL);
  xTaskCreate(Task_Consumer, "Consumer", 2048, NULL, 1, NULL);
  xTaskCreate(Task_Signal,   "Signal",   2048, NULL, 3, NULL);

  xTimerStart(softTimer, 0);
}

void loop() {
  // Empty: RTOS scheduler is running
}
