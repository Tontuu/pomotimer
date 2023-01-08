#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

//TODO: create a basic function that runs a clock from the given time to 0;
void update_timer(int *hours,  int *minutes,  int *seconds) {
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

void print_timer(int hours, int minutes, int seconds, int total_hours, int total_minutes, int total_seconds) {
  int print_seconds = total_seconds - seconds;
  int print_minutes = total_minutes - minutes;
  int print_hours = total_hours - hours;

  if (print_seconds <= -1) {
    print_seconds = 60 + print_seconds;
    print_minutes--;
  }

  if (print_minutes <= -1) {
    print_minutes = 60 + print_minutes;
    print_hours--;
  }

  printf("total time: %02d:%02d:%02d\ntimer %02d:%02d:%02d\n", total_hours, total_minutes, total_seconds, print_hours, print_minutes, print_seconds);
  printf("vars: %02d:%02d:%02d\n", hours, minutes, seconds);
}

void pomodoro_timer(int usr_hours, int usr_minutes, int usr_seconds) {
   int hours = 0;
   int minutes = 59;
   int seconds = 59;

  while(1) {
    system("clear");

    print_timer(hours, minutes, seconds, usr_hours, usr_minutes, usr_seconds);

    fflush(stdout);
    sleep(1);

    update_timer(&hours, &minutes, &seconds);

    if (hours == usr_hours && minutes == usr_minutes && seconds == usr_seconds+1) {
      system("clear");
      break;
    }
  }

  printf("Sucessfully finished timer.\n");
  fflush(stdout);
}

int main() {
  pomodoro_timer(2, 0, 5);

  exit(0);
}
