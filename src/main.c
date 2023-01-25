#include "include/pomotimer.h"

char *help_message = "Usage: pomotimer [OPTION...]\n\
\n\
Pomotimer is a pomodoro program which was made to work with lemonbar as main goal\n\
\n\
Subcommands:\n\
    get                Get pomodoro current session status\n\
    set <hh mm ss mm>  Set pomodoro in the format of hours, minutes, seconds and break time\n\
    quit               Quit session\n\
    pause              Pause pomodoro session\n\
    unpause            Unpause pomodoro session\n\
    skip               Skip pomodoro current session\n\
    stats              Print pomotimer usage stats like total sessions and total elapsed time.\n\
\n\
Options:\n\
    -h, --help      Show this help message and exit";

int print_stats()
{
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;

    char db_path[512];
    snprintf(db_path, 512, "%s/%s", homedir, DB_PATH);

    FILE *fp = fopen(db_path, "r");

    if (fp == NULL) {
        panic(ERRNO);
    }

    char buffer[1024];

    int row = 0;
    int col = 0;

    printf("╭--------------------------------------╮\n");
    printf("|----------------STATS-----------------|\n");
    printf("╰--------------------------------------╯\n\n");
    printf("╭--------------------------------------╮\n\n");
    printf("   SESSIONS       DATE        TIME     \n");

    int total_sessions = 0;
    int hours = 0;
    int minutes = 0;
    while(fgets(buffer, 1024, fp)){

        if (row == 0) {
            row++;
            continue;
        }

        col = 0;

        char* value = strtok(buffer, ",");
        int int_value = 0;

        while(value) 
        {
            size_t hours_index_end = 0;
            char chars_minutes[3];
            char chars_hours[10];
            switch (col) {
                case 0:
                    int_value = atoi(value);
                    total_sessions += int_value;
                    printf("      %02d", int_value);
                    break;

                case 1:
                    printf("       %s", value);
                    break;

                case 2:
                    hours_index_end = (strlen(value)-1) - 4;

                    for (size_t i = hours_index_end+1; i < strlen(value)-1; i++) {
                        chars_minutes[i - hours_index_end - 1] = value[i];
                    }
                    chars_minutes[3] = '\0';

                    for (size_t i = 0; i < hours_index_end; i++) {
                        chars_hours[i] = value[i];
                    }
                    chars_hours[hours_index_end] = '\0';

                    hours += atoi(chars_hours);
                    minutes += atoi(chars_minutes);

                    printf("    %s", value);
                    break;

                default:
                    break;
            }
            value = strtok(NULL,  ",");
            col++;
        }
        printf("\n");
        row++;
    }
    if (minutes >= 60) {
        hours += minutes / 60;
        minutes = minutes % 60;
    }

    char sessions_str[100];
    sprintf(sessions_str, "%02d", total_sessions);
    char time_str[100];
    sprintf(time_str, "%02dh%02dm", hours, minutes);

    int trailing_space_count = (int) strlen(time_str) - 6;
    
    printf("╰--------------------------------------╯\n");
    printf("|                 TOTAL                |\n");
    printf("╭--------------------------------------╮\n");
    printf("|   SESSIONS%21s%7s\n", "TIME", "|");
    printf("|      %-20s%*s%*s\n", sessions_str, 7+trailing_space_count, time_str, 6-trailing_space_count, "|");
    printf("╰--------------------------------------╯\n");
    fclose(fp);
    return 0; 
}

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

    ArgParser *quit_cmd = ap_cmd(parser, "quit");
    ap_set_helptext(quit_cmd, "Usage: pomodoro quit");

    ArgParser *pause_cmd = ap_cmd(parser, "pause");
    ap_set_helptext(pause_cmd, "Usage: pomodoro pause");

    ArgParser *unpause_cmd = ap_cmd(parser, "unpause");
    ap_set_helptext(unpause_cmd, "Usage: pomodoro unpause");

    ArgParser *skip_cmd = ap_cmd(parser, "skip");
    ap_set_helptext(skip_cmd, "Usage: pomodoro skip");

    ArgParser *stats_cmd = ap_cmd(parser, "stats");
    ap_set_helptext(stats_cmd, "Usage: pomodoro stats");


    ap_str_opt(parser, "time", 0);
    ap_str_opt(set_cmd, "time", 0);

    ap_parse(parser, argc, argv);

    if (ap_has_cmd(parser)) {
        char *ap_cmd_str = ap_cmd_name(parser);
        if (strcmp(ap_cmd_str, "get") == 0) {
            return pomodoro_timer_get();
        }

        if (strcmp(ap_cmd_str, "set") == 0) {
            if (fopen(TMP_FILE, "r") != NULL) {
                panic(ALREADY_RUNNING);
            } 

            Time time = parse_time_args(set_cmd);
            ap_free(parser);
            pomodoro_timer(time.hours, time.minutes, time.seconds, time.break_time,
            SET);
        }

        if (strcmp(ap_cmd_str, "quit") == 0) {
            if (system("pkill -o -9 pomotimer &>/dev/null") == 0) {
                if (remove_tempfile() == -1) {
                    panic(ERRNO);
                }
                printf("Successfully exited pomotimer\n");
            } else {
                panic(EXIT);
            }
            return 0;
        }

        if (strcmp(ap_cmd_str, "pause") == 0) {
            if (!pomodoro_pause())
            return 1;

            printf("Pomodoro session paused\n");
            return 0;
        }

        if (strcmp(ap_cmd_str, "unpause") == 0) {
            if (!pomodoro_unpause())
            return 1;

            printf("Pomodoro session unpaused\n");
            return 0;
        }

        if (strcmp(ap_cmd_str, "skip") == 0) {
            if (!pomodoro_skip())
            return 1;

            printf("Pomodoro session skipped\n");
            return 0;
        }

        if (strcmp(ap_cmd_str, "stats") == 0) {
            if (!print_stats())
            return 1;
        }

    } else {
        ap_free(parser);
        pomodoro_timer(0, 25, 0, 5, SET_AND_INTERACTIVE);

        system("clear");
    }

    return 0;
}
