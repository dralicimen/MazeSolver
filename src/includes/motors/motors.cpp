#include "globals.h"
#include "motors.h"
#include <EEPROM.h>
// #include <PID_v1.h> // Removed PID library, using custom PID
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
//const int Motors::forwardStep = 2264;
const int Motors::forwardStep = 100000;
const int Motors::turnStep = 742;

float Motors::leftCalib = 1;
float Motors::rightCalib = 1;
long Motors::targetLeft = 0;
long Motors::targetRight = 0;
long Motors::stepLeft = 0;
long Motors::stepRight = 0;
unsigned long Motors::startTimeLeft = 0;
unsigned long Motors::startTimeRight = 0;
int Motors::direction = 0;
bool Motors::commandCompleted = true;
bool Motors::commandLeftCompleted = false;
bool Motors::commandRightCompleted = false;

unsigned long Motors::lastSampleTimeLeft = 0;
unsigned long Motors::lastSampleTimeRight = 0;
long Motors::lastSampleStepLeft = 0;
long Motors::lastSampleStepRight = 0;

void Motors::initialize() {
    commandCompleted = false;
    commandLeftCompleted = false;
    commandRightCompleted = false;
    // pidLeft.SetMode(AUTOMATIC);
// pidRight.SetMode(AUTOMATIC);
// Removed due to custom PID implementation
    for (int i = 0; i < 2; i++) {
        pinMode(motorPins[i][0], OUTPUT);
        pinMode(motorPins[i][1], OUTPUT);
    }
    //Timer1.initialize(10);
    //Timer1.attachInterrupt(Motors::updateMotorSpeedISR);
    //EEPROM.get(0, leftCalib);
    //EEPROM.get(4, rightCalib);
    EEPROM.get(8, KpLeft);
    EEPROM.get(12, KiLeft);
    EEPROM.get(16, KdLeft);
    EEPROM.get(20, KpRight);
    EEPROM.get(24, KiRight);
    EEPROM.get(28, KdRight);
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
    Motors::maxSpeed =  Motors::defaultMaxSpeed;
    setMotorTarget(forwardStep, forwardStep);
}

void Motors::moveBackwards() {
    Motors::maxSpeed = Motors::turnSpeed;
    setMotorTarget(-forwardStep, -forwardStep);
}

void Motors::turnLeft() {
    Motors::maxSpeed = Motors::turnSpeed;
    setMotorTarget(-turnStep, turnStep);
}

void Motors::turnRight() {
    Motors::maxSpeed = Motors::turnSpeed;
    setMotorTarget(turnStep, -turnStep);
}

void Motors::turnBack() {
    setMotorTarget(turnStep*2, -turnStep*2);
}

void Motors::setMotorTarget(long leftTarget, long rightTarget) {
    lastSampleTimeLeft = millis();
    lastSampleTimeRight = millis();
    lastSampleStepLeft = 0;
    lastSampleStepRight = 0;
    startTimeLeft = millis();
    startTimeRight = millis();
    knobLeft.write(0);
    knobRight.write(0);
    stepLeft = 0;
    stepRight = 0;
    targetLeft = leftTarget;
    targetRight = rightTarget;
    commandCompleted = false;
    commandLeftCompleted = false;
    commandRightCompleted = false;
}

void Motors::stopMotors() {
    Motors::stopRight();
    Motors::stopLeft();
}

void Motors::stopLeft() {
    unsigned long durationLeft = millis() - startTimeLeft;

    analogWrite(motorPins[0][0], 0);
    analogWrite(motorPins[0][1], 0);
    knobLeft.write(0);
    stepLeft = 0;
    targetLeft = 0;
}
void Motors::stopRight() {
    unsigned long durationRight = millis() - startTimeRight;

    analogWrite(motorPins[1][0], 0);
    analogWrite(motorPins[1][1], 0);
    knobRight.write(0);
    stepRight = 0;
    targetRight = 0;
}
void Motors::updateMotorSpeedISR() {
    Motors::updateMotorSpeed();
}

void Motors::updateMotorSpeed() {
    stepLeft = -knobLeft.read();
    stepRight = knobRight.read();
    calibrateMotors(stepLeft, stepRight);

    bool leftDone = (targetLeft >= 0) ? (stepLeft >= targetLeft) : (stepLeft <= targetLeft);
    bool rightDone = (targetRight >= 0) ? (stepRight >= targetRight) : (stepRight <= targetRight);

    if (leftDone) {
        commandLeftCompleted=true;
        Motors::stopLeft();

    }else {
        float errorLeft = abs(targetLeft) - abs(stepLeft);
        integralLeft += errorLeft;
        float derivativeLeft = errorLeft - lastErrorLeft;
        lastErrorLeft = errorLeft;
        outputLeft = KpLeft * errorLeft + KiLeft * integralLeft + KdLeft * derivativeLeft;
        // Yöne göre motor pin kontrolü
        if (targetLeft >= 0) {
            analogWrite(motorPins[0][0], constrain(outputLeft * leftCalib, minSpeed, maxSpeed));
            analogWrite(motorPins[0][1], 0);
        } else {
            analogWrite(motorPins[0][0], 0);
            analogWrite(motorPins[0][1], constrain(outputLeft * leftCalib, minSpeed, maxSpeed));
        }
    }
    if (rightDone){
        commandRightCompleted=true;
        Motors::stopRight();
    }else{
        float errorRight = abs(targetRight) - abs(stepRight);
        integralRight += errorRight;
        float derivativeRight = errorRight - lastErrorRight;
        lastErrorRight = errorRight;
        outputRight = KpRight * errorRight + KiRight * integralRight + KdRight * derivativeRight;
        if (targetRight >= 0) {
            analogWrite(motorPins[1][0], constrain(outputRight *rightCalib, minSpeed, maxSpeed));
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
        unsigned long nowLeft = millis();
        unsigned long elapsedLeft = nowLeft - lastSampleTimeLeft;
        lastSampleTimeLeft = nowLeft;
        lastSampleStepLeft = stepLeft;

        unsigned long nowRight = millis();
        unsigned long elapsedRight = nowRight - lastSampleTimeRight;
        lastSampleTimeRight = nowRight;
        lastSampleStepRight = stepRight;

    }

    // Sağ motor için ayrı kalibrasyon kaldırıldı – artık karşılaştırmalı yapılmakta
}

void Motors::calibrateMotors(long stepLeft, long stepRight) {
    const float calibrationSensitivity = 0.005; // Hassasiyet, %0.5 gibi küçük bir adım

    if (stepLeft != 0 && stepRight != 0) {
        float ratio = (float)stepLeft / (float)stepRight;

        if (abs(1.0f - ratio) > 0.02) { // %2'den büyük fark varsa küçük ayarlama yap
            if (ratio > 1.0f) {
                // Sağ motor daha yavaş, hızını artır
                rightCalib *= (1.0f + calibrationSensitivity);
                leftCalib *= (1.0f - calibrationSensitivity);
            } else {
                // Sol motor daha yavaş, hızını artır
                leftCalib *= (1.0f + calibrationSensitivity);
                rightCalib *= (1.0f - calibrationSensitivity);
            }
        }
    }

    // Kalibrasyon değerlerinin mantıklı sınırlar içinde kalmasını sağla
    rightCalib = constrain(rightCalib, 0.8f, 1.2f);
    leftCalib = constrain(leftCalib, 0.8f, 1.2f);

    Serial.print("Left Calib: ");
    Serial.print(leftCalib);
    Serial.print(" StepLeft: ");
    Serial.print(stepLeft);
    Serial.print(" | Right Calib: ");
    Serial.print(rightCalib);
    Serial.print(" StepRight: ");
    Serial.println(stepRight);
}



void Motors::saveCalibration() {
    EEPROM.put(0, leftCalib);
    EEPROM.put(4, rightCalib);
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
