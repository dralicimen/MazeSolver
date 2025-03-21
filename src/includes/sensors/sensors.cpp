#include "sensors.h"
#include <Arduino.h>

// Sensör pinleri tanımlamaları
const int Sensors::sO1 = 11;
const int Sensors::sO2 = 12;
const int Sensors::sO3 = 8;
const int Sensors::sG = 13;
const int Sensors::lineSensor = A0;

// Sensör durum değişkenleri
bool Sensors::oL = false;
bool Sensors::oR = false;
bool Sensors::oF = false;
bool Sensors::oG = false;

void Sensors::initialize() {
    pinMode(sO1, INPUT);
    pinMode(sO2, INPUT);
    pinMode(sO3, INPUT);
    pinMode(sG, INPUT);
    pinMode(lineSensor, INPUT);
}

void Sensors::update() {
    oF = !digitalRead(sO1);
    oL = !digitalRead(sO2);
    oR = !digitalRead(sO3);
    oG = (analogRead(lineSensor) < 100);
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