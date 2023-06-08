import time
import datetime
import os
import sys
from CMD_Dict import cmdDict

# getting tim
def getTimeStamp():
    ts = time.time()
    d = datetime.datetime.fromtimestamp(ts).strftime('%d.%m.%Y')
    t = time.localtime()
    printTime = time.strftime("%H:%M:%S", t)
    return d, printTime

def WriteHeader(filename):

    header = "time:       message sent: \n"
    file = open(filename, 'r')
    lines = [line for line in file]
    file.close()
    if lines and lines[0] == header:
        # There are some lines in the file, and first line is the header
        return True
    else:
        # The first line is NOT the header
        file = open(filename, 'w')
        # Rewrite the file: append header if needed, and all lines which previously were there
        # excluding any misplaced header lines which were not at row 1
        file.write(header + ''.join([line for line in lines if not line == header]))
        file.close()
        return True

# sets up command logging
def logCMD(cmd):
    d, t = getTimeStamp()
    with open("Logs/Command_Log_{}.txt".format(d), "a") as logFile:
        WriteHeader("Logs/Command_Log_{}.txt".format(d))
        logFile.write(cmd)
        logFile.close()

def main():
    
    # collect user input and load it to proper variables
    Pid = sys.argv[1]
    payload = sys.argv[2]

    # set up message to be sent and logged
    msg = cmdDict[Pid][payload]
    print(msg)
    d, t = getTimeStamp()
    logCMD(str(t) + ' || ' + msg + '\n')

    # run the commanding bash script to send the command: 
#    command="./run.sh "+msg
#    os.system(command)

if __name__ == "__main__":
    main()
