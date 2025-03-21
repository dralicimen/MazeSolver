#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Wire.h>
#include <Encoder.h>
#include <EEPROM.h>

// Motor tanımları
extern const int motors[2][2];

// Sensör tanımları
extern const int sO1;
extern const int sO2;
extern const int sO3;
extern const int sG;

// Kalibrasyon değişkenleri
extern int leftCalib;
extern int rightCalib;

// Hareket ile ilgili değişkenler
extern int direction;
extern long togoLeft;
extern long stepLeft;
extern long togoRight;
extern long stepRight;
extern bool oL, oR, oF, oG, deadEnd;
extern bool routeRight, routeLeft, forward;

// Labirent ile ilgili değişkenler
extern const uint8_t w;
extern const uint8_t h;
extern uint8_t maze[][33];
extern uint8_t posX;
extern uint8_t posY;
extern int command;

// PID değişkenleri
extern float Kp;
extern float Ki;
extern float Kd;
extern float integralLeft, lastErrorLeft;
extern float integralRight, lastErrorRight;

// Fonksiyon Prototipleri
void loadMotorCalibration();
void saveMotorCalibration();
void initializeMotors();
void initializeSensors();
void fillMaze();
void stopLeft();
void stopRight();
void stopMotors();
void checkSensors();
int dx(int dir);
int dy(int dir);
int randomSelectCommand();
int computeCommand();
void useCommand(int command);
void setMotorSpeed(int motor, float speed);
float computePID(float target, float current, float &integral, float &lastError);
void updateMotors();
void calibrateMotors();
void moveForward();
void moveBackwards();
void turnLeft();
void turnRight();
void turnBackwards();
void mazeSolved();
void writeMaze();
void debug();
void debug2();
void waitForEnter();

#endif
