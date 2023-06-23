#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <iterator>
#include <cstdint>
#include <cstddef>
#include <bitset>
#include <errno.h>
#include <fcntl.h>
#include <array>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <thread>

#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <i2c/smbus.h>

#include "commands.h"
#include "posix_message_queue.hpp"

using namespace std;
using fcutils::system::posix_message_queue;

int file = 0;
bool new_cmd = false;
bool BMX_flag = false;
std::string response = "2020";

std::string Get_Date_Str()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[20];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%y%m%d",timeinfo);
  std::string str(buffer);
  return str;
}

std::string Get_DateTime_Str()
{
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[20];
  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer,sizeof(buffer),"%y%m%d_%H%M%S",timeinfo);
  std::string str(buffer);
  return str;
}

std::vector<uint32_t> HextoInt(const std::string& hex, int msg_size)
{
	std::vector<uint32_t> vmsg;
	for (int i = 0; i < msg_size; i += 2)
	{
		std::string byteString = hex.substr(i, 2);
		vmsg.push_back(stoi(byteString,0,16));
	}
	return vmsg;
}

void Parse_Run_CMD(const std::string& msg, int msg_size, std::string& response)
{
	std::vector<uint32_t> vCMD;

	if ((msg_size%2) > 0){
		cout << "Length of message mismatch. Please enter even number of hex letters." << endl;
		return;
	}

	vCMD = HextoInt(msg, msg_size);

	if (vCMD.size() != 8){
		cout << "Length of message mismatch. Please enter 8 bytes or 16 hex letters." << endl;
		return;
	}

	if(vCMD.at(4) == CMD_PING_SIAB){
		std::cout << "SIAB Process is Pinged" << std::endl;
		response = "0x20";
	}else{
		Process_CMD(file, vCMD, response);
		std::cout << "response: " << response << endl;
	}
}

void read_mq(sigval_t sig)
{
	int numRead{-1};
	// Retrive the posix message queue object from the sigval_t infos.
	posix_message_queue* mqp=static_cast<posix_message_queue*>(sig.sival_ptr);
	auto msg_size=mqp->get_msgsize();
	auto msg=new char[msg_size];
	auto mq_name=mqp->get_name();
	mqp->notify(read_mq);
	while ((numRead = mqp->receive(msg, msg_size)) > 0)
	{
		std::cout << " Queue: " << mq_name <<", message received: " << msg << ", of size: " << numRead << std::endl;
		std::cout << " CPU Time is : " << Get_DateTime_Str() << std::endl;
		if((msg[8] == '2') && (msg[9] == '7')){
			std::cout << "Enabling the BMX160 Logging." << std::endl;
			BMX_flag = true;
		}else if((msg[8] == '2') && (msg[9] == '8')){
			std::cout << "Disabling the BMX160 Logging." << std::endl;
			BMX_flag = false;
		}else if((msg[8] == '2') && (msg[9] == '9')){
			bool BMX_Flip1 = false;
			if(BMX_flag){
				BMX_flag = false;
				BMX_Flip1 = true;
				sleep(1);
			}
			std::string req_date;
			for(int i=0; i<6; i++)
			{
				req_date.push_back(msg[i+10]);
			}
			std::string BMX_filename = "/home/cherenkov/Programs/control_software/Archive/BMX160/BMX_"+req_date+".log";
			std::cout << "Archiving and Downloading the BMX160 file for: " << req_date << std::endl;
			std::string Download_TDRSS = "cp "+BMX_filename+" /srv/ftp/data/CT_0_"+Get_DateTime_Str()+"_02468.log &";
			system(Download_TDRSS.c_str());
			usleep(200000);
			std::string Download_StarLink = "cp "+BMX_filename+" /home/cherenkov/Documents/StarLinkSyncEngineer/CT_0_"+Get_DateTime_Str()+"_02468.log &";
			system(Download_StarLink.c_str());
			usleep(200000);
			if(BMX_Flip1) {BMX_flag = true;}
		}else{
			bool BMX_Flip2 = false;
			if(BMX_flag){
				BMX_flag = false;
				BMX_Flip2 = true;
				sleep(1);
			}
			Parse_Run_CMD(msg, numRead, response);
			if(BMX_Flip2) {BMX_flag = true;}
		}
	}
	new_cmd = true;
	delete [] msg;
}

void Sort_BMX_data(double *sorted_data, __u8 raw_data[])
{
	// Sorting the Magnetometer data
	if (raw_data[1] & 0x80){
		sorted_data[0] = - 0x10000 + ((raw_data[1] << 8) | (raw_data[0]));
	}else{
		sorted_data[0] =  (raw_data[1] << 8) | (raw_data[0]);
	}

	if (raw_data[3] & 0x80){
		sorted_data[1] = - 0x10000 + ((raw_data[3] << 8) | (raw_data[2]));
	}else{
		sorted_data[1] =  (raw_data[3] << 8) | (raw_data[2]);
	}

	if (raw_data[5] & 0x80){
		sorted_data[2] = - 0x10000 + ((raw_data[5] << 8) | (raw_data[4]));
	}else{
		sorted_data[2] =  (raw_data[5] << 8) | (raw_data[4]);
	}

	// Sorting the Gyroscope data
	if (raw_data[9] & 0x80){
		sorted_data[3] = - 0x10000 + ((raw_data[9] << 8) | (raw_data[8]));
	}else{
		sorted_data[3] =  (raw_data[9] << 8) | (raw_data[8]);
	}

	if (raw_data[11] & 0x80){
		sorted_data[4] = - 0x10000 + ((raw_data[11] << 8) | (raw_data[10]));
	}else{
		sorted_data[4] =  (raw_data[11] << 8) | (raw_data[10]);
	}

	if (raw_data[13] & 0x80){
		sorted_data[5] = - 0x10000 + ((raw_data[13] << 8) | (raw_data[12]));
	}else{
		sorted_data[5] =  (raw_data[13] << 8) | (raw_data[12]);
	}

	// Sorting the Accelerometer data
	if (raw_data[15] & 0x80){
		sorted_data[6] = - 0x10000 + ((raw_data[15] << 8) | (raw_data[14]));
	}else{
		sorted_data[6] =  (raw_data[15] << 8) | (raw_data[14]);
	}

	if (raw_data[17] & 0x80){
		sorted_data[7] = - 0x10000 + ((raw_data[17] << 8) | (raw_data[16]));
	}else{
		sorted_data[7] =  (raw_data[17] << 8) | (raw_data[16]);
	}

	if (raw_data[19] & 0x80){
		sorted_data[8] = - 0x10000 + ((raw_data[19] << 8) | (raw_data[18]));
	}else{
		sorted_data[8] =  (raw_data[19] << 8) | (raw_data[18]);
	}
}

void BMX_Log()
{
	if(BMX_flag)
	{
		int TOP_BMX_ADDR = 0x68;
		int BOT_BMX_ADDR = 0x69;
		__u8 BMX_DATA_SIZE = 20;
		__u8 BMX_DATA_ADDR = 0x04;
		__u8 bot_data[20] = {0};
		__u8 top_data[20] = {0};
		__u8 tbytes_read, bbytes_read = 0;

		std::ofstream BMX_File;
		std::string BMX_filename = "/home/trinty-ct-cpu/Programs/Trinity/control_software/Archive/BMX160/BMX_"+Get_Date_Str()+".log";
		BMX_File.open(BMX_filename.c_str(), ios::app|ios::ate);

		time_t sample_time = time (NULL);
		BMX_File << sample_time << "\t";

		// Reading the Bottom Backplane Chip
		if (ioctl(file, I2C_SLAVE, BOT_BMX_ADDR) < 0)
		{
			std::cout << "Error in setting up Bot BMX chip." << std::endl;
			exit(1);
		}
		bbytes_read = i2c_smbus_read_i2c_block_data(file, BMX_DATA_ADDR, BMX_DATA_SIZE, bot_data);
		if(bbytes_read != BMX_DATA_SIZE)
		{
			std::cout << "Number of bytes read from bottom chip mismatch" << std::endl;
		}
		else{
			double BMX_bot_data[9] = {0};
			Sort_BMX_data(BMX_bot_data, bot_data);
			for(int i=0; i<9; i++)
			{
				BMX_File << BMX_bot_data[i] << "\t";
			}
		}

		// Reading the Top Backplane Chip
		if (ioctl(file, I2C_SLAVE, TOP_BMX_ADDR) < 0)
		{
			std::cout << "Error in setting up Top BMX chip." << std::endl;
			exit(1);
		}
		tbytes_read = i2c_smbus_read_i2c_block_data(file, BMX_DATA_ADDR, BMX_DATA_SIZE, top_data);
		if(tbytes_read != BMX_DATA_SIZE)
		{
			std::cout << "Number of bytes read from top chip mismatch" << std::endl;
		}
		else{
			double BMX_top_data[9] = {0};
			Sort_BMX_data(BMX_top_data, top_data);
			for(int i=0; i<9; i++)
			{
				BMX_File << BMX_top_data[i] << "\t";
			}
		}
		BMX_File << std::endl;
		BMX_File.close();
	}
}

void BMX_timer(std::function<void(void)> func, uint32_t interval)
{
	std::thread([func, interval]()
	{
		while(true)
		{
			auto x = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
			func();
			std::this_thread::sleep_until(x);
		}
	}).detach();
}

int main()
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::cout << "Staring at: " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;

	file = open("/dev/i2c-1", O_RDWR);
	if (file < 0)
	{
		std::cout << "Error in opening file ..." << std::endl;
		exit(1);
	}

	std::string mqname="/writetoSIAB";
	std::cout << "Creating and opening the POSIX message queue: " << mqname << " for listening to commands ..." << std::endl;
	posix_message_queue rq(mqname.c_str());
	rq.open();

	mqname="/writetoRC";
	std::cout << "Creating and opening the POSIX message queue: " << mqname << " for responding to master ..." << std::endl;
	posix_message_queue wq(mqname.c_str());
	wq.open();

	rq.notify(read_mq);
	BMX_timer(BMX_Log, 1000);

	while(true){
		if(new_cmd){
			new_cmd = false;
			if(response == "0x20")
			{
				wq.send(response);
			}
			response.clear();
		}
		usleep(100000);
	}

	return 0;
}
