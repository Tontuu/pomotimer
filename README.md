# Pomotimer
## A [pomodoro](https://en.wikipedia.org/wiki/Pomodoro_Technique) timer with stats and fully implemented to work with lemonbar

### Setup
```console
./configure
```

### Some examples
```sh
# Help message
pomotimer --help

# Start interactively
pomotimer

# Start session in background 
pomotimer set 0 25 0 5 &

# Get current session state in background 
pomotimer get
```
***Note: if you proactively send a KILL or BREAK signal instead of waiting the pomotimer session to finish or running pomotimer quit comman, make sure that you remove manually the /tmp/tmp_pomotimer file so you can run it again. Pomotimer cannot delete the temporary file once it get killed before finishing the session per completely.***


*This program is made exclusively for me and it fits exactly to what i want. If you have any feature that you would like to see, leave a pull-request or open an issue and i'll see.*
