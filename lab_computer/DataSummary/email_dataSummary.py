#!/usr/bin/env python3

import smtplib, ssl
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.base import MIMEBase
from email.message import EmailMessage
from email import encoders
import time
import logging
import os
import subprocess
import time
import sys
from datetime import datetime, timedelta
from pypdf import PdfWriter

def run_ssh(command):
    # Run the command and capture its output
    result = subprocess.run(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # Remove ANSI escape codes
    #result = re.sub(r'\x1b\[[0-9;]*m', '', result.decode())
    # Check for errors
    if result.returncode == 0:
        # Command was successful
        # logging.info(f"{command} output:")
        # logging.info(result.stdout)
        
        logging.info(f"{command} output: {result.stdout} ")
        
    else:
        # Command encountered an error
        logging.error(f"{command} output error: {result.stderr} ")
        
    #return result.stdout


# Sends Emails
def send_email(date,attachment_path=None):
    # SMTP server details
    # Email configuration
    sender_email = 'sstepanoff3@gatech.edu'
    receiver_email = 'TrinityObservations@groups.gatech.edu'
    # Set the subject and body of the email
    subject = f'Trinity Data Summary {date}'
    #subject = 'TEST'
    body = f"""
    The Trinity Demonstrator Data Summary for {date}

    """

    # Create the email message
    msg = MIMEMultipart()
    msg['Subject'] = subject
    msg['From'] = sender_email
    msg['To'] = receiver_email
    msg.attach(MIMEText(body, "plain"))

    # Attach PDF file
    with open(attachment_path, "rb") as attachment:
        part = MIMEBase("application", "octet-stream")
        part.set_payload(attachment.read())
        encoders.encode_base64(part)
        part.add_header(
            "Content-Disposition",
            f"attachment; filename= {date}.pdf",
        )
        msg.attach(part)

    # Connect to the SMTP server
    smtp_server = smtplib.SMTP('outbound.mail.gatech.edu', 25)  # Assuming smtp.gatech.edu is the SMTP server for gatech.edu
    smtp_server.set_debuglevel(1)  # Optional: This will print debug information
    smtp_server.sendmail(sender_email, receiver_email, msg.as_string())

    # Close the connection
    smtp_server.quit()


def get_most_recent_file(folder_path):
    # Get list of files in the folder
    files = os.listdir(folder_path)
    if not files:
        return None

    # Filter out directories, get file paths and creation times
    file_info = [(os.path.join(folder_path, f), os.path.getmtime(os.path.join(folder_path, f))) for f in files if os.path.isfile(os.path.join(folder_path, f))]

    # Get the most recent file based on creation time
    most_recent_file = max(file_info, key=lambda x: x[1])[0]

    return most_recent_file

def compare_file_date_with_current(file_path):
    # Get file modification time
    file_modification_time = os.path.getmtime(file_path)
    file_modification_date = datetime.fromtimestamp(file_modification_time).date()
    
     # Get current date
    current_date = datetime.now().date()

    # Check file size
    file_size = os.path.getsize(file_path)  # size in bytes

    # Convert file size to MB
    file_size = file_size / 1024  # convert bytes to megabytes

    # Compare dates and file size
    if file_modification_date == current_date and file_size > 30:
        return file_modification_date.strftime('%Y%m%d') + ".pdf"
    else:
        logging.info('File size too small or none at all')
        return False

def merge_pdf(data,path):

  # Get current date
  current_date = datetime.now().date()
  # Calculate yesterday's date
  yesterday = current_date #- timedelta(days=1)

  formatted_date = yesterday.strftime('%Y%m%d')
  pdfs = [f'{path}scheduling/schedule_pdf/schedule_{formatted_date}.pdf',data]

  merger = PdfWriter()  
  for pdf in pdfs:
      merger.append(pdf)
  name = f"DataSummary_{formatted_date}.pdf"
  merger.write(name)
  merger.close()
  return name

# The command you want to run
system_path = "/data/TrinityLabComputer/"

# Configure logging
logging.basicConfig(filename=f'{system_path}/DataSummary/email.log', level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')


os.chdir(system_path+"/DataSummary/SummaryFiles/")

command = f'./getDataSummary.sh'  # Replace with your desired command
run_ssh(command)

folder_path = system_path + "DataSummary/SummaryFiles/"
most_recent_file = get_most_recent_file(folder_path)

if most_recent_file:
    logging.info(f"Most recent file: {most_recent_file}")
    todays_file=compare_file_date_with_current(most_recent_file)
    if todays_file != False:
        current_date = datetime.now().date()
        datasum_path = folder_path + todays_file
        data_sum_final=merge_pdf(datasum_path,system_path)
        attachment_path = folder_path + data_sum_final
        send_email(current_date,attachment_path)
        logging.info("The most recent file was modified today.")
        os.remove(attachment_path)
    else:
        logging.error("No file for today")
else:
    logging.info("Folder is empty.")

