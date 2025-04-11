#include <Arduino.h>
#include <BluetoothSerial.h>

#define SIGNAL_OUT_PIN 25  // GPIO25 = DAC1
#define ADC_PIN 34  // Or any ADC-capable pin
#define TABLE_SIZE 512     // Resolution of the waveform

#define SAMPLE_RATE_HZ 10  // Samples per second

enum WaveType { SINE, SQUARE, TRIANGLE, SAWTOOTH };
WaveType mode = TRIANGLE;  // Change this to SQUARE or TRIANGLE

uint8_t sineWaveTable[TABLE_SIZE];
uint8_t squareWaveTable[TABLE_SIZE];
uint8_t triangleWaveTable[TABLE_SIZE];
uint8_t sawtoothWaveTable[TABLE_SIZE];
uint8_t* waveTables[4] = {sineWaveTable, squareWaveTable, triangleWaveTable, sawtoothWaveTable};

int frequency = 5;  // Set desired waveform frequency in Hz
float amplitude = 1.0f;
TaskHandle_t occiloscopeThread;

void generateWaveform(WaveType type) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        double real = 0.5 * (sin(4 * PI * (double)i / (double)TABLE_SIZE) + 1.0) * 255.0;
        sineWaveTable[i] = (uint8_t)real;
        squareWaveTable[i] = (i < TABLE_SIZE / 2) ? 255 : 0;
        sawtoothWaveTable[i] = (i * 255) / (TABLE_SIZE - 1);  // 0 → 255 over 128 samples
        if (i < TABLE_SIZE / 2) {
            triangleWaveTable[i] = (i * 255) / ((TABLE_SIZE / 2) - 1);  // 0 → 255
        } else {
            triangleWaveTable[i] = ((TABLE_SIZE - 1 - i) * 255) / ((TABLE_SIZE / 2) - 1);  // 255 → 0
        }
    }
  }

void occiloscopeThread_fn( void * parameter) {
    Serial.print("occiloscopeThread_fn() running on core ");
    Serial.println(xPortGetCoreID());
    unsigned long lastSampleTime = 0;
    analogReadResolution(12);  // Optional: ESP32 ADC has 12-bit resolution (0-4095)
    BluetoothSerial btSerial;
    btSerial.begin("ESP32_Oscilloscope");
    for(;;) {
        unsigned long now = micros();
        if (now - lastSampleTime >= (1000000 / SAMPLE_RATE_HZ)) {
            lastSampleTime = now;
      
            int adcValue = analogRead(ADC_PIN);  // Read voltage
            btSerial.println(adcValue);          // Send over Bluetooth
        }
    }
    Serial.print("occiloscopeThread_fn() Shutting down");
}

void setup() {
    Serial.begin(9600);
    Serial.print("setup() running on core ");
    Serial.println(xPortGetCoreID());
    generateWaveform(mode);

    //Create task for occiloscope
    xTaskCreatePinnedToCore(
        occiloscopeThread_fn, /* Function to implement the task */
        "Task1", /* Name of the task */
        10000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        0,  /* Priority of the task */
        &occiloscopeThread,  /* Task handle. */
        0); /* Core where the task should run */

}

void loop() {
    static int i = 0;
    // Output current waveform
    dacWrite(SIGNAL_OUT_PIN, (uint8_t)((float)waveTables[mode][i] * amplitude));
    i = (i + 1) % TABLE_SIZE;
    int delayPerSample = 1000000 / (frequency * TABLE_SIZE);  // µs per sample
    delayMicroseconds(delayPerSample);

    if (Serial.available()) {
        char c = Serial.read();
        if (c >= '0' && c <= '3') {
          mode = (WaveType)(c - '0');
          Serial.print("Waveform changed to: ");
          switch (mode) {
            case SINE: Serial.println("SINE"); break;
            case SQUARE: Serial.println("SQUARE"); break;
            case TRIANGLE: Serial.println("TRIANGLE"); break;
            case SAWTOOTH: Serial.println("SAWTOOTH"); break;
          }
        }

        if (c >= '4' && c <= '9') {
            int selected_frequency = (c - '0');

            switch (selected_frequency) {
                case 4: frequency -= 5; break;
                case 6: frequency += 5; break; 
                
                case 7: frequency -= 1; break;
                case 9: frequency += 1; break;

                case 8: amplitude += 0.01f; break;
                case 5: amplitude -= 0.01f; break;
            }
            if(frequency <= 0) frequency = 5;
            else if (frequency >= 2000) frequency = 2000;

            if(amplitude <= 0.0f) amplitude = 0.0f;
            else if (amplitude >= 1.0f) amplitude = 1.0f;
            Serial.print("Frequency changed to: ");
            Serial.println(frequency);

            Serial.print("Amplitude changed to: ");
            Serial.println(amplitude);
        }
    }
}
