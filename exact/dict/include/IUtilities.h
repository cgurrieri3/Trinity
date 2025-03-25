#ifndef IUTILITIES_H
#define IUTILITIES_H

#include <iostream>
#include <ctime>
#include <iomanip>
#include <vector>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <dirent.h>
#include <chrono>


class IUtilities{
	public:
		/**
		 * \class IUtilities
		 * \brief A class containing methods used in aiding analysis
		 * 
		 * */

		/**
		 * Public constructor
		 * */
		IUtilities();
		/**
		 * Public destructor
		 * */
		~IUtilities();

		/**
		 * Public method to convert UTC time to Unix epoch
		 * 
		 * @param utcTimeStr A time string containing the date and time to be converted
		 * 
		 * @return An unsigned 64 bit number containing the UNIX epoch
		 * */
		static uint64_t ConvertUTCtoUnix(const std::string& utcTimeStr);

		/**
		*  Public method to convert Unix time fro TBtime to UTC time
		*  the input is a 18 digit number pulled from the file time. 
		*   Output is a string of date and time in the format: 
		*									"2023-09-13 14:30:00.00000".
		**/
		static std::string ConvertUnixToUTCfromFile(long long unixTimestampInNanoseconds);




		/**
		 * Public method to obtain all the files with a certain extension in a directory.
		 * 
		 * @param dirname A string with the path to the directory
		 * 
		 * Optional parameters:
		 * @param extension A string with the extension of files to accept. Default is ".root"
		 * 
		 * @return A std::vector of std::strings for each of the files in the directory
		 * */
		static std::vector<std::string> GetFilesInDirectory(std::string dirname, std::string extension=".root");

		/**
		 * Public method to find the Pixel ID (assuming the amplitude vector is sorted in ascending pixelID number) of the maximum number in a std::vector. If musicID is not equal -1, the maximum is searched for only in the 8 channels connected to that MUSIC.
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param musicID An int specifying which MUSIC chip to analyze. Default is -1, the maximum is searched for in all of the camera
		 * 
		 * @return int With the pixel ID
		 * */
		static int GetMaximumPixelID(std::vector<float> amplitude, int musicID=-1);
		/**
		 * Public method to find the Pixel ID for the maximum number in a std::vector within the region of interest as described in Event.
		 * @see Event()
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param roiPixels A std::vector with the pixel IDs describing the ROI. If not specified, the maximum is searched for int the whole amplitde vector. Default is 0.
		 * 
		 * @return int With the pixel ID
		 * */
		static int GetMaximumPixelID(std::vector<float> amplitude, std::vector<int> roiPixels=std::vector<int>(0));
		/**
		 * Public method to find the maximum number in a std::vector. If musicID is not equal -1, the maximum is searched for only in the 8 channels connected to that MUSIC.
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param musicID An int specifying which MUSIC chip to analyze. Default is -1, the maximum is searched for in all of the camera
		 * 
		 * @return float containing the maximum value
		 * */
		static float GetMaximum(std::vector<float> amplitude, int musicID=-1);/**
		 * Public method to find the maximum number in a std::vector within the region of interest as described in Event.
		 * @see Event()
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param roiPixels A std::vector with the pixel IDs describing the ROI. If not specified, the maximum is searched for int the whole amplitde vector. Default is 0.
		 * 
		 * @return float containing the maximum value
		 * */
		static float GetMaximum(std::vector<float> amplitude, std::vector<int> roiPixels=std::vector<int>(0));
		/**
		 * Public method to find the Pixel ID of the minimum number in a std::vector. If musicID is not equal -1, the maximum is searched for only in the 8 channels connected to that MUSIC.
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param musicID An int specifying which MUSIC chip to analyze. Default is -1, the maximum is searched for in all of the camera
		 * 
		 * @return int With the pixel ID
		 * */
		static int GetMinimumPixelID(std::vector<float> amplitude, int musicID=-1);
		/**
		 * Public method to find the Pixel ID for the minimum number in a std::vector within the region of interest as described in Event.
		 * @see Event()
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param roiPixels A std::vector with the pixel IDs describing the ROI. If not specified, the maximum is searched for int the whole amplitde vector. Default is 0.
		 * 
		 * @return int With the pixel ID
		 * */
		static int GetMinimumPixelID(std::vector<float> amplitude, std::vector<int> roiPixels=std::vector<int>(0));
		/**
		 * Public method to find the minimum number in a std::vector. If musicID is not equal -1, the maximum is searched for only in the 8 channels connected to that MUSIC.
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param musicID An int specifying which MUSIC chip to analyze. Default is -1, the maximum is searched for in all of the camera
		 * 
		 * @return float containing the minimum value
		 * */
		static float GetMinimum(std::vector<float> amplitude, int musicID=-1);
		/**
		 * Public method to find the minimum number in a std::vector within the region of interest as described in Event.
		 * @see Event()
		 * 
		 * @param amplitude A std::vector containing amplitude or charge values for all of the camera
		 * 
		 * Optional parameter:
		 * @param roiPixels A std::vector with the pixel IDs describing the ROI. If not specified, the maximum is searched for int the whole amplitde vector. Default is 0.
		 * 
		 * @return float containing the minimum value
		 * */
		static float GetMinimum(std::vector<float> amplitude, std::vector<int> roiPixels=std::vector<int>(0));
		/**
		 * Public method to calculate the pedestal based on the algorithm developed for uncorrelated samples in a trace
		 * The average of all samples in the specified time window is calculated. If isUnCorrelated is true, then the following pad samples are skipped. The process is repeated for the length of the trace std::vector.\ The std::vector provided should only include pedestal samples and not signal.
		 * 
		 * @param trace A std::vector containing the trace of the pixel to be analyzed
		 * 
		 * Optional parametes:
		 * @param tWindow The size of the window in 10s of ns to use. Default is 20 samples.
		 * @param pad The number of samples to skipa after each time window
		 * @param isUnCorrelated A bool specifying whether to skip the pad number of samples or not
		 * 
		 * @return A float with the average of the different window samples
		 * */
		static float GetPedestalAverage(std::vector<int> trace, int tWindow=20,int pad=10, bool isUnCorrelated=true);

		/**
		 * Public method to obtain the High Voltage channel to which the pixel ID is connected
		 * 
		 * @param pixelID An int with the pixel ID
		 * 
		 * @return An int with the HV channel the pixel is connected to
		 * */
		static int GetHVChannel(int pixelID); 

		/**
		 * Public method to linearly interpolate between 2 data points.
		 * The linear interpolation is done the following way
		 * 
		 * f(x) = ((y2-y1)/(x2-x1))*x + (f(x1)-((y2-y1)/(x2-x1))*x1)
		 * 
		 * @param x1 A double of the x coordinate of the first point
		 * @param y1 A double of the y coordinate of the first point
		 * @param x2 A double of the x coordinate of the second point
		 * @param y2 A double of the y coordinate of the second point
		 * @param xNew A double of the x coordinate of the point to find, between x1 and x2. (There is no strict need for xNew to be within the range, but the result will not be an interpolation but an extrapolation.)
		 * 
		 * @return A double with the y coordinate corresponding to xNew
		 * */
		static double Interpolate(double x1, double y1, double x2, double y2, double xNew);
	
};

#endif
