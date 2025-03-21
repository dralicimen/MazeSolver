#include <Arduino.h>
#include <main.h>
#include "includes/motors/motors.h"
#include "includes/sensors/sensors.h"
#include "includes/maze/maze.h"

Motors motorControl;
Sensors sensorControl;
Maze mazeControl;
int command = -1;

/**
 * @brief Rastgele bir yön seçimi yapar. Hücre değerlerine göre ağırlıklandırma uygulanır.
 * @return Komut kodu: 1 (ileri), 2 (sol), 3 (sağ), 4 (geri)
 */
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

/**
 * @brief Geçerli sensör verilerine göre bir hareket komutu hesaplar.
 * @return Komut kodu: 1 (ileri), 2 (sol), 3 (sağ), 4 (geri)
 */
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

/**
 * @brief Verilen komutu uygular ve robot durumunu günceller.
 * @param cmd Uygulanacak komut kodu
 */
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

    onMazeUpdated();
}

/**
 * @brief Robot konumunu ve labirent bilgisini günceller.
 */
void onMazeUpdated() {
    mazeControl.updatePosition();
    mazeControl.printMaze();
}

/**
 * @brief Hareket sırasında engel algılandığında çalışır, motorları durdurur.
 */
void onObstacleDetectedWhileMoving() {
    motorControl.stopMotors();
    Serial.println("ENGEL ALGILANDI! Hareket durduruldu.");
    command = -1;
}

/**
 * @brief Hareket tamamlandığında yeni komutu hesaplar ve uygular.
 */
void onCommandCompleted() {
    command = onNewCommandComputed();
    onUseCommand(command);
    command = -1;
}

/**
 * @brief Sistem başlatılırken çalıştırılan yapılandırma fonksiyonu.
 */
void setup() {
    Serial.begin(115200);
    motorControl.initialize();
    sensorControl.initialize();
    mazeControl.initialize();
    randomSeed(analogRead(0));
}

/**
 * @brief Ana döngü fonksiyonu. Sürekli sensör ve motor durumunu kontrol eder, olayları tetikler.
 */
void loop() {
    sensorControl.update();

    if (!motorControl.isCommandCompleted() && sensorControl.isObstacleFront() && command == 1) {
        onObstacleDetectedWhileMoving();
        return;
    }

    if (motorControl.isCommandCompleted()) {
        onCommandCompleted();
    }

    motorControl.updateMotorSpeed();
    delay(10);
}
