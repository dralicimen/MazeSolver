#include "globals.h"
#include "main.h"

// Global nesnelerin tanımı
Motors motorControl;
Sensors sensorControl;
Maze mazeControl;
int command = -1;
unsigned long commandStartTime = 0;

// Motor pinleri
const int M1_IN1 = 6;
const int M1_IN2 = 5;
const int M2_IN1 = 9;
const int M2_IN2 = 10;

// --- Encoder Pinleri ---
const int ENC_LEFT_A = 2;
const int ENC_LEFT_B = 7;
const int ENC_RIGHT_A = 3;
const int ENC_RIGHT_B = 4;

// --- Engel ve çizgi sensör pinleri ---
const int SENSOR_L = 8;
const int SENSOR_F = 11;
const int SENSOR_R = 12;
const int SENSOR_GROUND = A0;
const int SENSOR_LINE = A0;
const bool debug = true;
const int START_BUTTON = A1;

