#include "maze.h"
#include "globals.h"
#include <Arduino.h>

Maze::Maze() {

    initialize();
}


// Labirenti başlat
void Maze::initialize() {

    for (int i = 0; i < MAZE_WIDTH; i++) {
        for (int j = 0; j < MAZE_HEIGHT; j++) {
            maze[i][j] = 100; // Varsayılan hücre değeri
        }
    }
    // Robotu başlangıç konumuna yerleştir
    robot.pos.x = MAZE_WIDTH / 2;
    robot.pos.y = MAZE_HEIGHT / 2;
    robot.direction = 0; // Yukarı bakıyor
    maze[robot.pos.x][robot.pos.y]--;
    Maze::deadEnd = false;
}

// Robotun yeni pozisyonunu güncelle
void Maze::updatePosition() {
    Position newPos = getFront();

    if (isValidPosition(newPos)) {
        robot.pos = newPos;
        maze[robot.pos.x][robot.pos.y]--;
    }
}

// Çıkmaz sokakları işaretle
void Maze::markDeadEnd() {
    maze[robot.pos.x][robot.pos.y] = 0;
}

// Labirenti seri porta yazdır
void Maze::printMaze() {
    Serial.println("----- LABİRENT DURUMU -----");
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (x == robot.pos.x && y == robot.pos.y) {
                Serial.print("[");
                Serial.print(robot.direction);
                Serial.print("]");
            } else if (maze[x][y] == 0) {
                Serial.print(" X ");
            } else {
                Serial.print(maze[x][y]);
                Serial.print(" ");
            }
        }
        Serial.println();
    }
    Serial.println("--------------------------");
}

// Belirli bir hücrenin değerini döndür
uint8_t Maze::getCellValue(int x, int y) const {
    return isValidPosition({x, y}) ? maze[x][y] : 255;
}

// Belirli bir hücreye değer ata
void Maze::setCellValue(int x, int y, uint8_t value) {
    if (isValidPosition({x, y})) {
        maze[x][y] = value;
    }
}

// Yön doğrultusunda x koordinat değişimi
int Maze::gX(int dir) const {
    static const int dx[4] = {0, 1, 0, -1}; // 0: Yukarı, 1: Sağ, 2: Aşağı, 3: Sol
    return dx[dir % 4];
}

// Yön doğrultusunda y koordinat değişimi
int Maze::gY(int dir) const {
    static const int dy[4] = {-1, 0, 1, 0}; // 0: Yukarı, 1: Sağ, 2: Aşağı, 3: Sol
    return dy[dir % 4];
}

// Ön taraftaki hücrenin konumu
Maze::Position Maze::getFront() const {
    return {robot.pos.x + gX(robot.direction), robot.pos.y + gY(robot.direction)};
}

// Sağ taraftaki hücrenin konumu
Maze::Position Maze::getRight() const {
    return {robot.pos.x + gX((robot.direction + 1) % 4), robot.pos.y + gY((robot.direction + 1) % 4)};
}

// Sol taraftaki hücrenin konumu
Maze::Position Maze::getLeft() const {
    return {robot.pos.x + gX((robot.direction + 3) % 4), robot.pos.y + gY((robot.direction + 3) % 4)};
}

// Arka taraftaki hücrenin konumu
Maze::Position Maze::getBack() const {
    return {robot.pos.x + gX((robot.direction + 2) % 4), robot.pos.y + gY((robot.direction + 2) % 4)};
}

// Robotun mevcut durumunu döndür
Maze::RobotState Maze::getRobotState() const {
    return robot;
}

// Robotun mevcut durumunu ayarla
void Maze::setRobotState(const RobotState &state) {
    robot = state;
}

// Pozisyonun geçerli olup olmadığını kontrol et
bool Maze::isValidPosition(Position pos) const {
    return (pos.x >= 0 && pos.x < MAZE_WIDTH && pos.y >= 0 && pos.y < MAZE_HEIGHT);
}

