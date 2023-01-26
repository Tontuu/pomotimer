#include "include/pomotimer_utils.h"


void panic(Error error) {
  char error_str[100];
  if (error == ERRNO) strcpy(error_str, strerror(errno));

  if (error == ALREADY_RUNNING)
      strcpy(error_str, "Pomotimer session is already running on this machine.");

  if (error == EXIT)
      strcpy(error_str, "Could not exit from pomotimer session. Check if there's a session running.");

  if (error == TOO_FEW_ARGUMENTS)
    strcpy(error_str, "Too few arguments given");

  if (error == TOO_MUCH_ARGUMENTS)
    strcpy(error_str, "Too much arguments given");

  if (error == INVALID_HOUR)
    strcpy(error_str, "Unable to accept hours argument");

  if (error == INVALID_MINUTE)
    strcpy(error_str, "Unable to accept minutes argument");

  if (error == INVALID_SECONDS)
    strcpy(error_str, "Unable to accept seconds argument");

  if (error == INVALID_BREAK_TIME)
    strcpy(error_str, "Unable to accept break time argument");

  if (error == UNKNOW_ARGUMENT)
    strcpy(error_str, "Unable to recognize argument");

  if (error == NO_PAPLAY)
    strcpy(error_str, "Unable to play notification sound. Paplay/notify-send "
                      "binary was not found.");

  fprintf(stderr, "ERROR: %s\n", error_str);
  exit(1);
}

int write_into_tempfile(char *str) {
  FILE *fp = fopen(TMP_FILE, "w");

  if (fp != NULL) {
    fprintf(fp, "%s\n", str);
    fclose(fp);
    return 1;
  }

  return 0;
}

int read_from_tempfile(char *file_str) {
  FILE *fp = fopen(TMP_FILE, "r");

  if (fp != NULL) {
    if (!fgets(file_str, 50, fp))
      panic(ERRNO);
    fclose(fp);
    return 1;
  }

  return 0;
}

char get_status_at_tempfile() {
  char file_str[50];
  char status;

  FILE *fp = fopen(TMP_FILE, "r");

  if (fp != NULL) {
    if (!fgets(file_str, 50, fp))
      panic(ERRNO);

    if (strchr(file_str, 'P') != NULL)
      status = 'P';

    if (strchr(file_str, 'S') != NULL)
      status = 'S';

    fclose(fp);

    return status;
  }

  return 0;
}

int remove_tempfile() { return remove(TMP_FILE); }

int check_time(int time_value) {
  if (time_value > 60 || time_value < 0) {
    return -1;
  }

  return time_value;
}

Time parse_time_args(ArgParser *parser) {
  if (ap_count_args(parser) < 4)
    panic(TOO_FEW_ARGUMENTS);

  if (ap_count_args(parser) > 4)
    panic(TOO_MUCH_ARGUMENTS);

  int *time_values = ap_args_as_ints(parser);

  if (ap_count_args(parser) > 4)
    panic(TOO_MUCH_ARGUMENTS);

  if (ap_count_args(parser) < 4)
    panic(TOO_FEW_ARGUMENTS);

  Time time = check_values(time_values);
  free(time_values);
  return time;
}

Time check_values(int *time_values) {
  Time time;

  if (time_values[0] > 12 || time_values[0] < 0)
    panic(INVALID_HOUR);

  if (time_values[1] > 60 || time_values[1] < 0)
    panic(INVALID_MINUTE);

  if (time_values[2] > 60 || time_values[2] < 0)
    panic(INVALID_SECONDS);

  if (time_values[3] > 60 || time_values[2] < 0)
    panic(INVALID_BREAK_TIME);

  time.hours = time_values[0];
  time.minutes = time_values[1];
  time.seconds = time_values[2];
  time.break_time = time_values[3];

  return time;
}

void print_menu(int hours, int minutes, int seconds, int break_time) {
  printf("----\n");
  printf("Pomotimer\n");
  printf("----\n\n");
  printf("Hours [%dh]\nminutes [%dm]\nSeconds [%ds]\nBreak-time "
         "[%dm]\n",
         hours, minutes, seconds, break_time);
}

void notify(NotificType notification, Time time) {
  struct passwd *pw = getpwuid(getuid());
  char *homedir = pw->pw_dir;

  char sound_path[512];
  snprintf(sound_path, 512, "%s/%s", homedir, SOUND_PATH);

  char msg[100];

  if (notification == NOTIFY_BREAK) {
    char buf[100];
    snprintf(buf, 100, "Focus session is complete, chilling for %dm.\n",
             time.break_time);
    strcpy(msg, buf);
  }

  if (notification == NOTIFY_FINISH_SESSION) {
    char buf[100];
    snprintf(buf, 100, "Break of %dm is down\n", time.break_time);
    strcpy(msg, buf);
  }

  if (notification == NOTIFY_FINISH_POMODORO)
    strcpy(msg, "Pomodoro session is finished\n");

  char command[1024];
  snprintf(command, 1024,
           "paplay --volume=32768 %s >/dev/null 2>&1 && notify-send 'Pomotimer' '\n%s'",
           sound_path, msg);

  int status_code = system(command);

  if (status_code == 127)
    panic(NO_PAPLAY);
}


int format_minutes_to_hours(char buf[512], int minutes)
{
    int hours = minutes / 60;
    int remaining_minutes = minutes % 60;
    return snprintf(buf, 512, "%02dh%02dm", hours, remaining_minutes);
}

void sum_time(char *time1, char *time2, char buf[100]) {
    char chars_hours[10];
    char chars_minutes[10];
    int hours = 0;
    int minutes = 0;

    int hours_index = 0;
    int minutes_index = 0;
    while(time1) {
        if (time1[hours_index] == 'h') {
            chars_hours[hours_index] = '\0';
            break;
        }
        chars_hours[hours_index] = time1[hours_index];
        hours_index++;
    }
    while(time1) {
        if (time1[hours_index + minutes_index] == 'm') {
            chars_minutes[minutes_index-1] = '\0';
            break;
        }
        chars_minutes[minutes_index] = time1[hours_index + minutes_index + 1];
        minutes_index++;
    }

    hours += atoi(chars_hours);
    minutes += atoi(chars_minutes);
    hours_index = 0;
    minutes_index = 0;
    while(time2) {
        if (time2[hours_index] == 'h') {
            chars_hours[hours_index] = '\0';
            break;
        }
        chars_hours[hours_index] = time2[hours_index];
        hours_index++;
    }

    while(time2) {
        if (time2[hours_index + minutes_index] == 'm') {
            chars_minutes[minutes_index-1] = '\0';
            break;
        }
        chars_minutes[minutes_index] = time2[hours_index + minutes_index + 1];
        minutes_index++;
    }

    hours += atoi(chars_hours);
    minutes += atoi(chars_minutes);

    if (minutes >= 60) {
        hours += minutes / 60;
        minutes = minutes % 60;
    }

    snprintf(buf, 100, "%dh%02dm\n", hours, minutes);
}

void sanatize_db(char *db_path) {
    FILE *fp = fopen(db_path, "r");

    char buffer[1024];


    int lines = 0;

    while(!feof(fp))
    {
        char c = fgetc(fp);
        if (c == '\n') lines++;
    }
    Cell cells[lines];

    rewind(fp);

    int row = 0;
    int col = 0;

    while(fgets(buffer, 1024, fp)){

        if (row == 0) {
            row++;
            continue;
        }
        col = 0;
        char* value = strtok(buffer, ",");

        while(value) 
        {
            switch (col) {
                case 0:
                cells[row-1].sessions = atoi(value);
                break;

                case 1:
                strcpy(cells[row-1].date, value);
                break;

                case 2:
                strcpy(cells[row-1].time, value);
                break;

                default:
                break;
            }
            value = strtok(NULL,  ",");
            col++;
        }
        row++;
    }

    Cell new_cells[lines];
    int index_to_remove;
    int index = 0;

    int length = lines - 1;

    for (int i = 0; i < lines-1; i++) {
        index_to_remove = 0;
        for (int j = i + 1; j < lines-1; j++) {
            if (strcmp(cells[i].date, cells[j].date) == 0) {
                cells[j].sessions = cells[j].sessions + 1;
                sum_time(cells[j].time, cells[i].time, cells[j].time);
                index_to_remove = j;
                length--;
            }
        }

        if (index_to_remove == 0) {
            new_cells[index] = cells[i];
            index++;
        }
    }

    printf("--------------------\n");

    fp = fopen(db_path, "w");

    if (fp != NULL) {
        fprintf(fp, "Session,Date,Time\n");
        for (int i = 0; i < length; i++) {
            fprintf(fp, "%d,%s,%s", new_cells[i].sessions, new_cells[i].date, new_cells[i].time);
        }
    }
}

int add_to_db(const char* total_time)
{
    int success = 1;
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;

    char db_path[512];
    snprintf(db_path, 512, "%s/%s", homedir, DB_PATH);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char date[1024];
    snprintf(date, 1024, "%d/%02d/%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    FILE *fp = fopen(db_path, "r");
    if (fp == NULL) {
        fp = fopen(db_path, "w");
        fprintf(fp, "Session,Date,Time\n");
        rewind(fp);
    }

    fp = fopen(db_path, "a");
    if (fp != NULL) {
        fprintf(fp, "1,%s,%s\n", date, total_time);
        fclose(fp);
        sanatize_db(db_path);
        return success;
    }

    return !success;
}
