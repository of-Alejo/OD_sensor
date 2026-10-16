#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "OdEmulator.h"

// --- Configuracion de Pines ---
const uint8_t k_u8PinDacOutput = 25; // Pin DAC1 para salida analogica

// --- Variables Globales y FreeRTOS ---
OdEmulator objOdSensor;
SemaphoreHandle_t xMutexSerial;

OdEmulator::OdEmulator() {
    fltCurrentOdPpm = 7.5f; // Valor inicial estable
    randomSeed(analogRead(0));
}

void OdEmulator::vUpdateSimulation() {
    // Variacion aleatoria suave emulando el comportamiento del agua
    float fltDelta = ((float)random(-10, 11) / 100.0f);
    fltCurrentOdPpm += fltDelta;

    if (fltCurrentOdPpm > k_fltMaxOdPpm) fltCurrentOdPpm = k_fltMaxOdPpm;
    if (fltCurrentOdPpm < k_fltMinOdPpm) fltCurrentOdPpm = k_fltMinOdPpm;
}

float OdEmulator::fltGetOdPpm() {
    return fltCurrentOdPpm;
}

uint8_t OdEmulator::u8CalculateDacValue(float fltVoltageRef) {
    // Convierte el valor de concentracion a un voltaje proporcional (0 a 3.3V)
    // Suponiendo calibracion lineal basica de la libreria DFRobot
    float fltSimulatedVoltage = (fltCurrentOdPpm / k_fltMaxOdPpm) * fltVoltageRef;
    return (uint8_t)((fltSimulatedVoltage / 3.3f) * 255.0f);
}

// --- Prototipo de Tarea ---
void vTaskOdDacOutput(void *pvParameters);

void setup() {
    Serial.begin(115200);
    while (!Serial);

    xMutexSerial = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(
        vTaskOdDacOutput,
        "TaskOD",
        2048,
        NULL,
        1,
        NULL,
        1 // Ejecutar en el Core 1
    );

    vTaskDelete(NULL); // Eliminar tarea loop
}

void loop() {}

void vTaskOdDacOutput(void *pvParameters) {
    const TickType_t xDelayTicks = pdMS_TO_TICKS(2000); // 2 Hz para bajo consumo
    TickType_t xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xDelayTicks);

        objOdSensor.vUpdateSimulation();
        
        // Calcular y escribir valor analogo (0-255)
        uint8_t u8DacSignal = objOdSensor.u8CalculateDacValue(3.3f);
        dacWrite(k_u8PinDacOutput, u8DacSignal);

        if (xSemaphoreTake(xMutexSerial, portMAX_DELAY) == pdTRUE) {
            Serial.print(F("[OD] Emulacion -> Concentracion: "));
            Serial.print(objOdSensor.fltGetOdPpm(), 2);
            Serial.print(F(" ppm | Salida DAC: "));
            Serial.println(u8DacSignal);
            xSemaphoreGive(xMutexSerial);
        }
    }
}