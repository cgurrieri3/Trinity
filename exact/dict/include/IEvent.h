#include <TROOT.h>
#ifndef IFHBEVENT_H
#define IFHBEVENT_H

#include "Event.h"

using namespace std;

/**
 * \class IEvent
 * \brief Class that contains the raw data plus the auxilliary data
 * 
 * Inherits from class Event and adds new data to each of the events based on the auxilliary data obtained from the State Messages.
 * Data contains information about the telescope conditions at the time of trigger
 * @see Event()
 * 
 * */

class IEvent : public Event{
	public:
		/** 
		 * Public constructor
		 * */
		IEvent(); 
		/**
		 * Public destructor
		 * */
		~IEvent(); 

	protected:
		
		float IcorrectionFactor;/**< Depreacted*/


	private:
		// Parameters from the first file
		float latitude;/**< float containing geographic latitude of the balloon*/
		float longitude;/**< float containing geographic longitude of the balloon*/
		int altitude;/**< int containing the altitude of the balloon*/
		float sunAzimuth;/**< float containing the azimuth of the Sun*/
		float sunElevation;/**< float containing the elevation of the Sun*/
		float moonAzimuth;/**< float containing the azimuth of the Moon*/
		float moonElevation;/**< float containing the elevation of the Moon*/
		float horizon;/**< float containing the location of the horizon in degrees*/
		float azimuth;/**< float containing the azimuth pointing of the telescope*/

		// Parameters from the second file
		int TrigEvent; /**< int containing the number of triggers in the run*/
		int TempFlag; /**< int flipping between temperature of SiPM or MicroController (obsolete)*/
		int emon1;/**< int with first emon reading*/
		int emon2;/**< int with second emon reading*/
		std::vector<float> hv; /**< std::vector of size 8 containing the setting of the 8 different high voltage channels*/
		std::vector<float> hvc;/**< std::vector of size 8 containing the current of the * different high voltage channels*/
		std::vector<float> ucTemp; /**< std::vector of size 32 containing the temperature for the 32 microControllers*/
		int CpuTemp;/**< int containing the temperature of the CPU*/
		int CoboTemp;/**< int containing the temperature of the CoBo*/
		int RadTemp;/**< int containing the temperature of the radiator*/
		float lvpsVol;/**< float containing the output voltage of the LVPS*/
		float pumpVol;/**< float containing the ouptu voltage to the pumps*/
		int lvpsCur;/**< int contiaing the output current of the LVPS*/
		int pumpCur;/**< int containing the current of the pump*/
		std::vector<int> siabMPWR;/**< std::vector<int> of the MUSIC power switch*/
		std::vector<int> hvSW;/**< std::vector<int> of the High voltage power switch*/
		std::vector<float> sipmTemp;/**< std::vector<float> of the temperature of the SiPM matrices*/

		// Tilt Sensor information
		float tiltAngle;/**< float with the corrected tilt angle without voltage sag*/
		float tiltAngleRaw;/**< float containing the raw tilt angle from state messages*/

		//Run Number
		int runNumber;/**< int containing the number of the run*/

		//Revised Trigger Time
		unsigned long long rTimeTB;/**< unsigned long long containing the revised TB Time**/


	public:
		// void LoadFromFile(const std::string& filename1, const std::string& filename2);
		/**
		 * Public method to fill in most of the member variables. Vectors have size row x cols, depending on the csv file used to obtain them.
		 * Each of the rows corresponds to a different time stamp and each column to different values of the different auxilliary data sources. Column 0 should be the timestamp
		 * 
		 * 
		 * @param timestamp An int storing the time of trigger in seconds after a preset vlaue (May 13th)
		 * @param data A std::vector of strings with size row x cols from the .csv file containing the balloon infromation.
		 * @param data2 A std::vector of strings containing the state message information.
		 * */
		void SetParametersFromTimestamp(int timestamp,std::vector<std::vector<std::string>> data,std::vector<std::vector<std::string>> data2);
		/**
		 * Public method to fill in the tiltAngle class members.
		 * 
		 * @param timestamp An int storing the time of trigger in seconds after a preset vlaue (May 13th)
		 * @param data A std::vector of strings with size row x cols from the .csv file containing the tilt infromation.
		 * */
		void SetTelescopePointing(int timestamp, std::vector<std::vector<std::string>> data);
		/**
		 * Public method to fill in the tiltAngleRaw class members.
		 * 
		 * @param timestamp An int storing the time of trigger in seconds after a preset vlaue (May 13th)
		 * @param data A std::vector of strings with size row x cols from the .csv file containing the tilt infromation.
		 * 
		 * */
		void SetTelescopePointingRaw(int timestamp, std::vector<std::vector<std::string>> data);
		/**
		 * Public method to get the closest timestamp to the event in the auxilliary data file1.
		 * 
		 * @return An int with the closest timestamp
		 * */
		int GetclosestTimestamp1() const;
		/**
		 * Public method to get the closest timestamp to the event in the auxilliary data file2.
		 * 
		 * @return An int with the closest timestamp
		 * */
		int GetclosestTimestamp2() const;
		/**
		 * Public method to get the geographic latitute of the balloon at the time the event was recorded.
		 * 
		 * @return A float with the latitude in degrees
		 * */
		float GetLatitude() const;
		/**
		 * Public method to get the geographic longitude of the balloon at the time the event was recorded.
		 * 
		 * @return A float with the longitude in degrees
		 * */
		float GetLongitude() const;
		/**
		 * Public method to get the altitude of the balloon at the time the event was recorded.
		 * 
		 * @return An int with the altitude in meters
		 * */
		int GetAltitude() const;
		/**
		 * Public method to get the azimuth of the Sun's location in degrees
		 * 
		 * @return A float with the azimuth in degrees
		 * */
		float GetSunAzimuth() const;
		/**
		 * Public method to get the elevation of the Sun's location in degrees
		 * 
		 * @return A float with the elevation in degrees
		 * */
		float GetSunElevation() const;
		/**
		 * Public method to get the azimuth of the Moon's location in degrees
		 * 
		 * @return A float with the azimuth in degrees
		 * */
		float GetMoonAzimuth() const;
		/**
		 * Public method to get the elevation of the Moon's location in degrees
		 * 
		 * @return A float with the elevation in degrees
		 * */
		float GetMoonElevation() const;
		/**
		 * Public method to get the horizon as seen from the balloon in degrees
		 * 
		 * @return A float with the horizon location in degrees
		 * */
		float GetHorizon() const;
		/**
		 * Public method to get the azimuth of the telescope's orientation in degrees
		 * 
		 * @return A float with the azimuthal orientation in degrees
		 * */
		float GetAzimuth() const;
		/**
		 * Public method to get the number of triggered events in the run
		 * 
		 * @return An int with the number of triggered events
		 * */
		int GetTrigEvent() const;
		/**
		 * Public method to get if the temperature of in the log is of the microController or the SiPMs
		 * (deprecated)
		 * 
		 * @return An int switch
		 * */
		int GetTempFlag() const;
		/**
		 * Public method to get the emon1 reading
		 * 
		 * @return An int with the emon reading in pW
		 * */
		int GetEmon1() const;
		/**
		 * @see GetEmon1()
		 * */
		int GetEmon2() const;
		/**
		 * Public method to get the std::vector of size 8 (typically) that contains the high voltage setting for each of the 8 channels.
		 * 
		 * @return A std::vector<float> with the high voltage setting in V
		 * */
		const std::vector<float>& Gethv() const;
		/**
		 * Public method to get the std::vector of size 8 (typically) that contains the high voltage current for each of the 8 channels.
		 * 
		 * @return A std::vector<float> with the high voltage current in mA
		 * */
		const std::vector<float>& Gethvc() const;
		/**
		 * Public method to get the std::vector of size 32 (typically) that contains the temperature of the 32 microControllers
		 * 
		 * @return A std::vector<float> with the microController temperature in C
		 * */
		const std::vector<float>& GetUCTemp() const;
		/**
		 * Public method to get the temperature of the CPU
		 * 
		 * @return An int with the CPU temperature in C
		 * */
		int GetCpuTemp() const;
		/**
		 * Public method to get the temperature of the CoBo
		 * 
		 * @return An int with the CoBo temperature in C
		 * */
		int GetCoboTemp() const;
		/**
		 * Public method to get the temperature of the radiator
		 * 
		 * @return An int with the radiator temperature in C
		 * */
		int GetRadTemp() const;
		/**
		 * Public method to get the output voltage of the LVPS
		 * 
		 * @return A float with the output voltage in V
		 * */
		float GetlvpsVol() const;
		/**
		 * Public method to get the output voltage to the pumps
		 * 
		 * @return A float with the output voltage in V
		 * */
		float GetpumpVol() const;
		/**
		 * Public method to get the output current of the LVPS
		 * 
		 * @return An int with the output current in mA
		 * */
		int GetlvpsCur() const;
		/**
		 * Public method to get the output voltage of the pumps
		 * 
		 * @return An int with the current in mA
		 * */
		int GetpumpCur() const;
		/**
		 * Public method to get the switch setting for each of the 64 MUSICs
		 * 
		 * @return A std::vector<int> with the MUSIC power switch
		 * */
		const std::vector<int>& GetsiabMPWR() const;
		/**
		 * Public method to get the HV switch setting for each of the 32 SiPM matrices
		 * 
		 * @return A std::vector<int> with the HV power switch
		 * */
		const std::vector<int>& GethvSW() const;
		/**
		 * Public method to get the temperature of the SiPMs
		 * 
		 * @return An std::vector<float> with the temperature of the 32 SiPMs in C
		 * */
		const std::vector<float>& GetSiPMTemp() const;
		/**
		 * Public method to obtain the telescope tilt pointing angle after voltage sag corrections
		 * 
		 * @return float with the angle from horizontal in degrees
		 * */
		float GetTiltAngle() const;
		/**
		 * Public method to obtain the telescope tilt pointing angle without any corrections
		 * 
		 * @return float with the angle from horizontal in degrees
		 * */
		float GetTiltAngleRaw() const;
		/**
		 * Public method to find the closest time stamp position in the time vector
		 * 
		 * @param timeArray A std::vector containing all the time stamps from the file
		 * @param eventTime An unsigned long long containing the time when the event was recorded in 10s of nano seconds.
		 * @return An int with the position of the closest time stamp
		 * */
		static int FindClosestTimestampIndex(std::vector<unsigned long long> timeArray, unsigned long long eventTime);
		/**
		 * Public method to recover the run number to which the event belongs to
		 * 
		 * @return An int with the run number
		 * */
		int GetRunNumber() const;
		/**
		 * Public method to set the runNumber member variable
		 * 
		 * @param n An int with the run number
		 * */
		void SetRunNumber(int n);
		/**
		 * Public method to set revised Trigger Time
		 * 
		 * @param tTime an unsigned long long for revised trigger time
		 * */
		void SetRevTimeTB(unsigned long long tTime);
		/**
		 * Public method to recover the revised TB Time to which the event belongs to
		 * 
		 * @return An unsigned long long with the revised time
		 * */
		unsigned long long GetRevTimeTB() const;
	private:
		/**
		 * Public method to find the closest time stamp in the data vector 
		 * @see SetParametersFromTimestamp
		 * 
		 * @param data A std::vector containing all the information from the auxilliary data file
		 * @param timestamp An int with the time the event was recorded in seconds after some reference (May 13th)
		 * @return An int with the closest time stamp
		 * */
    	int findClosestTimestamp(const std::vector<std::vector<std::string>>& data, int timestamp) const;

};
#endif
