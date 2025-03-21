#include "logic.h"

int randomSelectCommand() {
    Maze::Position front = mazeControl.getFront();
    Maze::Position right = mazeControl.getRight();
    Maze::Position left = mazeControl.getLeft();

    int frontCell = !sensorControl.isObstacleFront() ? mazeControl.getCellValue(front.x, front.y) : 255;
    int rightCell = !sensorControl.isObstacleRight() ? mazeControl.getCellValue(right.x, right.y) : 255;
    int leftCell = !sensorControl.isObstacleLeft() ? mazeControl.getCellValue(left.x, left.y) : 255;

    int frontWeight = (frontCell < 255) ? (100 - frontCell) * 50 : 0;
    int rightWeight = (rightCell < 255) ? (100 - rightCell) * 15 : 0;
    int leftWeight = (leftCell < 255) ? (100 - leftCell) * 15 : 0;

    int totalWeight = frontWeight + rightWeight + leftWeight;
    if (totalWeight == 0) return 4;

    int randValue = random(0, totalWeight);
    if (randValue < frontWeight) return 1;
    else if (randValue < frontWeight + rightWeight) return 3;
    else return 2;
}

int onNewCommandComputed() {
    sensorControl.update();

    if (sensorControl.isObstacleFront()) {
        if (!sensorControl.isObstacleLeft() && sensorControl.isObstacleRight()) return 2;
        else if (sensorControl.isObstacleLeft() && !sensorControl.isObstacleRight()) return 3;
        else if (!sensorControl.isObstacleLeft() && !sensorControl.isObstacleRight()) return randomSelectCommand();
        else return 4;
    } else {
        return randomSelectCommand();
    }
}
