#!/bin/bash

# Run the first command
pkill -f influxdb_load_cont.py

# Run the second command
output="/home/mpotts32/Database/output.log"
exec "$(type -p nohup)" /home/mpotts32/Database/influxdb_load_cont.py "${@}" > "$output" 2>&1 &


exit 0
