#ifndef POMOTIMER_UTILS_H
#define POMOTIMER_UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/args.h"

#define TMP_FILE "/tmp/tmp_pomotimer"

typedef enum {
  TOO_FEW_ARGUMENTS,
  TOO_MUCH_ARGUMENTS,
  ERRNO,
  INVALID_HOUR,
  INVALID_MINUTE,
  INVALID_SECONDS,
  INVALID_BREAK_TIME
} Error;

typedef enum {
  NOTIFY_BREAK,
  NOTIFY_FINISH_SESSION,
  NOTIFY_FINISH_POMODORO,
} NotificType;

typedef struct {
  int hours;
  int minutes;
  int seconds;
  int break_time;
} Time;

void panic(Error error);

int write_into_tempfile(char *str);
int read_from_tempfile(char *file_str);
int remove_tempfile();
Time parse_time_args(ArgParser *parser);
Time check_values(int *time_values);
void print_menu(int hours, int minutes, int seconds, int break_time);
void notify(NotificType notification, Time time);

#endif
