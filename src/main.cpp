#include <Arduino.h>
#include "main.h"
#include "events.h"
#include "logic.h"

bool started = false; // Başlatma durumu
int commands[4]={1,2,3,4};
int i=1;
void setup() {
    Serial.begin(115200);
    motorControl.initialize();
    sensorControl.initialize();
    mazeControl.initialize();
    motorControl.stopMotors();

}

void loop() {
    sensorControl.update();

    if (!started) {
        if (sensorControl.sB==true) {
            started = true;
            Serial.println("Başlatma butonuna basıldı. Robot başlıyor...");
            delay(300); // debounce için
        } else {
            // Bekleme animasyonu/logosu vs. eklenebilir
            return;
        }
    }



    if (motorControl.isCommandCompleted()) {
        delay(1000);
        onCommandCompleted();

    }

    motorControl.updateMotorSpeed();


}
