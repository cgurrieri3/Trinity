import smtplib, ssl
from email.message import EmailMessage
import time
import sources as src

# Sends Emails
def send_email(reason):
    port = 465  # For starttls
    smtp_server = "smtp.gmail.com"
    sender_email = "sofiastepanoff22@gmail.com"
    receiver_email = "TrinityObservations@groups.gatech.edu"
    password = 'jjxqrdecssjizosh'
    
    # Set the subject and body of the email
    subject = 'Trinity EON'
    body = f"""
    The Trinity demonstrator has shutdown due to: {reason}

    """

    em = EmailMessage()
    em['From'] = sender_email
    em['To'] = receiver_email
    em['Subject'] = subject
    em.set_content(body)


    context = ssl.create_default_context()
    #server.starttls(context=context)
    with smtplib.SMTP_SSL('smtp.gmail.com', 465, context=context) as server:
        server.ehlo()
        
        server.login(sender_email, password)
        server.sendmail(sender_email, receiver_email, em.as_string())
        server.close()
        print('successfully sent the mail')
        log_file('successfully sent the mail')

# Saves to log file

def log_file(message):
    current_time = time.time()
    current_time = time.ctime(current_time)
    with open("/home/mpotts32/obs_prgm/trinity.log", "a") as file:
        file.write(message + str(current_time) + '\n')


# prints to user and saves to log file to make the code easier to read
def communicate(m):
    print(m)
    log_file(f'{m} ')

def pad_word(word, total_length=30):
    if len(word) >= total_length:
        return word
    else:
        padding_length = total_length - len(word)
        left_padding = padding_length // 2
        right_padding = padding_length - left_padding
        padded_word = '*' * left_padding + word + '*' * right_padding
        return padded_word

def symbol_decorator(func):
    def wrapper(*args, **kwargs):
        print("==============================")
        result = func(*args, **kwargs)
        print("==============================")
        return result
    return wrapper

# makes pretty communications for the user to make things easier to read
@symbol_decorator
def fancy_communicate(m):
    result = pad_word(m)
    print(result)

#fancy_communicate('Trigger Rate scan complete: \nTo start a new scan type: triggerScan, newScan \nTo start DAQ type: triggerScan, startDAQ \nSM not Enabled')

def get_source_times():
    source1, source2 =src.get_source_times()
    print('NGC1068:')
    communicate(source1)
    print('TXS 0506+056:')
    communicate(source2)
    

get_source_times()

