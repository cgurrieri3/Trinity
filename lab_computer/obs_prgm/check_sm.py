from influxdb import InfluxDBClient
from datetime import datetime,timedelta
import time
import communicate as lets


def accepted_values_16(meas, uc_t, simp_t, hv_s, siab_c, music_p):
	if meas == "UC_temps":
		acpt_low = uc_t * 0 + -10
		acpt_hi = uc_t * 1.3

	elif meas == "SiPM_temps":
		acpt_low = simp_t*0 + -10
		acpt_hi = simp_t*1.3

	elif meas == 'HV_Status':
		acpt_low = hv_s * 1
		acpt_hi = hv_s *1
	elif meas == "SIABcurrent":
		acpt_low = siab_c * .8
		acpt_hi = siab_c * 1.2
	elif meas == "MUSIC_Power":
		acpt_low = music_p *1
		acpt_hi = music_p *1
	elif meas == "other":
		acpt_low = 0
		acpt_hi = 0
	return acpt_low, acpt_hi

def dp16(ldp,meas,siab_c, simp_t, uc_t, music_p,hv_s):
	
	low,hi=accepted_values_16(meas, uc_t, simp_t, hv_s,siab_c,music_p)
	checked = []
	index = list(range(1, 17))
	

	for i in index:
		
		value=float(ldp[f'{i}'])
		if value == low and value == hi:
			
			checked.append(1)

		elif value >= low and value <= hi:
			
			checked.append(1)
		else:
			
			checked.append(0)
	#if meas == 'SIABcurrent' or meas == 'HV_Status' or meas == 'MUSIC_Power' or meas =:
	total=sum(checked)
	if total == 16:
		
		return 1
	else:
		lets.communicate(f'Conditions not matched {meas}')
		
		return 0
			
def accepted_values_1(meas, asad,tb_c, trigger_rate):
	
	if meas == "ASADCurrent":
		acpt_low = asad * 0.9
		acpt_hi = asad * 1.3
	elif meas == "TBCurrent":
		acpt_low = tb_c * 0.9
		acpt_hi = tb_c * 1.1
	elif meas == "TriggerRate":
		acpt_low = trigger_rate * 0
		acpt_hi = trigger_rate * 3998
	return acpt_low, acpt_hi


def dp1(ldp,meas,asad,tb_c,trigger_rate):
	low,hi=accepted_values_1(meas, asad,tb_c,trigger_rate)
	
	
		
	value=float(ldp['value1'])
	if value == low and value == hi:
		
		return 1

	elif value >= low and value <= hi:
		
		return 1
	else:
		
		lets.communicate(f'Conditions not matched {meas}')
		
		return 0

def accepted_values_4(meas, hv_c,hv):
	#print(asad)
	#print(meas)
	if meas == "HV":
		acpt_low = hv * 0.95
		acpt_hi = hv * 1.05
	elif meas == "HV_currents":
		acpt_low = 2
		acpt_hi = hv_c
	return acpt_low, acpt_hi

def dp4(ldp,meas,hv_c,hv):
	#print('hey',meas)
	low,hi=accepted_values_4(meas, hv_c,hv)
	#print(low, hi)

	checked = []
	index = list(range(1, 5))
	

	for i in index:
		
		value=float(ldp[f'{i}'])
		if value == low and value == hi:
			
			checked.append(1)

		elif value >= low and value <= hi:
			
			checked.append(1)
		else:
			
			checked.append(0)
	#if meas == 'SIABcurrent' or meas == 'HV_Status' or meas == 'MUSIC_Power' or meas =:
	total=sum(checked)
	if total == 4:
		#print(f'Conditions matched {meas}')
		return 1
	else:
		lets.communicate(f'Conditions not matched {meas}')
		
		return -100

def dp8(ldp,meas,total):

	index = list(range(1,9))
	s = 0
	for i in index:
		value = float(ldp[f'{i}'])
		s = value + s
	
	if s > 0:
		return 1
	else:
		lets.communicate(f'Conditions not matched {meas}')
		
		return 0

def get_time_sm(client):
	query = f"SELECT * FROM \"TriggerRate\" ORDER BY time DESC LIMIT 1"
	result = client.query(query)
	points = list(result.get_points())
	SM_time = points[0]['time']
	
	SM_time = datetime.strptime(SM_time, "%Y-%m-%dT%H:%M:%SZ")
	
	return SM_time


def query_last_SM(siab_c, simp_t, uc_t, music_p, hv_s,hv_c, hv,asad, tb_c,trigger_rate,module):
	#SIAB_current, HVcurrent, SIMP_Temp, UC_Temp,MUSICpower,HVswitch,hv_Current, hv, asad, tb_current,trigger
	# Local host lines for access
	host = 'localhost'
	port = 8086
	username = 'mpotts32'
	password = 'Ttys@210'
	database = 'trinity_ct' # Database for independent measurements for each day, need to change the measurent line in cre_df_list()
    #database = 'Trinity1'

    # Initialize the InfluxDB client and write the points in batches
	client = InfluxDBClient(host = host, port=port, username=username, password=password,database=database)

    # Create a new database if it does not already exist
    # client.create_database(database)
	result = client.query('SHOW MEASUREMENTS')
	measurements = [measurement['name'] for measurement in result.get_points()]

	matched_conditions = []
	last_data_point = 0
	for measurement in measurements:
		query = f"SELECT * FROM {measurement} ORDER BY time DESC LIMIT 1"
		result = client.query(query)
		points = list(result.get_points())
		
		for data in points:
		    for key, value in data.items():
		        if value is None:
		            data[key] = 0
		
		if points:
			last_data_point = points[0]
			try:
				lets.log_file(f"Last data point for measurement '{measurement}': {last_data_point['1']}")

				try:
					#print(f"Last data point for measurement '{measurement}': {last_data_point['16']}")
					#siab_c, simp_t, uc_t, music_p,hv_s
					match_status = dp16(last_data_point,measurement,siab_c, simp_t, uc_t, music_p,hv_s)	
					matched_conditions.append(match_status)
					#print(matched_conditions)
				except Exception as e:
					#print(e)
					try:

						#print(f"Last data point for measurement '{measurement}': {last_data_point['8']}")
						matched_status=dp8(last_data_point,measurement,module)
						#print(matched_conditions)
					except:
						#print(f"Last data point for measurement '{measurement}': {last_data_point['4']}")
						match_status = dp4(last_data_point,measurement,hv_c, hv)
						matched_conditions.append(match_status)
						

						#print(matched_conditions)
			except:
				lets.log_file(f"Last data point for measurement '{measurement}': {last_data_point['value1']}")
				match_status=dp1(last_data_point,measurement,asad,tb_c,trigger_rate)
				matched_conditions.append(match_status)
				#print(matched_conditions)
		else:
			lets.communicate(f"No data found for measurement '{measurement}'")
			
	# check to make sure the state messages is within the last 5 minutes
	
	time_difference = abs(datetime.utcnow() - get_time_sm(client))
	five = timedelta(minutes=5)
	if time_difference > five:
		lets.communicate('No statemessages within 5 minutes')
		return 2

	all_conditions = sum(matched_conditions)
	if len(matched_conditions) == all_conditions:
		lets.communicate('State messages are SAFE')
		return 1

	elif all_conditions < 0:
		lets.communicate('HV current or voltage shutdown')
		return -1

	else:

		# lets.communicate('Check statemessages something is wrong')	
		return 0