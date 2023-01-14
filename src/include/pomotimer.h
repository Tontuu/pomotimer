#ifndef POMOTIMER_H
#define POMOTIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "pomotimer_utils.h"

#include "../lib/args.h"

typedef enum {
  SET,
  SET_AND_INTERACTIVE,
} Mode;

int pomodoro_skip();

int pomodoro_pause();

int pomodoro_unpause();

void pomodoro_timer(int usr_hours, int usr_minutes, int usr_seconds,
                    int usr_break_time, Mode mode);
int pomodoro_timer_get();

char *timer(int hours, int minutes, int seconds, int total_hours,
            int total_minutes, int total_seconds);

void update_timer(int *hours, int *minutes, int *seconds);

void run_break(Mode mode, int break_minutes);

void print_menu(int hours, int minutes, int seconds, int break_time);

void reset(int *hours, int *minutes, int *seconds);


#endif
