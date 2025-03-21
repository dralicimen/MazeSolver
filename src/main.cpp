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

    int frontCell = !sensorControl.isObstacleFront() ? mazeControl.getCellValue(front.x, front.y) : 255;
    int rightCell = !sensorControl.isObstacleRight() ? mazeControl.getCellValue(right.x, right.y) : 255;
    int leftCell = !sensorControl.isObstacleLeft() ? mazeControl.getCellValue(left.x, left.y) : 255;

    int frontWeight = (frontCell < 255) ? (100 - frontCell) * 50 : 0;
    int rightWeight = (rightCell < 255) ? (100 - rightCell) * 15 : 0;
    int leftWeight = (leftCell < 255) ? (100 - leftCell) * 15 : 0;

    int totalWeight = frontWeight + rightWeight + leftWeight;
    if (totalWeight == 0) return 4; // hiçbir yön yoksa geri dön

    int randValue = random(0, totalWeight);

    if (randValue < frontWeight) return 1; // ileri
    else if (randValue < frontWeight + rightWeight) return 3; // sağ
    else return 2; // sol
}

// Hareket karar mekanizması
int computeCommand() {
    sensorControl.update();

    if (sensorControl.isObstacleFront()) {
        if (!sensorControl.isObstacleLeft() && sensorControl.isObstacleRight()) return 2; // sola dön
        else if (sensorControl.isObstacleLeft() && !sensorControl.isObstacleRight()) return 3; // sağa dön
        else if (!sensorControl.isObstacleLeft() && !sensorControl.isObstacleRight()) return randomSelectCommand(); // iki yön de açık
        else return 4; // çıkmaz, geri dön
    } else {
        return randomSelectCommand();
    }
}

// Hareketi uygula
void useCommand(int command) {
    switch (command) {
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
    mazeControl.updatePosition();
    mazeControl.printMaze(); // Durumu yazdır
}

// **SETUP FONKSİYONU**
void setup() {
    Serial.begin(115200);
    motorControl.initialize();
    sensorControl.initialize();
    mazeControl.initialize();
    randomSeed(analogRead(0)); // Rastgelelik için
}

// **LOOP FONKSİYONU**
void loop() {
    sensorControl.update();

    // Eğer ileri gidiyorsa ve önde engel algılanırsa dur
    if (!motorControl.isCommandCompleted() && sensorControl.isObstacleFront() && command == 1) {
        motorControl.stopMotors();     // hareketi durdur
        Serial.println("ENGEL ALGILANDI! Duruldu.");
        command = -1;                  // komutu sıfırla
        return;                        // bu döngüyü sonlandır
    }

    // Eğer hareket tamamlandıysa yeni komut hesapla
    if (motorControl.isCommandCompleted()) {
        int cmd = computeCommand();
        command = cmd;
        useCommand(command);
        command = -1;
    }

    motorControl.updateMotorSpeed();
    delay(10); // çok hızlı çalışmasın
}

