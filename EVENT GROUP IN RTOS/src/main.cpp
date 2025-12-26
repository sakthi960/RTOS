#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/* Event bits */
#define EVENT_WIFI   (1 << 0)
#define EVENT_SENSOR (1 << 1)

/* Handles */
EventGroupHandle_t systemEvents;
TaskHandle_t notifyTaskHandle;

/* ---------------- EVENT SETTER TASK ---------------- */
void Task_SetEvents(void *pv) {
  vTaskDelay(pdMS_TO_TICKS(2000));
  xEventGroupSetBits(systemEvents, EVENT_WIFI);
  Serial.println("WiFi Connected");

  vTaskDelay(pdMS_TO_TICKS(2000));
  xEventGroupSetBits(systemEvents, EVENT_SENSOR);
  Serial.println("Sensor Ready");

  vTaskDelete(NULL);
}

/* ---------------- EVENT WAITER TASK ---------------- */
void Task_WaitEvents(void *pv) {
  Serial.println("Waiting for WiFi & Sensor...");

  xEventGroupWaitBits(
    systemEvents,
    EVENT_WIFI | EVENT_SENSOR, // Wait for both
    pdTRUE,                    // Clear bits on exit
    pdTRUE,                    // Wait for ALL bits
    portMAX_DELAY
  );

  Serial.println("All system events received");
  xTaskNotifyGive(notifyTaskHandle);   // Notify another task

  vTaskDelete(NULL);
}

/* ---------------- NOTIFICATION RECEIVER TASK ---------------- */
void Task_Notification(void *pv) {
  Serial.println("Notification task waiting...");

  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  Serial.println("Task notification received!");
  vTaskDelete(NULL);
}

/* ---------------- SETUP ---------------- */
void setup() {
  Serial.begin(115200);

  systemEvents = xEventGroupCreate();

  xTaskCreate(Task_Notification, "Notify", 2048, NULL, 1, &notifyTaskHandle);
  xTaskCreate(Task_WaitEvents,   "WaitEvt", 2048, NULL, 2, NULL);
  xTaskCreate(Task_SetEvents,    "SetEvt",  2048, NULL, 3, NULL);
}

void loop() {
}
