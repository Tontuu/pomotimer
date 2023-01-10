#ifndef POMOTIMER_H
#define POMOTIMER_H

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void panic();

int write_into_tempfile(char* str);
int read_from_tempfile(char* file_str);

void pomodoro_timer(int usr_hours, int usr_minutes, int usr_seconds, char mode);
char* timer(int hours, int minutes, int seconds, int total_hours, int total_minutes, int total_seconds);
void update_timer(int *hours,  int *minutes,  int *seconds);

#endif
