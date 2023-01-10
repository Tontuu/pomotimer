#include "include/pomotimer.h"

#define TMP_FILE "/tmp/tmp_pomotimer"

void update_timer(int *hours, int *minutes, int *seconds) {
  (*seconds)++;

  if (*seconds == 60) {
    *seconds = 0;
    (*minutes)++;
  }

  if (*minutes == 60) {
    *minutes = 0;
    (*hours)++;
  }
}

char *timer(int hours, int minutes, int seconds, int total_hours,
            int total_minutes, int total_seconds) {
  int timer_seconds = total_seconds - seconds;
  int timer_minutes = total_minutes - minutes;
  int timer_hours = total_hours - hours;

  if (timer_seconds <= -1) {
    timer_seconds = 60 + timer_seconds;
    timer_minutes--;
  }

  if (timer_minutes <= -1) {
    timer_minutes = 60 + timer_minutes;
    timer_hours--;
  }

  char *buf = (char *)malloc(33);
  snprintf(buf, 33, "%02d:%02d:%02d", timer_hours, timer_minutes,
           timer_seconds);

  return buf;
}

void panic() {
  fprintf(stderr, "ERROR: %s\n", strerror(errno));
  exit(1);
}

int write_into_tempfile(char *str) {
  FILE *fp = fopen(TMP_FILE, "w");

  if (fp != NULL) {
    fprintf(fp, "%s\n", str);
    fclose(fp);
    return 0;
  }

  return -1;
}

int read_from_tempfile(char *file_str) {
  FILE *fp = fopen(TMP_FILE, "r");

  if (fp != NULL) {
    if (!fgets(file_str, 50, fp))
      panic();
    fclose(fp);
    return 0;
  }

  return -1;
}

void pomodoro_timer(int usr_hours, int usr_minutes, int usr_seconds,
                    char filemode) {
  int hours = 0;
  int minutes = 0;
  int seconds = 0;

  while (1) {
    if (filemode == 'r') {
      char file_str[50];

      if (read_from_tempfile(file_str) == -1) {
        fprintf(stderr, "There's no pomodoro session running at the moment\n");
        exit(1);
      }

      printf("%s", file_str);
      exit(0);
    }

    char *timer_str =
        timer(hours, minutes, seconds, usr_hours, usr_minutes, usr_seconds);

    if (filemode == 'w')
      if (write_into_tempfile(timer_str) == -1)
        panic();

    system("clear");
    printf("%s", timer_str);
    fflush(stdout);
    free(timer_str);

    sleep(1);

    update_timer(&hours, &minutes, &seconds);

    if (hours == usr_hours && minutes == usr_minutes &&
        seconds == usr_seconds + 1) {
      system("clear");

      if (remove(TMP_FILE) == -1)
        panic();

      break;
    }
  }

  fflush(stdout);
}
