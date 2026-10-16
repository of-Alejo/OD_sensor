## Sensor de Oxígeno Disuelto (OD)

- **Función:** Mide la concentración de oxígeno presente en el agua.
- **Sensor base:** Su comportamiento replica las características operativas del sensor electroquímico SEN0237-A.
- **Rango de operación:** Actúa estrictamente en el rango de 0 a 20 mg/L (o ppm).
- **Hardware y pines:** La emulación se ejecuta en el ESP32 Wroom 32 (30 pines), utilizando específicamente el Pin 25, el cual corresponde a uno de los conversores digital-analógico (DAC) nativos de la placa.
- **Tipo de salida:** Entrega una salida puramente analógica. El código convierte el valor simulado en un voltaje físico real escalado entre 0V y 3.3V, engañando al nodo principal para que lo lea a través de su ADC como si fuera el electrodo físico original.
- **Dinámica de cambio:** Inicia en un valor estable (ej. 7.5 ppm) y ejecuta un algoritmo matemático de caminata aleatoria (Random Walk). Este algoritmo le suma o resta pequeñas fluctuaciones de ± 0.10 ppm en cada ciclo, generando una curva de voltaje suave que emula la cinética del agua sin dar saltos bruscos irreales, acotada siempre para no exceder los límites físicos del hardware.