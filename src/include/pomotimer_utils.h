#ifndef POMOTIMER_UTILS_H
#define POMOTIMER_UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <pwd.h>

#include "../lib/args.h"

#define TMP_FILE "/tmp/tmp_pomotimer"
#define DB_PATH ".local/share/pomotimer/data.csv"
#define SOUND_PATH ".local/share/pomotimer/assets/sound.wav"

typedef enum {
  ALREADY_RUNNING,
  EXIT,
  TOO_FEW_ARGUMENTS,
  TOO_MUCH_ARGUMENTS,
  INVALID_HOUR,
  INVALID_MINUTE,
  INVALID_SECONDS,
  INVALID_BREAK_TIME,
  UNKNOW_ARGUMENT,
  ERRNO,
  NO_PAPLAY,
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

typedef struct {
    int sessions;
    char date[100];
    char time[100];
} Cell;

void panic(Error error);

int format_minutes_to_hours(char buf[512], int minutes);
char get_status_at_tempfile();
int write_into_tempfile(char *str);
int read_from_tempfile(char *file_str);
int remove_tempfile();
Time parse_time_args(ArgParser *parser);
Time check_values(int *time_values);
void print_menu(int hours, int minutes, int seconds, int break_time);
void notify(NotificType notification, Time time);
int add_to_db(const char* total_time);
void sum_time(char *time1, char *time2, char buf[100]);
void sanatize_db(char *db_path);

#endif
