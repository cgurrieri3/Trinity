# generate the file
# auto set time - from SAM
# Maunel set time is the sam as sam unless changed by the user

# check to make sure the time makes sense
# get UTC time from lab computer
# get UTC time of SAM/Manuel time
# Sam will add buffer
import ephem
import pytz
import requests
from datetime import datetime, timezone, timedelta, time as time_obj
import pandas as pd
import time
import communicate as lets

latitude = 38.5202  # Latitude of Frisco Peak
longitude = -113.2883  # Longitude Frisco Peak
elevation = 3048  # Elevation of Frisco Peak
interval_minutes = 1  # Adjust the interval as needed

datetime_today = datetime.utcnow() # - timedelta(hours=6)
day_today = datetime_today.date()
time_today = datetime_today.time()

if time_today > time_obj(15, 15):
	day_today += timedelta(days=1)
else:
	pass

start_date = datetime.combine(day_today, time_obj(0, 0))

# Getting relevant moon times

def moon_position_over_time(latitude, longitude, start_date, interval_minutes):
	observer = ephem.Observer()
	observer.lat = str(latitude)
	observer.lon = str(longitude)
	observer.elevation = elevation

	moon_times = []
	moon_altitudes = []

	for hour in range(24):
		current_time = start_date + timedelta(hours=hour)

		for minute in range(0, 60, interval_minutes):
			current_time_with_minute = current_time + timedelta(minutes=minute)
			observer.date = current_time_with_minute
			moon = ephem.Moon(observer)
			m_altitude = float(moon.alt) * 180 / ephem.pi  # Convert altitude to degrees
			moon_times.append(current_time_with_minute)
			moon_altitudes.append(m_altitude)

	moonrise_index = next((i for i in range(1, len(moon_altitudes)) if moon_altitudes[i] > 0 and moon_altitudes[i - 1] <= 0), None)
	time_of_moonrise = moon_times[moonrise_index] if moonrise_index is not None else None

	moonset_index = next((i for i in range(1, len(moon_altitudes)) if moon_altitudes[i] < 0 and moon_altitudes[i - 1] >= 0), None)
	time_of_moonset = moon_times[moonset_index] if moonset_index is not None else None

	max_altitude_index = moon_altitudes.index(max(moon_altitudes))
	time_of_max_altitude = moon_times[max_altitude_index]

	return moon_times, moon_altitudes, time_of_moonrise, time_of_moonset, time_of_max_altitude





# Getting relevant sun times

def sun_position_over_time(latitude, longitude, start_date, interval_minutes):
	observer = ephem.Observer()
	observer.lat = str(latitude)
	observer.lon = str(longitude)
	observer.elevation = elevation

	sun_times = []
	sun_altitudes = []

	for hour in range(24):
		current_time = start_date + timedelta(hours=hour)

		for minute in range(0, 60, interval_minutes):
			current_time_with_minute = current_time + timedelta(minutes=minute)
			observer.date = current_time_with_minute
			sun = ephem.Sun(observer)
			s_altitude = float(sun.alt) * 180 / ephem.pi
			sun_altitudes.append(s_altitude)
			sun_times.append(current_time_with_minute)

	sunrise_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] > -15 and sun_altitudes[i - 1] <= -15), None)
	time_of_sunrise_crit = sun_times[sunrise_index] if sunrise_index is not None else None

	sunset_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] < -15 and sun_altitudes[i - 1] >= -15), None)
	time_of_sunset_crit = sun_times[sunset_index] if sunset_index is not None else None

	sunrise_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] > 0 and sun_altitudes[i - 1] <= 0), None)
	time_of_sunrise = sun_times[sunrise_index] if sunrise_index is not None else None

	sunset_index = next((i for i in range(1, len(sun_times)) if sun_altitudes[i] < 0 and sun_altitudes[i - 1] >= 0), None)
	time_of_sunset = sun_times[sunset_index] if sunset_index is not None else None



	return sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit





# Creating list of unsafe times

def get_times(sun_altitudes, sun_times, moon_altitudes, moon_times):
	times_list = []
	date_format = "%Y-%m-%d %H:%M:%S"
	for i in range(len(moon_altitudes) - 1):
		if moon_altitudes[i] < moon_altitudes[i - 1] and moon_altitudes[i] > 0:
			times_list.append(sun_times[i].strftime(date_format))
	for i in range(len(sun_altitudes) - 1):
		if sun_altitudes[i] > -15 and not (moon_altitudes[i] < moon_altitudes[i - 1] and moon_altitudes[i] > 0):
			times_list.append(sun_times[i].strftime(date_format))
	return times_list





# Get endtime

def get_endtime(time_of_sunrise_crit, time_of_sunset_crit, time_of_moonset, time_of_max_altitude):
	start_time = None
	end_time = None
	start_time_2 = None
	end_time_2 = None
	if not ((time_of_sunset_crit) < time_of_moonset < (time_of_sunrise_crit)):
		start_time = (time_of_sunset_crit)
	else:
		start_time = time_of_moonset

	if not ((time_of_sunset_crit) < time_of_max_altitude < (time_of_sunrise_crit)):
		end_time = (time_of_sunrise_crit)
	else:
    		end_time = time_of_max_altitude

	if start_time == (time_of_sunset_crit) and end_time == time_of_max_altitude:
		if (time_of_sunset_crit) < time_of_moonset < (time_of_sunrise_crit):
			start_time_2 = time_of_moonset
			end_time_2 = (time_of_sunrise_crit)

	if start_time > end_time == time_of_max_altitude:
		start_time_2 = start_time
		start_time = time_of_sunset_crit
		end_time_2 = time_of_sunrise_crit

	if not (start_time_2 == None and end_time_2 == None):
		if start_time_2 > end_time_2:
			start_time_2 = None
			end_time_2 = None

	if not (start_time_2 == None and end_time_2 == None):
		if (end_time_2 - start_time_2) < timedelta(minutes=45):
			end_time_2 = None
			start_time_2 = None
		elif (end_time - start_time) < timedelta(minutes=45):
			start_time = start_time_2
			end_time = end_time_2
			end_time_2 = None
			start_time_2 = None

	return end_time, start_time





# Create file

def create_file(user_input='nope'):
	try:
		date_format = "%Y-%m-%d %H:%M:%S"
		user_input = datetime.strptime(user_input, date_format)

	except ValueError:
		user_input = 0

	moon_times, moon_altitudes, time_of_moonrise, time_of_moonset, time_of_max_altitude = moon_position_over_time(latitude, longitude, start_date, interval_minutes)
	sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit = sun_position_over_time(latitude, longitude, start_date, interval_minutes)
	sun_moon_list = [time_of_sunrise.strftime(date_format), time_of_sunset.strftime(date_format), time_of_moonrise.strftime(date_format), time_of_moonset.strftime(date_format)]
	endtime, start_time = get_endtime(time_of_sunrise_crit, time_of_sunset_crit, time_of_moonset, time_of_max_altitude)

	sun_moon_list.append(start_time)

	if user_input !=  0:
		if user_input < time_of_sunrise_crit and user_input > time_of_sunset_crit:
                        # and user_input < moonrise:
			 sun_moon_list.append(user_input)
		else:
                        lets.communicate('User input not safe')

	else:
		sun_moon_list.append(endtime.strftime(date_format))


	with open("/data/TrinityLabComputer/obs_prgm/EON_time_adam.txt", "w") as file:
	    	# Loop through the list and write each element to the file
		for item in sun_moon_list:
			file.write(str(item) + '\n')
		file.close()

	print(" ")
	print("Moonrise time (UTC):", time_of_moonrise.strftime("%Y-%m-%d %H:%M"))
	print("Moonset time (UTC):", time_of_moonset.strftime("%Y-%m-%d %H:%M"))
	print("Sunrise time (UTC):", time_of_sunrise.strftime("%Y-%m-%d %H:%M"))
	print("Sunset time (UTC):", time_of_sunset.strftime("%Y-%m-%d %H:%M"))
	print(" ")
	print("\033[1mExtrigs time (UTC):\033[1m", start_time.strftime("%Y-%m-%d %H:%M"))
	print("\033[1mCutoff time (UTC):\033[1m", endtime.strftime("%Y-%m-%d %H:%M"))

	lets.log_file('EON_time_adam.txt created')





# Check current time

def check_current_time():
	df = pd.read_csv('/data/TrinityLabComputer/obs_prgm/EON_time_adam.txt',header=None)
	df.columns = ['UTC DateTime']
	df['UTC DateTime'] = pd.to_datetime(df['UTC DateTime'])
	row_titles = [ 'Sunrise', 'Sunset','Moonrise', 'Moonset', 'Start Extrigs',  'Cutoff']
	df.index = row_titles
	cutoff_row = df.loc['Cutoff']
	cutoff_time = cutoff_row['UTC DateTime']
	current_utc_time = datetime.utcnow()

	moon_times, moon_altitudes, time_of_moonrise, time_of_moonset, time_of_max_altitude = moon_position_over_time(latitude, longitude, start_date, interval_minutes)
	sun_times, sun_altitudes, time_of_sunrise, time_of_sunset, time_of_sunrise_crit, time_of_sunset_crit = sun_position_over_time(latitude, longitude, start_date, interval_minutes)
	times_list = get_times(sun_altitudes, sun_times, moon_altitudes, moon_times)
	endtime, start_time = get_endtime(time_of_sunrise_crit, time_of_sunset_crit, time_of_moonset, time_of_max_altitude)

	#print(sunset_time)
	def curr_est_offset():
		tz_est = pytz.timezone('US/Eastern')
		offset = tz_est.utcoffset(datetime.utcnow())
		offset_seconds = (offset.days * 86400) + offset.seconds
		offset_hours = offset_seconds // 3600
		return abs(offset_hours) # -4 or -5

	if (start_time < current_utc_time < cutoff_time): # and not (current_utc_time.strftime("%Y-%m-%d %H:%M") in times_list):   #and current_utc_time > sunset_time:

		lets.communicate(f'TIME: TIME - The time is safe cutoff is {cutoff_time-timedelta(hours =curr_est_offset())} in ET')

		return 1
	else:
		lets.communicate('TIME: Current time UNSAFE')
		return 0




create_file()
check_current_time()

