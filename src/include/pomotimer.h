#ifndef POMOTIMER_H
#define POMOTIMER_H

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "pomotimer_utils.h"

#include "../lib/args.h"

typedef enum {
  SET,
  SET_AND_INTERACTIVE,
} Mode;

void pomodoro_timer(int usr_hours, int usr_minutes, int usr_seconds, Mode mode);
int pomodoro_timer_get();

char* timer(int hours, int minutes, int seconds, int total_hours, int total_minutes, int total_seconds);
void update_timer(int *hours,  int *minutes,  int *seconds);

#endif
