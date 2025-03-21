#ifndef MAZE_H
#define MAZE_H
#include "globals.h"
#include <Arduino.h>

class Maze {
public:

    struct Position {
        int x;
        int y;
    };

    struct RobotState {
        Position pos;
        int direction; // 0: Yukarı, 1: Sağ, 2: Aşağı, 3: Sol
    };

    Maze();

    void initialize();
    void updatePosition();
    void markDeadEnd();
    void printMaze();

    uint8_t getCellValue(int x, int y) const;
    void setCellValue(int x, int y, uint8_t value);

    Position getFront() const;
    Position getRight() const;
    Position getLeft() const;
    Position getBack() const;

    RobotState getRobotState() const;
    void setRobotState(const RobotState &state);

private:
    uint8_t maze[MAZE_WIDTH][MAZE_HEIGHT];
    RobotState robot;

    int gX(int dir) const;
    int gY(int dir) const;
    bool isValidPosition(Position pos) const;
};

#endif // MAZE_H
