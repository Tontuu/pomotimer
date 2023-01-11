#include "include/pomotimer_utils.h"

void panic(Error error) {
  char error_str[100];
  if (error == ERRNO)
    strcpy(error_str, strerror(errno));

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

  fprintf(stderr, "ERROR: %s\n", error_str);
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
      panic(ERRNO);
    fclose(fp);
    return 0;
  }

  return -1;
}

int remove_tempfile() {
  return remove(TMP_FILE);
}


int check_time(int time_value) {
  if (time_value > 60 || time_value < 0 ) {
    return -1;
  }

  return time_value;
}

Time parse_time_args(ArgParser *parser) {
  if (ap_count_args(parser) < 3)
    panic(TOO_FEW_ARGUMENTS);

  if (ap_count_args(parser) > 3)
    panic(TOO_MUCH_ARGUMENTS);

  int *time_values = ap_args_as_ints(parser);

  if (ap_count_args(parser) > 3)
    panic(TOO_MUCH_ARGUMENTS);

  if (ap_count_args(parser) < 3)
    panic(TOO_FEW_ARGUMENTS);

  return check_values(time_values);
}

Time check_values(int *time_values) {
  Time time;

  if (time_values[0] > 12 || time_values[0] < 0)
    panic(INVALID_HOUR);

  if (time_values[1] > 60 || time_values[1] < 0)
    panic(INVALID_MINUTE);

  if (time_values[2] > 60 || time_values[2] < 0)
    panic(INVALID_SECONDS);

  time.hours = time_values[0];
  time.minutes = time_values[1];
  time.seconds = time_values[2];

  free(time_values);
  return time;
}
