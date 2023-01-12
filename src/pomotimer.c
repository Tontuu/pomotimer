#include "include/pomotimer.h"

void print_menu(int hours, int minutes, int seconds, int break_time) {
  printf("----\n");
  printf("Pomotimer\n");
  printf("----\n\n");
  printf("Hours [%dh]\nminutes [%dm]\nSeconds [%ds]\nBreak-time "
         "[%dm]\n", hours, minutes, seconds, break_time);
}

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

void run_break(Mode mode, int break_minutes) {
  int hours = 0;
  int seconds = 0;
  int minutes = 0;

  while(1) {
    char *timer_str = timer(hours, minutes, seconds, 0, break_minutes, 0);
    char buf[100];
    snprintf(buf, 33, "%s (B)", timer_str);

    if (mode == SET)
      if (write_into_tempfile(buf) == -1)
	panic(ERRNO);

    if (mode == SET_AND_INTERACTIVE) {
      system("clear");
      printf("%s (B)", timer_str);
      fflush(stdout);
    }

    free(timer_str);
    sleep(1);

    update_timer(&hours, &minutes, &seconds);

    if (hours == 0 && minutes == break_minutes &&
        seconds == 1) 
      break;
  }
}

void reset(int *hours, int *minutes, int *seconds) {
  *hours = 0;
  *minutes = 0;
  *seconds = 0;
}

void pomodoro_timer(int usr_hours, int usr_minutes, int usr_seconds,
                    int usr_break_time, Mode mode) {
  Time time = { usr_hours, usr_minutes, usr_seconds, usr_break_time };
  int hours = 0;
  int minutes = 0;
  int seconds = 0;

  int current_session = 1;
  int max_sessions = 4;

  if (mode == SET_AND_INTERACTIVE) {
    print_menu(usr_hours, usr_minutes, usr_seconds, usr_break_time);
    printf("Example: 0 25 0 10\n\n");
    printf("Input: ");
    scanf("%d%d%d%d", &usr_hours, &usr_minutes, &usr_seconds, &usr_break_time);
    int time_values[5] = {usr_hours, usr_minutes, usr_seconds, usr_break_time};
    time = check_values(time_values);
  }

  do  {
    char *timer_str =
        timer(hours, minutes, seconds, time.hours, time.minutes, time.seconds);

    if (mode == SET) {
      char buf[100];
      snprintf(buf, 33, "%s (%d/%d)", timer_str, current_session, max_sessions);

      if (write_into_tempfile(buf) == -1)
	panic(ERRNO);
      
    }

    if (mode == SET_AND_INTERACTIVE) {
      system("clear");
      printf("%s (%d/%d)", timer_str, current_session, max_sessions);
      fflush(stdout);
    }

    free(timer_str);
    sleep(1);

    update_timer(&hours, &minutes, &seconds);

    if (hours == time.hours && minutes == time.minutes &&
        seconds == time.seconds + 1) {

      if (current_session == max_sessions)
	break;

      run_break(mode, time.break_time);

      reset(&hours, &minutes, &seconds);
      current_session++;
    }
  } while (current_session <= max_sessions);

  if (mode == SET && remove_tempfile() == -1)
    panic(ERRNO);

  if (mode == SET_AND_INTERACTIVE)
    printf("Sucessfully finished timer\n");

  fflush(stdout);
}

int pomodoro_timer_get() {
  char file_str[50];

  if (read_from_tempfile(file_str) == -1) {
    fprintf(stderr, "There's no pomodoro session running at the moment\n");
    return -1;
  }
  printf("%s", file_str);

  return 0;
}
