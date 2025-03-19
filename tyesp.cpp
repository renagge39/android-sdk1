#include <Arduino.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "plant_model.h"

#define SOIL_SENSOR_PIN 34
#define LDR_SENSOR_PIN 35
#define RELAY_PIN 5

const tflite::Model* model;
tflite::MicroInterpreter* interpreter;
TfLiteTensor* input;
TfLiteTensor* output;

void setup_tflite() {
    model = tflite::GetModel(plant_model);
    static tflite::AllOpsResolver resolver;
    static tflite::MicroInterpreter static_interpreter(model, resolver, nullptr, 4096, nullptr);
    interpreter = &static_interpreter;
    interpreter->AllocateTensors();
    input = interpreter->input(0);
    output = interpreter->output(0);
}

void setup() {
    Serial.begin(115200);
    pinMode(RELAY_PIN, OUTPUT);
    setup_tflite();
}

void loop() {
    float soil_moisture = analogRead(SOIL_SENSOR_PIN) / 4095.0;
    float ldr_value = analogRead(LDR_SENSOR_PIN) / 4095.0;

    input->data.f[0] = soil_moisture;
    input->data.f[1] = ldr_value;

    interpreter->Invoke();
    float prediction = output->data.f[0];

    if (prediction < 0.5) digitalWrite(RELAY_PIN, HIGH);
    else digitalWrite(RELAY_PIN, LOW);

    Serial.printf("Soil: %.2f, Light: %.2f, AI Prediction: %.2f\n", soil_moisture, ldr_value, prediction);
    delay(5000);
}
