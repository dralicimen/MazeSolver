#include "logic.h"

bool deadEnd = false;

void printMazeValues(int front, int right, int left) {
    Serial.println("--- Maze Cells ---");
    Serial.print("     [ "); Serial.print(front); Serial.println(" ]");
    Serial.print("[ "); Serial.print(left); Serial.print(" ]   [ "); Serial.print(right); Serial.println(" ]");
    Serial.println("------------------");
}

int computeNewCommand() {
    sensorControl.update();

    bool frontObstacle = sensorControl.isObstacleFront();
    bool leftObstacle = sensorControl.isObstacleLeft();
    bool rightObstacle = sensorControl.isObstacleRight();
    Serial.print(leftObstacle); Serial.print(" ");
    Serial.print(frontObstacle); Serial.print(" ");
    Serial.print(rightObstacle); Serial.print(" ");
    Serial.print(sensorControl.oG); Serial.print("\n");

    Maze::Position front = mazeControl.getFront();
    Maze::Position right = mazeControl.getRight();
    Maze::Position left = mazeControl.getLeft();

    int frontCell = frontObstacle ? 255 : mazeControl.getCellValue(front.x, front.y);
    int rightCell = rightObstacle ? 255 : mazeControl.getCellValue(right.x, right.y);
    int leftCell = leftObstacle ? 255 : mazeControl.getCellValue(left.x, left.y);

    Serial.println("--- Sensor States ---");
    Serial.print("Front Obstacle: "); Serial.println(frontObstacle);
    Serial.print("Left Obstacle: "); Serial.println(leftObstacle);
    Serial.print("Right Obstacle: "); Serial.println(rightObstacle);

    printMazeValues(frontCell, rightCell, leftCell);

    if (frontObstacle && leftObstacle && rightObstacle) {
        mazeControl.deadEnd = true;
        return 4;
    }

    int frontWeight = (frontCell < 255) ? (101 - frontCell) * (mazeControl.deadEnd ? 5 : 50) : 0;
    int rightWeight = (rightCell < 255) ? (101 - rightCell) * (mazeControl.deadEnd ? 50 : 15) : 0;
    int leftWeight = (leftCell < 255) ? (101 - leftCell) * (mazeControl.deadEnd ? 50 : 15) : 0;

    int totalWeight = frontWeight + rightWeight + leftWeight;

    Serial.print("Debug totalWeight: "); Serial.println(totalWeight);

    if (totalWeight == 0) {
        mazeControl.deadEnd = true;
        return 4;
    }

    int randValue = random(totalWeight);

    if (randValue < frontWeight && !frontObstacle) {
        mazeControl.deadEnd = false;
        return 1;
    }
    randValue -= frontWeight;
    if (randValue < rightWeight && !rightObstacle) {
        mazeControl.deadEnd = false;
        return 3;
    }
    randValue -= rightWeight;
    if (!leftObstacle) {
        mazeControl.deadEnd = false;
        return 2;
    }

    mazeControl.deadEnd = false;
    return 4;
}