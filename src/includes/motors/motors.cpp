#include "globals.h"
#include "motors.h"
#include <EEPROM.h>
#include <TimerOne.h>

const int Motors::motorPins[2][2] = {{M1_IN1, M1_IN2}, {M2_IN1, M2_IN2}};
Encoder Motors::knobLeft(ENC_LEFT_A, ENC_LEFT_B);
Encoder Motors::knobRight(ENC_RIGHT_A, ENC_RIGHT_B);

float Motors::KpLeft = 2.5, Motors::KiLeft = 0.02, Motors::KdLeft = 0.1;
float Motors::KpRight = 2.5, Motors::KiRight = 0.02, Motors::KdRight = 0.1;
float Motors::outputLeft = 0, Motors::outputRight = 0;
float Motors::integralLeft = 0, Motors::lastErrorLeft = 0;
float Motors::integralRight = 0, Motors::lastErrorRight = 0;

int Motors::minSpeed = 50;
int Motors::maxSpeed = 150;
const int Motors::defaultMaxSpeed = 150;
const int Motors::turnSpeed = 100;
const int Motors::forwardStep = 2264;
const int Motors::turnStep = 742;

float Motors::leftCalib = 0.80;
float Motors::rightCalib = 1.20;
float Motors::leftCalibIntegral = 0, Motors::leftCalibLastError = 0;
float Motors::rightCalibIntegral = 0, Motors::rightCalibLastError = 0;
float Motors::KpCalib = 0.001, Motors::KiCalib = 0.00001, Motors::KdCalib = 0.0001;

long Motors::targetLeft = 0;
long Motors::targetRight = 0;
long Motors::stepLeft = 0;
long Motors::stepRight = 0;
unsigned long Motors::startTimeLeft = 0;
unsigned long Motors::startTimeRight = 0;
unsigned long Motors::lastUpdateTime = 0;
float Motors::integralMax = 300.0;
int Motors::direction = 0;
bool Motors::commandCompleted = true;
bool Motors::commandLeftCompleted = false;
bool Motors::commandRightCompleted = false;

unsigned long Motors::lastSampleTimeLeft = 0;
unsigned long Motors::lastSampleTimeRight = 0;
long Motors::lastSampleStepLeft = 0;
long Motors::lastSampleStepRight = 0;

void Motors::initialize() {
    // EEPROM'dan kalibrasyon ve PID parametrelerini al, yoksa öntanımlı kalsın
    float temp;
    EEPROM.get(0, temp);
    if (!isnan(temp) && temp >= 0.5f && temp <= 1.5f) leftCalib = temp;
    EEPROM.get(4, temp);
    if (!isnan(temp) && temp >= 0.5f && temp <= 1.5f) rightCalib = temp;

    EEPROM.get(8, temp);
    if (!isnan(temp)) KpLeft = temp;
    EEPROM.get(12, temp);
    if (!isnan(temp)) KiLeft = temp;
    EEPROM.get(16, temp);
    if (!isnan(temp)) KdLeft = temp;
    EEPROM.get(20, temp);
    if (!isnan(temp)) KpRight = temp;
    EEPROM.get(24, temp);
    if (!isnan(temp)) KiRight = temp;
    EEPROM.get(28, temp);
    if (!isnan(temp)) KdRight = temp;

    commandCompleted = commandLeftCompleted = commandRightCompleted = false;
    for (int i = 0; i < 2; i++) {
        pinMode(motorPins[i][0], OUTPUT);
        pinMode(motorPins[i][1], OUTPUT);
    }
    lastUpdateTime = millis();
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
    maxSpeed = defaultMaxSpeed;
    setMotorTarget(forwardStep, forwardStep);
}

void Motors::moveBackwards() {
    maxSpeed = turnSpeed;
    setMotorTarget(-forwardStep, -forwardStep);
}

void Motors::turnLeft() {
    maxSpeed = turnSpeed;
    setMotorTarget(-turnStep, turnStep);
}

void Motors::turnRight() {
    maxSpeed = turnSpeed;
    setMotorTarget(turnStep, -turnStep);
}

void Motors::turnBack() {
    setMotorTarget(turnStep * 2, -turnStep * 2);
}

void Motors::setMotorTarget(long leftTarget, long rightTarget) {
    lastSampleTimeLeft = lastSampleTimeRight = millis();
    lastSampleStepLeft = lastSampleStepRight = 0;
    startTimeLeft = startTimeRight = millis();
    knobLeft.write(0);
    knobRight.write(0);
    stepLeft = stepRight = 0;
    targetLeft = leftTarget;
    targetRight = rightTarget;
    commandCompleted = commandLeftCompleted = commandRightCompleted = false;
}

void Motors::stopMotors() {
    stopLeft();
    stopRight();
}

void Motors::stopLeft() {
    analogWrite(motorPins[0][0], 0);
    analogWrite(motorPins[0][1], 0);
    knobLeft.write(0);
    stepLeft = targetLeft = 0;
}

void Motors::stopRight() {
    analogWrite(motorPins[1][0], 0);
    analogWrite(motorPins[1][1], 0);
    knobRight.write(0);
    stepRight = targetRight = 0;
}

void Motors::updateMotorSpeedISR() {
    updateMotorSpeed();
}

void Motors::updateMotorSpeed() {
    stepLeft = knobLeft.read();
    stepRight = -knobRight.read();

    calibrateMotors(stepLeft, stepRight);

    unsigned long now = millis();
    float dt = (now - lastUpdateTime) / 1000.0;
    lastUpdateTime = now;
    if (dt <= 0) dt = 0.001;

    bool leftDone = (targetLeft >= 0) ? (stepLeft >= targetLeft) : (stepLeft <= targetLeft);
    bool rightDone = (targetRight >= 0) ? (stepRight >= targetRight) : (stepRight <= targetRight);

    if (leftDone) {
        commandLeftCompleted = true;
        stopLeft();
    } else {
        float errorLeft = abs(targetLeft) - abs(stepLeft);
        integralLeft += errorLeft * dt;
        integralLeft = constrain(integralLeft, -integralMax, integralMax);
        float derivativeLeft = (errorLeft - lastErrorLeft) / dt;
        lastErrorLeft = errorLeft;
        outputLeft = KpLeft * errorLeft + KiLeft * integralLeft + KdLeft * derivativeLeft;
        if (targetLeft >= 0) {
            analogWrite(motorPins[0][0], constrain(outputLeft * leftCalib, minSpeed, maxSpeed));
            analogWrite(motorPins[0][1], 0);
        } else {
            analogWrite(motorPins[0][0], 0);
            analogWrite(motorPins[0][1], constrain(outputLeft * leftCalib, minSpeed, maxSpeed));
        }
    }

    if (rightDone) {
        commandRightCompleted = true;
        stopRight();
    } else {
        float errorRight = abs(targetRight) - abs(stepRight);
        integralRight += errorRight * dt;
        integralRight = constrain(integralRight, -integralMax, integralMax);
        float derivativeRight = (errorRight - lastErrorRight) / dt;
        lastErrorRight = errorRight;
        outputRight = KpRight * errorRight + KiRight * integralRight + KdRight * derivativeRight;
        if (targetRight >= 0) {
            analogWrite(motorPins[1][0], constrain(outputRight * rightCalib, minSpeed, maxSpeed));
            analogWrite(motorPins[1][1], 0);
        } else {
            analogWrite(motorPins[1][0], 0);
            analogWrite(motorPins[1][1], constrain(outputRight * rightCalib, minSpeed, maxSpeed));
        }
    }

    if (leftDone && rightDone) {
        commandCompleted = true;
        stopMotors();
        saveCalibration();
    }

    if (abs(stepLeft - lastSampleStepLeft) >= 10 && abs(stepRight - lastSampleStepRight) >= 10) {
        lastSampleTimeLeft = now;
        lastSampleStepLeft = stepLeft;
        lastSampleTimeRight = now;
        lastSampleStepRight = stepRight;
    }
}

void Motors::calibrateMotors(long stepLeft, long stepRight) {
    if (stepLeft != 0 && stepRight != 0) {
        unsigned long now = millis();
        float dt = (now - lastUpdateTime) / 1000.0;
        if (dt <= 0) dt = 0.001;

        float ratio = (float)stepLeft / (float)stepRight;
        float error = 1.0f - ratio;

        leftCalibIntegral += error * dt;
        float derivative = (error - leftCalibLastError) / dt;
        leftCalibLastError = error;

        float output = KpCalib * error + KiCalib * leftCalibIntegral + KdCalib * derivative;

        leftCalib = constrain(leftCalib + output, 0.8f, 1.2f);
        rightCalib = constrain(rightCalib - output, 0.8f, 1.2f);
    }

    Serial.print("Left Calib: "); Serial.print(leftCalib);
    Serial.print(" StepLeft: "); Serial.print(stepLeft);
    Serial.print(" targetLeft: "); Serial.print(targetLeft);
    Serial.print(" | Right Calib: "); Serial.print(rightCalib);
    Serial.print(" StepRight: "); Serial.print(stepRight);
    Serial.print(" targetRight: "); Serial.println(targetRight);
}

void Motors::saveCalibration() {
    EEPROM.put(0, leftCalib);
    EEPROM.put(4, rightCalib);
}

void Motors::sencron() {
    maxSpeed = 255;
    setMotorTarget(100000, 100000);
}

bool Motors::isCommandCompleted() {
    return commandCompleted;
}

long Motors::getStepLeft() {
    stepLeft = -knobLeft.read();
    return stepLeft;
}

long Motors::getStepRight() {
    stepRight = knobRight.read();
    return stepRight;
}
