#include <Arduino.h>
#include "main.h"
#include "events.h"
#include "logic.h"

bool started = false; // Başlatma durumu

void setup() {
    Serial.begin(115200);
    motorControl.initialize();
    sensorControl.initialize();
    mazeControl.initialize();
    randomSeed(analogRead(0));

    pinMode(START_BUTTON, INPUT_PULLUP); // A1 pini, buton bağlı ve ters lojik (LOW = basıldı)
}

void loop() {
    if (!started) {
        if (digitalRead(START_BUTTON) == LOW) {
            started = true;
            Serial.println("Başlatma butonuna basıldı. Robot başlıyor...");
            delay(300); // debounce için
        } else {
            // Bekleme animasyonu/logosu vs. eklenebilir
            return;
        }
    }

    sensorControl.update();

    if (!motorControl.isCommandCompleted() && sensorControl.isObstacleFront() && command == 1) {
        onObstacleDetectedWhileMoving();
        return;
    }

    if (!motorControl.isCommandCompleted() && command != -1) {
        if (millis() - commandStartTime > COMMAND_TIMEOUT_MS) {
            Serial.println("ZAMAN AŞIMI: Komut manuel olarak tamamlandı.");
            onCommandCompleted();
            return;
        }
    }

    if (motorControl.isCommandCompleted()) {
        onCommandCompleted();
    }

    motorControl.updateMotorSpeed();
    delay(10);
}
