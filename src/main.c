#include "include/pomotimer.h"

char* help_message = "Usage: pomotimer [OPTION...]\n\
\n\
Pomotimer is a pomodor that made focusing to work with lemonbar\n\
\n\
Subcommands:\n\
    get             Get pomodoro current session status\n\
    set <TIME>      Set pomodoro in the format of hours, minutes, seconds and break time\n\
\n\
Options:\n\
    -h, --help      Show this help message and exit";

int main(int argc, char **argv) {
  ArgParser *parser = ap_new();

  if (!parser)
    panic(ERRNO);

  ap_set_helptext(parser, help_message);
  ap_set_version(parser, "1.0");

  ArgParser *get_cmd = ap_cmd(parser, "get");
  ap_set_helptext(get_cmd, "Return current session status");

  ArgParser *set_cmd = ap_cmd(parser, "set");
  ap_set_helptext(set_cmd, "Usage: pomodoro set <hh> <mm> <ss> <break-time>");

  ap_str_opt(parser, "time", 0);
  ap_str_opt(set_cmd, "time", 0);

  ap_parse(parser, argc, argv);

  if (ap_has_cmd(parser)) {
    char *ap_cmd_str = ap_cmd_name(parser);
    if (strcmp(ap_cmd_str, "get") == 0) {
      return pomodoro_timer_get();
    }

    if (strcmp(ap_cmd_str, "set") == 0) {
      Time time = parse_time_args(set_cmd);
      ap_free(parser);
      pomodoro_timer(time.hours, time.minutes, time.seconds, time.break_time, SET);
    }
  } else {
    ap_free(parser);
    pomodoro_timer(0, 25, 0, 5, SET_AND_INTERACTIVE);
    system("clear");
  }

  return 0;
}
