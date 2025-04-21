#ifndef ISIPM_H
#define ISIPM_H

#include <iostream>
#include <vector>

class ISiPM{
	/**
	 * \class ISiPM
	 * \brief A container class for all the SiPM information for camera calibration
	 * 
	 * These class stores the values needed to calibrate each of the SiPMs in the camera. It contains conversion factors from ADC to PE for both charge and amplitude. 
	 * All the values are based on lab measurements and have been corrected for operating voltage at the time the event was recorded
	 * */
	public:
		/**
		 * Public empty constructor
		 * */
		ISiPM();
		/**
		 * Public constructor using the number of pixels
		 * 
		 * @param nPixels An int with the total number of pixels in the camera
		 * */
		ISiPM(int nPixels);
		/**
		 * Public destructor
		 * */
		~ISiPM();

		/**
		 * Public method to set the gain value for each of the SiPMs in the camera
		 * 
		 * @param values A std::vector of size nPixels that contains the gain values for each SiPM in the camera.
		 * */

		void SetGain(std::vector<double> values);
		/**
		 * Public method to set the optical cross-talk value for each of the SiPMs in the camera
		 * 
		 * @param values A std::vector of size nPixels that contains the optical corss-talk values for each SiPM in the camera.
		 * */
		//void SetOptXTalk(std::vector<double> values);
		/**
		 * Public method to set the temperature correction value for each of the SiPMs in the camera. 
		 * Note: Temperature correction is calculated as a dividing factor, it should be used to divide any of both To PE conversions.
		 * 
		 * @param values A std::vector of size nPixels that contains the temperature correction values for each SiPM in the camera.
		 * */
		void SetTCorrection(std::vector<double> values);
		/**
		 * Public method to set the amplitude in ADC to photo-electron value for each of the SiPMs in the camera
		 * 
		 * @param values A std::vector of size nPixels that contains the ADC to photo-electron values for each SiPM in the camera.
		 * */
		void SetAmplToPE(std::vector<double> values);
		/**
		 * Public method to set the charge in ADC to photo-electron value for each of the SiPMs in the camera
		 * Note: The charge should be calculated using the sum of the peak of the trace, the sample immediately before and the one immediately after.
		 * 
		 * @param values A std::vector of size nPixels that contains the ADC to photo-electron values for each SiPM in the camera.
		 * */
		void SetChgToPE(std::vector<double> values);

		/**
		 * Public method to get the gain as stored in the object
		 * 
		 * @return a std::vector<double> of size nPixels with the gain values
		 * */
		std::vector<double> GetGain();
		/**
		 * Public method to get the optical cross-talk as stored in the object
		 * 
		 * @return a std::vector<double> of size nPixels with the optical cross-talk values
		 * */
		
    //std::vector<double> GetOptXTalk();
		/**
		 * Public method to get the temperature correction as stored in the object
		 * 
		 * @return a std::vector<double> of size nPixels with the temperature correction values
		 * */
		std::vector<double> GetTCorrection();
		/**
		 * Public method to get the amplitude in ADC to photo-electron as stored in the object
		 * 
		 * @return a std::vector<double> of size nPixels with the amplitude in ADC to photo-electron values
		 * */
		std::vector<double> GetAmplToPE();
		/**
		 * Public method to get the charge in ADC to photo-electron as stored in the object
		 * 
		 * @return a std::vector<double> of size nPixels with the charge in ADC to photo-electron values
		 * */
		std::vector<double> GetChgToPE();

	private:
		std::vector <double> gain; /**< A std::vector containing the gain values for all the pixels*/
		//std::vector <double> optXTalk;/**< A std::vector containing the optical cross-talk values for all the pixels*/
		std::vector <double> gainTCorrection;/**< A std::vector containing the temperature correction values for all the pixels*/
		std::vector <double> amplToPE;/**< A std::vector containing the amplitude in ADC to photo-electron values for all the pixels*/
		std::vector <double> chgToPE;/**< A std::vector containing the charge in ADC to photo-electron values for all the pixels*/
};

#endif // ISIPM_H