#include <Arduino.h>
#include <main.h>
#include "includes/motors/motors.h"
#include "includes/sensors/sensors.h"
#include "includes/maze/maze.h"

Motors motorControl;
Sensors sensorControl;
Maze mazeControl;
int command = -1;

// Rastgele komut belirleme
int randomSelectCommand() {
    Maze::Position front = mazeControl.getFront();
    Maze::Position right = mazeControl.getRight();
    Maze::Position left = mazeControl.getLeft();

    int frontCell = !sensorControl.isObstacleFront() ? (101 - mazeControl.getCellValue(front.x, front.y)) : 0;
    int rightCell = !sensorControl.isObstacleRight() ? (101 - mazeControl.getCellValue(right.x, right.y)) : 0;
    int leftCell = !sensorControl.isObstacleLeft() ? (101 - mazeControl.getCellValue(left.x, left.y)) : 0;

    int frontWeight = (frontCell > 0) ? (101 - frontCell) * 50 : 0;
    int rightWeight = (rightCell > 0) ? (101 - rightCell) * 15 : 0;
    int leftWeight = (leftCell > 0) ? (101 - leftCell) * 15 : 0;

    int totalWeight = frontWeight + rightWeight + leftWeight;
    int randValue = random(0, totalWeight);

    if (randValue < frontWeight) return 1;
    else if (randValue < frontWeight + rightWeight) return 3;
    else return 2;
}

// Hareket karar mekanizması
int computeCommand() {
    if (command != -1) return false;
    sensorControl.update();

    if (sensorControl.isObstacleFront()) {
        if (!sensorControl.isObstacleLeft() && sensorControl.isObstacleRight()) return 1;
        else if (sensorControl.isObstacleLeft() && !sensorControl.isObstacleRight()) return 3;
        else if (sensorControl.isObstacleLeft() && sensorControl.isObstacleRight()) return 4;
        else return randomSelectCommand();
    } else {
        return randomSelectCommand();
    }
}

// Hareketi uygula
void useCommand(int command) {
    switch (command) {
        case 1: motorControl.moveForward();
            break;
        case 2: motorControl.turnLeft();
            break;
        case 3: motorControl.turnRight();
            break;
        case 4: motorControl.moveBackwards();
            break;
        default: motorControl.stopMotors();
            break;
    }
    mazeControl.updatePosition();
}

// **SETUP FONKSİYONU**
void setup() {
    Serial.begin(115200);
    motorControl.initialize();
    sensorControl.initialize();
    mazeControl.initialize();
}

// **LOOP FONKSİYONU**
void loop() {
    sensorControl.update();
    int cmd = computeCommand();
    if (cmd != command) {
        command = cmd;
        useCommand(command);
    }
    motorControl.updateMotorSpeed();
    Serial.println(command);
}