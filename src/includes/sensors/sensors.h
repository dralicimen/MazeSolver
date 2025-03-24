#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

class Sensors {
public:
    static const int sO1;
    static const int sO2;
    static const int sO3;
    static const int sG;
    static const int lineSensor;
    static const int button;

    static bool oL;
    static bool oR;
    static bool oF;
    static bool oG;
    static bool sB;

    static void initialize();
    static void update();

    static bool isObstacleFront();
    static bool isObstacleLeft();
    static bool isObstacleRight();
    static bool isGoalDetected();
};

#endif // SENSORS_H
