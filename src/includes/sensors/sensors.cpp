#include "globals.h"
#include "main.h"
#include "sensors.h"
#include <Arduino.h>

// Sensör pinleri tanımlamaları
const int Sensors::sO1 = SENSOR_L;
const int Sensors::sO2 = SENSOR_F;
const int Sensors::sO3 = SENSOR_R;
const int Sensors::sG = SENSOR_GROUND;
const int Sensors::lineSensor = SENSOR_LINE;
const int Sensors::button = START_BUTTON;
// Sensör durum değişkenleri
bool Sensors::oL = false;
bool Sensors::oR = false;
bool Sensors::oF = false;
bool Sensors::oG = false;
bool Sensors::sB = false;

void Sensors::initialize() {
    pinMode(sO1, INPUT);
    pinMode(sO2, INPUT);
    pinMode(sO3, INPUT);
    pinMode(sG, INPUT);
    pinMode(lineSensor, INPUT);
    pinMode(button, INPUT);
}

void Sensors::update() {
    oF = !digitalRead(sO2);
    oL = !digitalRead(sO1);
    oR = !digitalRead(sO3);
    oG = digitalRead(sG);
    sB= digitalRead(button);



}

bool Sensors::isObstacleFront() {
    return oF;
}

bool Sensors::isObstacleLeft() {
    return oL;
}

bool Sensors::isObstacleRight() {
    return oR;
}

bool Sensors::isGoalDetected() {
    return oG;
}