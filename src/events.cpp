#include "events.h"
#include "logic.h"

void onCommandCompleted() {
    command = onNewCommandComputed();
    onUseCommand(command);
    command = -1;
}

void onObstacleDetectedWhileMoving() {
    motorControl.stopMotors();
    Serial.println("ENGEL ALGILANDI! Hareket durduruldu.");
    command = -1;
}

void onUseCommand(int cmd) {
    switch (cmd) {
        case 1:
            motorControl.moveForward();
        break;
        case 2:
            motorControl.turnLeft();
        mazeControl.setRobotState({mazeControl.getLeft(), (mazeControl.getRobotState().direction + 3) % 4});
        break;
        case 3:
            motorControl.turnRight();
        mazeControl.setRobotState({mazeControl.getRight(), (mazeControl.getRobotState().direction + 1) % 4});
        break;
        case 4:
            motorControl.moveBackwards();
        mazeControl.setRobotState({mazeControl.getBack(), (mazeControl.getRobotState().direction + 2) % 4});
        break;
        default:
            motorControl.stopMotors();
        break;
    }
    commandStartTime = millis();
    onMazeUpdated();
}

void onMazeUpdated() {
    mazeControl.updatePosition();
    mazeControl.printMaze();
}
