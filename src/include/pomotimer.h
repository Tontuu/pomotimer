#ifndef POMOTIMER_H
#define POMOTIMER_H

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

void pomodoro_timer(int usr_hours, int usr_minutes, int usr_seconds);
char* timer(int hours, int minutes, int seconds, int total_hours, int total_minutes, int total_seconds);
void update_timer(int *hours,  int *minutes,  int *seconds);

#endif
