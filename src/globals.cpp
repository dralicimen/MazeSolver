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
const int M2_IN1 = 10;
const int M2_IN2 = 9;

// --- Encoder Pinleri ---
const int ENC_LEFT_A = 3;
const int ENC_LEFT_B = 7;
const int ENC_RIGHT_A = 2;
const int ENC_RIGHT_B = 4;

// --- Engel ve çizgi sensör pinleri ---
const int SENSOR_O1 = 12;
const int SENSOR_O2 = 11;
const int SENSOR_O3 = 8;
const int SENSOR_GROUND = 13;
const int SENSOR_LINE = A0;

const int START_BUTTON = A1;

