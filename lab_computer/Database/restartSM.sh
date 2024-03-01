#!/bin/bash

# Run the first command
pkill -f influx_load_statemessages.py

# Run the second command
output="/home/mpotts32/Database/output.log"
exec "$(type -p nohup)" /home/mpotts32/Database/ctcpu_state_mess/influx_load_statemessages.py "${@}" > "$output" 2>&1 &


exit 0
