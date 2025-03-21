#ifndef EVENTS_H
#define EVENTS_H

#include "main.h"

void onCommandCompleted();
void onObstacleDetectedWhileMoving();
void onUseCommand(int cmd);
void onMazeUpdated();

#endif
