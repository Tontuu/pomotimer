#include "include/pomotimer.h"

int update_timer(int *hours, int *minutes, int *seconds) {
  int elapsed_minutes = 0;
  (*seconds)++;

  if (*seconds == 60) {
    *seconds = 0;
    (*minutes)++;
    elapsed_minutes++;
  }

  if (*minutes == 60) {
    *minutes = 0;
    (*hours)++;
  }

  return elapsed_minutes;
}

void timer(char buf[1024], int hours, int minutes, int seconds, int total_hours,
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

  snprintf(buf, 33, "%02d:%02d:%02d", timer_hours, timer_minutes,
           timer_seconds);
}

void run_break(Mode mode, int break_minutes) {
  int hours = 0;
  int seconds = 0;
  int minutes = 0;

  while (1) {
    char timer_str[1024];
    timer(timer_str, hours, minutes, seconds, 0, break_minutes, 0);

    char buf[100];
    if (mode == SET) {
      char status = get_status_at_tempfile();

      if (status == 'P') {
        snprintf(buf, 1028, "%s (P)", timer_str);

        int success = write_into_tempfile(buf);
        if (!success)
          panic(ERRNO);

        sleep(1);
        continue;
      }

      if (status == 'S') {
        int success = write_into_tempfile(timer_str);
        if (!success)
          panic(ERRNO);

        break;
      }
    }

    snprintf(buf, 1028, "%s (B)", timer_str);

    if (mode == SET) {
      int success = write_into_tempfile(buf);
      if (!success)
        panic(ERRNO);
    }

    if (mode == SET_AND_INTERACTIVE) {
      system("clear");
      printf("%s (B)", timer_str);
      fflush(stdout);
    }

    sleep(1);

    update_timer(&hours, &minutes, &seconds);

    if (hours == 0 && minutes == break_minutes && seconds == 1)
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
  Time time_obj = {usr_hours, usr_minutes, usr_seconds, usr_break_time};
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  int total_elapsed_minutes = 0;

  int current_session = 1;
  int max_sessions = 4;


  if (mode == SET_AND_INTERACTIVE) {
    print_menu(usr_hours, usr_minutes, usr_seconds, usr_break_time);
    printf("Example: 0 25 0 10\n\n");
    printf("Input: ");
    scanf("%d%d%d%d", &usr_hours, &usr_minutes, &usr_seconds, &usr_break_time);
    int time_values[5] = {usr_hours, usr_minutes, usr_seconds, usr_break_time};
    time_obj = check_values(time_values);
  }

  do {
    char timer_str[1024];
        timer(timer_str, hours, minutes, seconds, time_obj.hours, time_obj.minutes, time_obj.seconds);

    if (mode == SET) {
      char status = get_status_at_tempfile();

      if (status == 'P') {
        char buf[100];
        snprintf(buf, 1028, "%s (P)", timer_str);

        int success = write_into_tempfile(buf);
        if (!success)
          panic(ERRNO);

        sleep(1);
        continue;
      }

      if (status == 'S') {
        if (current_session == max_sessions)
          break;

        int success = write_into_tempfile(timer_str);

        if (!success)
          panic(ERRNO);

        notify(NOTIFY_BREAK, time_obj);
        run_break(mode, time_obj.break_time);
        notify(NOTIFY_FINISH_SESSION, time_obj);

        reset(&hours, &minutes, &seconds);
        current_session++;
        continue;
      }
    }

    if (mode == SET) {
      char buf[100];
      snprintf(buf, 1040, "%s (%d/%d)", timer_str, current_session, max_sessions);

      if (mode == SET) {
        int success = write_into_tempfile(buf);
        if (!success)
          panic(ERRNO);
      }
    }

    if (mode == SET_AND_INTERACTIVE) {
      system("clear");
      printf("%s (%d/%d)", timer_str, current_session, max_sessions);
      fflush(stdout);
    }

    sleep(1);

    total_elapsed_minutes += update_timer(&hours, &minutes, &seconds);

    if (hours == time_obj.hours && minutes == time_obj.minutes &&
        seconds == time_obj.seconds + 1) {

      #if 0
      if (current_session != max_sessions) {
          notify(NOTIFY_BREAK, time_obj);
          run_break(mode, time_obj.break_time);
          notify(NOTIFY_FINISH_SESSION, time_obj);
      }
      #endif

      reset(&hours, &minutes, &seconds);
      current_session++;
    }
  } while (current_session <= max_sessions);

  if (mode == SET && remove_tempfile() == -1)
    panic(ERRNO);

  if (mode == SET_AND_INTERACTIVE) {
      system("clear");
      printf("Sucessfully finished timer\n");
  }

  char total_time[512];
  format_minutes_to_hours(total_time, total_elapsed_minutes);


  if (!add_to_db(total_time)) {
      panic(ERRNO);
  }

  fflush(stdout);

  notify(NOTIFY_FINISH_POMODORO, time_obj);

}

int pomodoro_timer_get() {
  char file_str[50];

  int success = read_from_tempfile(file_str);
  if (!success) {
    fprintf(stderr, "There's no pomodoro session running at the moment\n");
    return 1;
  }
  printf("%s", file_str);

  return 0;
}

int pomodoro_pause() {
  char file_str[50];
  int success = read_from_tempfile(file_str);
  if (!success) {
    fprintf(stderr, "There's no pomodoro session running at the moment\n");
    return 0;
  }

  int length = strlen(file_str);
  if (length > 13)
    file_str[length - 7] = '\0'; // In normal session `00:00:00 (1/4)`

  else
    file_str[length - 5] = '\0'; // In break session `00:00:00 (B)`

  strcat(file_str, " (P)");

  success = write_into_tempfile(file_str);
  if (!success)
    panic(ERRNO);

  return 1;
}

int pomodoro_unpause() {
  char file_str[50];
  int success = read_from_tempfile(file_str);

  if (!success) {
    fprintf(stderr, "There's no pomodoro session running at the moment\n");
    return 0;
  }

  int length = strlen(file_str);

  if (strchr(file_str, 'P') != NULL)
    file_str[length - 5] =
        '\0'; // Paused string looks like that: `00:00:00 (P)`
  else {
    fprintf(stderr, "There's no pomodoro paused session\n");
    return 0;
  }

  success = write_into_tempfile(file_str);
  if (!success)
    panic(ERRNO);

  return 1;
}

int pomodoro_skip() {
  char file_str[50];
  int success = read_from_tempfile(file_str);
  if (!success) {
    fprintf(stderr, "There's no pomodoro session running at the moment\n");
    return 0;
  }

  int length = strlen(file_str);
  if (length > 13)
    file_str[length - 7] = '\0'; // In normal session `00:00:00 (1/4)`

  else
    file_str[length - 5] = '\0'; // In break session `00:00:00 (B)`

  strcat(file_str, " (S)");

  success = write_into_tempfile(file_str);
  if (!success)
    panic(ERRNO);

  return 1;
}
