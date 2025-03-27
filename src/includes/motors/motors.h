#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>
#include <Encoder.h>
#include <EEPROM.h>
#include <TimerOne.h>
#include "globals.h"

class Motors {
public:
    static void initialize();
    static void savePIDParameters();
    static void moveForward();
    static void moveBackwards();
    static void turnLeft();
    static void turnRight();
    static void turnBack();
    static void stopMotors();
    static void stopLeft();
    static void stopRight();
    static void updateMotorSpeed();
    static void updateMotorSpeedISR();
    static void sencron();
    static void setMotorTarget(long leftTarget, long rightTarget);
    static bool isCommandCompleted();

    static long getTargetLeft() { return targetLeft; }
    static long getTargetRight() { return targetRight; }
    static long getStepLeft();
    static long getStepRight();
    static int getDirection() { return direction; }

private:
    static void saveCalibration();
    static void calibrateMotors(long stepLeft, long stepRight);

    static const int motorPins[2][2];
    static Encoder knobLeft;
    static Encoder knobRight;

    static float KpLeft, KiLeft, KdLeft;
    static float KpRight, KiRight, KdRight;
    static float integralLeft, lastErrorLeft;
    static float integralRight, lastErrorRight;
    static float outputLeft, outputRight;

    static int minSpeed;
    static int maxSpeed;
    static const int defaultMaxSpeed;
    static const int turnSpeed;
    static const int forwardStep;
    static const int turnStep;

    static float leftCalib;
    static float rightCalib;

    // PID tabanlı kalibrasyon için yeni değişkenler
    static float KpCalib, KiCalib, KdCalib;
    static float leftCalibIntegral, leftCalibLastError;
    static float rightCalibIntegral, rightCalibLastError;

    static long targetLeft;
    static long targetRight;
    static long stepLeft;
    static long stepRight;
    static unsigned long startTimeLeft;
    static unsigned long startTimeRight;
    static unsigned long lastSampleTimeLeft;
    static unsigned long lastSampleTimeRight;
    static long lastSampleStepLeft;
    static long lastSampleStepRight;
    static int direction;
    static bool commandCompleted;
    static bool commandLeftCompleted;
    static bool commandRightCompleted;

    static unsigned long lastUpdateTime;
    static float integralMax;
};

#endif // MOTORS_H
