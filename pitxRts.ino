#include <Arduino.h>
#include "DHT.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

QueueHandle_t sensorQueue;
EventGroupHandle_t eventGroup;

void Task_SensorRead(void *pvParameters) {
    float data[2];
    while (1) {
        data[0] = dht.readTemperature();
        data[1] = dht.readHumidity();
        xQueueSend(sensorQueue, &data, portMAX_DELAY);
        xEventGroupSetBits(eventGroup, BIT0);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void Task_KontrolVentilasi(void *pvParameters) {
    float data[2];
    while (1) {
        xEventGroupWaitBits(eventGroup, BIT0, pdTRUE, pdFALSE, portMAX_DELAY);
        xQueueReceive(sensorQueue, &data, portMAX_DELAY);
        if (data[0] > 30) digitalWrite(12, HIGH);
        else digitalWrite(12, LOW);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(12, OUTPUT);
    dht.begin();

    sensorQueue = xQueueCreate(5, sizeof(float[2]));
    eventGroup = xEventGroupCreate();

    xTaskCreate(Task_SensorRead, "SensorRead", 2048, NULL, 2, NULL);
    xTaskCreate(Task_KontrolVentilasi, "Ventilasi", 2048, NULL, 1, NULL);
}

void loop() {
    vTaskDelay(1000);
}
