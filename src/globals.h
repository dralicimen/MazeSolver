#ifndef GLOBALS_H
#define GLOBALS_H

#include <Encoder.h>

extern const int START_BUTTON;
// Motor pinleri
extern const int M1_IN1;
extern const int M1_IN2;
extern const int M2_IN1;
extern const int M2_IN2;
extern const int motorPins[2][2];

// --- Encoder Nesneleri ---
extern const int ENC_LEFT_A;
extern const int ENC_LEFT_B;
extern const int ENC_RIGHT_A;
extern const int ENC_RIGHT_B;

extern const int SENSOR_O1;
extern const int SENSOR_O2;
extern const int SENSOR_O3;
extern const int SENSOR_GROUND;
extern const int SENSOR_LINE;

// Maze boyutları
#define MAZE_WIDTH 10
#define MAZE_HEIGHT 10

#endif
