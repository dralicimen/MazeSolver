#include <Arduino.h>
#include "main.h"
#include "events.h"
#include "logic.h"

void setup() {
    Serial.begin(115200);
    motorControl.initialize();
    sensorControl.initialize();
    mazeControl.initialize();
    randomSeed(analogRead(0));
}

void loop() {
    sensorControl.update();

    if (!motorControl.isCommandCompleted() && sensorControl.isObstacleFront() && command == 1) {
        onObstacleDetectedWhileMoving();
        return;
    }

    if (motorControl.isCommandCompleted()) {
        onCommandCompleted();
    }

    motorControl.updateMotorSpeed();
    delay(10);
}
