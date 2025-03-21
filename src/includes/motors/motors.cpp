#include "motors.h"
#include <EEPROM.h>
#include "pidautotuner.h"
#include <TimerOne.h>

const int Motors::motorPins[2][2] = {{6, 5}, {10, 9}};
Encoder Motors::knobLeft(3, 7);
Encoder Motors::knobRight(2, 4);

float Motors::KpLeft = 1, Motors::KiLeft = 0.001, Motors::KdLeft = 0.1;
float Motors::KpRight = 1, Motors::KiRight = 0.001, Motors::KdRight = 0.1;
float Motors::integralLeft = 0, Motors::lastErrorLeft = 0;
float Motors::integralRight = 0, Motors::lastErrorRight = 0;

const int Motors::minSpeed = 50;
const int Motors::maxSpeed = 255;
const int Motors::forwardStep = 1800;
const int Motors::turnStep = 750;

int Motors::leftCalib = 0;
int Motors::rightCalib = 0;
long Motors::targetLeft = 0;
long Motors::targetRight = 0;
long Motors::stepLeft = 0;
long Motors::stepRight = 0;
int Motors::direction = 0;
bool Motors::commandCompleted = true;

void Motors::initialize() {
    for (int i = 0; i < 2; i++) {
        pinMode(motorPins[i][0], OUTPUT);
        pinMode(motorPins[i][1], OUTPUT);
    }
    Timer1.initialize(10000);
    Timer1.attachInterrupt(Motors::updateMotorSpeedISR);
    EEPROM.get(0, leftCalib);
    EEPROM.get(4, rightCalib);
    EEPROM.get(8, KpLeft);
    EEPROM.get(12, KiLeft);
    EEPROM.get(16, KdLeft);
    EEPROM.get(20, KpRight);
    EEPROM.get(24, KiRight);
    EEPROM.get(28, KdRight);
}

void Motors::autoTunePID() {
    PIDAutotuner tuner;
    tuner.setTargetInputValue(0);
    tuner.setLoopInterval(10000);
    tuner.setOutputRange(minSpeed, maxSpeed);
    tuner.setZNMode(PIDAutotuner::ZNModeNoOvershoot);
    tuner.setTuningCycles(10);
    tuner.startTuningLoop(micros());

    while (!tuner.isFinished()) {
        long stepLeft = knobLeft.read();
        long stepRight = knobRight.read();
        float outputLeft = tuner.tunePID(stepLeft, micros());
        float outputRight = tuner.tunePID(stepRight, micros());
        analogWrite(motorPins[0][0], outputLeft);
        analogWrite(motorPins[1][0], outputRight);
    }

    KpLeft = tuner.getKp();
    KiLeft = tuner.getKi();
    KdLeft = tuner.getKd();
    KpRight = tuner.getKp();
    KiRight = tuner.getKi();
    KdRight = tuner.getKd();
    savePIDParameters();
}

void Motors::savePIDParameters() {
    EEPROM.put(8, KpLeft);
    EEPROM.put(12, KiLeft);
    EEPROM.put(16, KdLeft);
    EEPROM.put(20, KpRight);
    EEPROM.put(24, KiRight);
    EEPROM.put(28, KdRight);
}

void Motors::moveForward() {
    setMotorTarget(forwardStep, forwardStep);
}

void Motors::moveBackwards() {
    setMotorTarget(-forwardStep, -forwardStep);
}

void Motors::turnLeft() {
    setMotorTarget(-turnStep, turnStep);
}

void Motors::turnRight() {
    setMotorTarget(turnStep, -turnStep);
}

void Motors::setMotorTarget(long leftTarget, long rightTarget) {
    targetLeft = leftTarget;
    targetRight = rightTarget;
    commandCompleted = false;
}

void Motors::stopMotors() {
    analogWrite(motorPins[0][0], 0);
    analogWrite(motorPins[0][1], 0);
    analogWrite(motorPins[1][0], 0);
    analogWrite(motorPins[1][1], 0);
    targetLeft = 0;
    targetRight = 0;
    saveCalibration();
    commandCompleted = true;
}

void Motors::updateMotorSpeedISR() {
    Motors::updateMotorSpeed();
}

void Motors::updateMotorSpeed() {
    stepLeft = knobLeft.read();
    stepRight = knobRight.read();
    calibrateMotors(stepLeft, stepRight);

    if (abs(targetLeft - stepLeft) < 10 && abs(targetRight - stepRight) < 10) {
        stopMotors();
        return;
    }

    float leftSpeed = computePID(targetLeft, stepLeft, integralLeft, lastErrorLeft) + leftCalib;
    float rightSpeed = computePID(targetRight, stepRight, integralRight, lastErrorRight) + rightCalib;

    analogWrite(motorPins[0][0], constrain(leftSpeed, minSpeed, maxSpeed));
    analogWrite(motorPins[1][0], constrain(rightSpeed, minSpeed, maxSpeed));
}

void Motors::calibrateMotors(long stepLeft, long stepRight) {
    float diff = stepLeft - stepRight;
    if (abs(diff) > 5) {
        if (diff > 0) {
            rightCalib += 1;
        } else {
            leftCalib += 1;
        }
    }
}

void Motors::saveCalibration() {
    EEPROM.put(0, leftCalib);
    EEPROM.put(4, rightCalib);
}

float Motors::computePID(float target, float current, float &integral, float &lastError) {
    float error = target - current;
    integral += error;
    float derivative = error - lastError;
    lastError = error;
    float output = (KpLeft * error) + (KiLeft * integral) + (KdLeft * derivative);
    return constrain(output, -maxSpeed, maxSpeed);
}

bool Motors::isCommandCompleted() {
    return commandCompleted;
}