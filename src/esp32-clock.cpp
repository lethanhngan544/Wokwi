// #include <Arduino.h>
// #include <driver/dac.h>

// #define SIGNAL_OUT_PIN 25

// enum WAVE_MODE {
//   TRIANGLE,
//   SQUARE,
//   SINE
// };

// WAVE_MODE mode = WAVE_MODE::SINE;  // Change to SQUARE, SINE, or TRIANGLE

// const int SINE_TABLE_SIZE = 256;
// uint8_t sineTable[SINE_TABLE_SIZE];

// #define FREQ_HZ 1000  // Adjust frequency (Hz)
// #define SAMPLE_RATE (FREQ_HZ * SINE_TABLE_SIZE)  // Adjust sample rate dynamically


// void setup() {
//   Serial.begin(115200);
//   pinMode(SIGNAL_OUT_PIN, OUTPUT);
//   generateSineTable();
// }

// void loop() {
//   static uint16_t i = 0;
//   static uint8_t triangleVal = 0;
//   static int8_t triangleStep = 2;
//   static uint32_t lastUpdate = 0;
//   uint32_t now = micros();

//   // Calculate the time interval for the desired frequency
//   uint32_t interval = 1000000 / SAMPLE_RATE;  // Microseconds per step

//   if (now - lastUpdate >= interval) {
//     lastUpdate = now;
//     int signal = 0;

//     switch (mode) {
//       case SQUARE:
//         signal = (i % 2) ? 255 : 0;
//         break;

//       case SINE:
//         signal = sineTable[i % SINE_TABLE_SIZE];
//         break;

//       case TRIANGLE:
//         signal = triangleVal;
//         triangleVal += triangleStep;
//         if (triangleVal == 255 || triangleVal == 0) triangleStep = -triangleStep;
//         break;
//     }

//     dacWrite(SIGNAL_OUT_PIN, signal);
//     i++;
//   }
// }
