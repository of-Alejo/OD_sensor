#ifndef OD_EMULATOR_H
#define OD_EMULATOR_H

#include <Arduino.h>

// Modulo para emular la cinetica quimica del oxigeno disuelto
class OdEmulator {
private:
    float fltCurrentOdPpm;      
    const float k_fltMaxOdPpm = 20.0f; // Limite superior SEN0237-A[cite: 10]
    const float k_fltMinOdPpm = 0.0f;  // Limite inferior SEN0237-A[cite: 10]

public:
    OdEmulator();
    void vUpdateSimulation();
    float fltGetOdPpm();
    uint8_t u8CalculateDacValue(float fltVoltageRef);
};

#endif // OD_EMULATOR_H