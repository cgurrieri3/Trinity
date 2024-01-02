import subprocess
import time

# run an expect script
def run_shut_down_ct():
    # Specify the path to your Expect script
    expect_script = "/home/trinity/control_software/FailSAFE/ct_exact_scripts/shut_down.exp"

    # Run the Expect script using the 'expect' command
    process = subprocess.Popen(["expect", expect_script], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # Wait for the process to finish
    stdout, stderr = process.communicate()

    # Check the output and any errors
    if process.returncode == 0:
        print("Expect script executed successfully")
        print("Output:\n", stdout)
        log_file(f'Shut down camera Successful ')
    else:
        print("Error running Expect script")
        print("Error Output:\n", stdout)
        log_file(f'Shut down camera failed')



# add to log file
def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/home/trinity/control_software/FailSAFE/noLabConnection.log", "a") as file:
        file.write(message + str(current_time) + '\n')

def ping(host):
    try:
        subprocess.check_output(['ping', '-c', '4', host])  # Use '-n' instead of '-c' on Windows
        return True
    except subprocess.CalledProcessError:
        return False

# ping gatech lab computer
host_to_ping = 'phys43199.physics.gatech.edu'
if ping(host_to_ping):
    #print(f"{host_to_ping} is reachable.")
    log_file(f"{host_to_ping} is reachable. -")
    
    #run_shut_down_ct()
    
else:
    log_file(f"{host_to_ping} is not reachable. -")
    
    run_shut_down_ct()


    

    
