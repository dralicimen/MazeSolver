#include "events.h"
#include "logic.h"

void onCommandCompleted() {
    command = computeNewCommand();
    Serial.println(command);
    useCommand(command);

    command = -1;

}

void waitForButton() {
    while (Serial.available() < 1)
    {


        delay(100);
    }
    while (Serial.available() > 0)
    {
        byte dummyread = Serial.read();
    }


}
void onObstacleDetectedWhileMoving() {
    motorControl.stopMotors();
    Serial.println("ENGEL ALGILANDI! Hareket durduruldu.");
    command = -1;
}

void useCommand(int cmd) {
    Serial.println(cmd);
    switch (cmd) {
        case 1:
            motorControl.moveForward();
            updateMaze();
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
            motorControl.turnBack();
        mazeControl.setRobotState({mazeControl.getBack(), (mazeControl.getRobotState().direction + 2) % 4});
        break;
        default:
            motorControl.stopMotors();
        break;
    }
    commandStartTime = millis();

}

void updateMaze() {
    mazeControl.updatePosition();
    if (mazeControl.deadEnd==true) {
        mazeControl.markDeadEnd();
    }
    //mazeControl.printMaze();
}
