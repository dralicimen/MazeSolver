#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "includes/motors/motors.h"
#include "includes/sensors/sensors.h"
#include "includes/maze/maze.h"

// Global değişkenler
extern Motors motorControl;
extern Sensors sensorControl;
extern Maze mazeControl;

extern int command;
extern bool deadEnd;

// Ana fonksiyonlar
void setup();
void loop();

// Hareket ve labirent işleme fonksiyonları
void fillMaze();
int computeCommand();
void useCommand(int command);
void mazeSolved();
void debug();
void debug2();

#endif // MAIN_H
