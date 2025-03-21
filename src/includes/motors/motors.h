#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
#include <Encoder.h>
#include <EEPROM.h>
#include "pidautotuner.h"
#include <TimerOne.h>

class Motors {
public:
    static void initialize();
    static void autoTunePID();
    static void savePIDParameters();
    static void moveForward();
    static void moveBackwards();
    static void turnLeft();
    static void turnRight();
    static void stopMotors();
    static void updateMotorSpeed();
    static void updateMotorSpeedISR();
    static void setMotorTarget(long leftTarget, long rightTarget);
    static bool isCommandCompleted();

    static long getTargetLeft() { return targetLeft; }
    static long getTargetRight() { return targetRight; }
    static long getStepLeft() { return stepLeft; }
    static long getStepRight() { return stepRight; }
    static int getDirection() { return direction; }

private:
    static void saveCalibration();
    static void calibrateMotors(long stepLeft, long stepRight);
    static float computePID(float target, float current, float &integral, float &lastError);

    static const int motorPins[2][2];
    static Encoder knobLeft;
    static Encoder knobRight;

    static float KpLeft, KiLeft, KdLeft;
    static float KpRight, KiRight, KdRight;
    static float integralLeft, lastErrorLeft;
    static float integralRight, lastErrorRight;

    static const int minSpeed;
    static const int maxSpeed;
    static const int forwardStep;
    static const int turnStep;

    static int leftCalib;
    static int rightCalib;
    static long targetLeft;
    static long targetRight;
    static long stepLeft;
    static long stepRight;
    static int direction;
    static bool commandCompleted;
};

#endif // MOTORS_H
