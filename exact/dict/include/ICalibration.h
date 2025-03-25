#ifndef ICALIBRATION_H
#define ICALIBRATION_H

#include "IUtilities.h"

#include <TROOT.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

const double BrVperDeg = 0.037;/**< Breakdown Voltage change per degree C*/
const double BrVol[32] = {39.3884, 38.9061, 38.7750, 39.4964, 38.9802, 37.9820, 39.2063, 38.8830,
							39.2391, 37.3280, 38.7265, 38.2766, 38.9536, 39.1507, 38.8840, 40.3128, 
							38.4516, 38.1072, 38.3544, 36.9768, 38.5418, 37.1179, 39.0720, 38.8999,
							38.2774, 37.5842, 39.1776, 38.8899, 38.4792, 39.7979, 39.1601, 38.7190};/**< Breakdown voltages fore each of the matrices in EUSO-SPB2*/

const double VOffsetPx[512] = {633.6, 640, 534.4, 569.6, 908.8, 662.4, 611.2, 681.6, 768, 729.6, 710.4, 723.2, 771.2, 758.4, 848, 816, 576, 614.4, 521.6, 640, 617.6, 620.8, 620.8, 636.8, 675.2, 560, 640, 656, 630.4, 604.8, 665.6, 627.2, 867.2, 880, 828.8, 880, 860.8, 825.6, 816, 732.8, 947.2, 851.2, 912, 880, 876.8, 908.8, 944, 931.2, 838.4, 739.2, 816, 774.4, 828.8, 720, 771.2, 825.6, 800, 764.8, 796.8, 854.4, 812.8, 793.6, 713.6, 790.4, 691.2, 681.6, 736, 736, 752, 790.4, 691.2, 716.8, 729.6, 707.2, 688, 691.2, 704, 716.8, 774.4, 816, 681.6, 704, 620.8, 748.8, 672, 739.2, 732.8, 697.6, 771.2, 752, 806.4, 748.8, 675.2, 774.4, 812.8, 723.2, 601.6, 579.2, 540.8, 627.2, 592, 595.2, 588.8, 675.2, 745.6, 681.6, 710.4, 656, 739.2, 774.4, 825.6, 864, 761.6, 672, 752, 816, 620.8, 691.2, 643.2, 652.8, 841.6, 806.4, 774.4, 886.4, 816, 835.2, 768, 723.2, 828.8, 844.8, 902.4, 611.2, 812.8, 816, 816, 752, 816, 816, 704, 940.8, 816, 691.2, 950.4, 748.8, 816, 713.6, 774.4, 659.2, 851.2, 838.4, 787.2, 816, 668.8, 656, 678.4, 675.2, 742.4, 947.2, 704, 668.8, 950.4, 816, 816, 816, 816, 816, 816, 924.8, 828.8, 880, 902.4, 832, 873.6, 844.8, 790.4, 793.6, 723.2, 742.4, 752, 585.6, 787.2, 726.4, 780.8, 710.4, 604.8, 614.4, 576, 601.6, 636.8, 636.8, 713.6, 710.4, 435.2, 364.8, 380.8, 576, 694.4, 467.2, 412.8, 630.4, 550.4, 457.6, 518.4, 816, 816, 457.6, 528, 816, 851.2, 915.2, 745.6, 816, 803.2, 867.2, 864, 873.6, 771.2, 816, 745.6, 774.4, 732.8, 726.4, 816, 777.6, 588.8, 531.2, 534.4, 544, 595.2, 700.8, 633.6, 627.2, 595.2, 579.2, 668.8, 652.8, 691.2, 588.8, 556.8, 569.6, 547.2, 480, 499.2, 505.6, 441.6, 585.6, 544, 835.2, 563.2, 540.8, 502.4, 512, 569.6, 572.8, 569.6, 451.2, 892.8, 905.6, 816, 950.4, 915.2, 816, 937.6, 928, 947.2, 816, 816, 950.4, 816, 816, 940.8, 816, 662.4, 822.4, 787.2, 822.4, 748.8, 768, 633.6, 707.2, 947.2, 867.2, 816, 924.8, 956.8, 876.8, 873.6, 320, 636.8, 675.2, 598.4, 646.4, 588.8, 649.6, 614.4, 633.6, 742.4, 614.4, 672, 595.2, 563.2, 710.4, 678.4, 617.6, 454.4, 512, 457.6, 537.6, 496, 560, 496, 377.6, 534.4, 643.2, 502.4, 486.4, 534.4, 496, 483.2, 480, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 320, 678.4, 611.2, 652.8, 688, 617.6, 464, 652.8, 601.6, 563.2, 643.2, 592, 528, 528, 700.8, 643.2, 691.2, 896, 675.2, 732.8, 684.8, 908.8, 758.4, 793.6, 716.8, 889.6, 726.4, 800, 742.4, 851.2, 928, 902.4, 918.4, 742.4, 752, 784, 828.8, 780.8, 803.2, 812.8, 758.4, 896, 793.6, 953.6, 816, 905.6, 848, 931.2, 953.6, 886.4, 876.8, 816, 816, 880, 816, 816, 816, 860.8, 912, 905.6, 892.8, 851.2, 841.6, 899.2, 844.8, 912, 864, 873.6, 777.6, 825.6, 915.2, 918.4, 816, 736, 694.4, 732.8, 723.2, 726.4, 851.2, 806.4, 841.6, 924.8, 896, 816, 816, 864, 857.6, 774.4, 732.8, 630.4, 739.2, 716.8, 755.2, 713.6, 726.4, 681.6, 748.8, 710.4, 556.8, 547.2, 576, 611.2, 675.2, 723.2, 643.2, 518.4, 534.4, 470.4, 480, 617.6, 601.6, 710.4, 624, 816, 857.6, 793.6, 928, 873.6, 828.8, 880, 851.2, 774.4, 841.6, 825.6, 828.8, 851.2, 825.6, 819.2, 761.6, 771.2, 723.2, 694.4, 678.4, 755.2, 790.4, 784, 732.8, 857.6, 835.2, 886.4, 832, 860.8, 883.2, 880, 755.2, 720, 707.2, 784, 844.8, 726.4, 688, 617.6, 563.2, 534.4, 851.2, 531.2, 480, 800, 518.4, 486.4, 550.4, 880, 646.4, 652.8, 652.8, 854.4, 659.2, 841.6, 947.2, 611.2, 556.8, 537.6, 604.8, 611.2, 624, 659.2, 614.4}/**< Offset adjustments for camera flatfielding in mV*/;

class ICalibration {
	/**
	 * \class ICalibration
	 * \brief Class used to calculate correction factors for each of the SiPMs
	 * 
	 * During the data acquisition 2 things affect the response of the SiPMs, temperature and operating voltage. The calculations to obtain the correct gain and temperature corrections are performed by this class
	 * */
	public:
		/**
		 * Public constructor. Uses a file with the ADC to PE conversion factors and errors for each of the SiPMs as measured in the lab
		 * 
		 * @param filename A string with the file that contains the measurements
		 * */
    	ICalibration(const std::string& filename);
    	/**
    	 * A destructor
    	 * */
    	~ICalibration();

    	/**
    	 * A method used to obtain the conversion factor in ADC to PE for the amplitude of a pulse for a specific pixel
    	 * 
    	 * @param pixelID An int with the ID of the pixel
    	 * 
    	 * @return a double with the conversion from ADC to photo-electrons for amplitude.
    	 * */
	    double GetAmplitude(int pixelID) const;
	    /**
    	 * A method used to obtain the error on the measurement of the conversion factor in ADC to PE for the amplitude of a pulse for a specific pixel
    	 * 
    	 * @param pixelID An int with the ID of the pixel
    	 * 
    	 * @return a double with the error on the measurement of the conversion from ADC to photo-electrons for amplitude.
    	 * */
	    double GetErrAmplitude(int pixelID) const;
	    /**
    	 * A method used to obtain the conversion factor in ADC to PE for the cahrge of a pulse for a specific pixel
    	 * 
    	 * @param pixelID An int with the ID of the pixel
    	 * 
    	 * @return a double with the conversion from ADC to photo-electrons for charge.
    	 * */
	    double GetCharge(int pixelID) const;
	    /**
    	 * A method used to obtain the error on the measurement of the conversion factor in ADC to PE for the charge of a pulse for a specific pixel
    	 * 
    	 * @param pixelID An int with the ID of the pixel
    	 * 
    	 * @return a double with the error on the measurement of the conversion from ADC to photo-electrons for charge.
    	 * */
	    double GetErrCharge(int pixelID) const;
	    /**
	     * A method used to obtain the voltage offset set for the pixel in the camera
	     * 
	     * @param pixelID An int with the pixel ID of which the voltage offset is to be retrieved
	     * 
	     * @return A double with the offset setting in mV
	     * */
	    double GetVOffsetPx(int pixelID);
	     /**
	     * A method used to obtain the voltage offset set for all pixels in the camera
	     * 
	     * @return A std::vector<double> with the offset setting in mV
	     * */
	    std::vector<double> GetVOffsetPx();
	    /**
	     * A method to calculate the relative over voltage compared to the nominal voltage used for the lab measurements
	     * 
	     * The relative over voltage is defined as 
	     * 
	     * R.O.V. = (operatingVoltage - breakdownVoltage)/(nominalVoltage - breakdownVoltage)
	     * 
	     * @param pixelID An int with the pixel ID of interes
	     * @param operatingVol The voltage at which the SiPM was biased in V
	     * @param nominalVol The voltage used in the lab measurements in V
	     * 
	     * @return A double with the R.O.V.
	     * */
	    static double GetRelativeOverVoltage(int pixelID, double operatingVol, double nominalVol);
	    /**
	     * A method to calculate the over voltage of the SiPM
	     * 
	     * The over voltage is defined as 
	     * 
	     * O.V. = (operatingVoltage - breakdownVoltage)/(breakdownVoltage)
	     * 
	     * @param pixelID An int with the pixel ID of interes
	     * @param operatingVol The voltage at which the SiPM was biased in V
	     * 
	     * @return A double with the O.V.
	     * */
	    static double GetOverVoltage(int pixelID, double operatingVol);
	    /**
	     * A method to calculate the relative gain of the SiPM when compared to room temperature at which the lab measurements where performed.
	     * 
	     * The relative gain is defined as follows
	     * 
	     * G(T_Lab)/G(T) = 1/(1-BrVperDeg*((T-T_Lab)/(OperatingVoltage-BreakdownVoltage)))
	     * 
	     * @param pixelID An int with the pixelID of interest
	     * @param sipmTemp A double with the temperature of the SiPM in C
	     * @param OperatingVol A double with the operating voltage in V
	     * 
	     * @return A double with the relativeGain
	     * */
	    static double GetRelativeGain(int pixelID, double sipmTemp, double OperatingVol);
	    /**
	     * 
	     * A method that obtains on which line of a text file the data related to a measurement is present
	     * 
	     * @param filename A string with the file with the measurements for the SiPM
	     * @param measurement A string describing which measurement to extract
	     * 
	     * @return An int with the line number where the measurement appears
	     * */
	   	static int GetMeasurementLine(std::string filename, std::string measurement);
	   	/**
	   	 * A method to obtain a measurement from the file containing all of the measurements for the SiPM.
	   	 * 
	   	 * @param pixelID An int with the pixel ID of interest
	   	 * @param sipmTemp A double with the temperature of the SiPM in C
	   	 * @param OperatingVol A double with the operating voltage in V
	   	 * @param filename A std::string with the name of the file that contains the measurements
	   	 * @param measurement A std::string with the measurement name in the file to extract
	   	 * 
	   	 * Optional parameters:
	   	 * @param biasVoltageRef A std::string describing if the provided OperatingVoltage is the over voltage or absolute ("REL","ABS").\ Default is "ABS"
	   	 * 
	   	 * @return A double with the value of the measurement at that voltage and temperature
	   	 * */
	    static double GetMeasurementFromFile(int pixelID, double sipmTemp, double OperatingVol, std::string filename, std::string measurement, std::string biasVoltageRef="ABS");


	private:
	    std::vector<double> amplitudes;/**< ADC to photo-electron conversion for amplitude*/
	    std::vector<double> errAmplitudes;/**< Error in the ADC to photo-electron conversion for amplitude*/
	    std::vector<double> charges;/**< ADC to photo-electron conversion for charge*/
	    std::vector<double> errCharges;/**< Error in the ADC to photo-electron conversion for charge*/

		/**
		 * A private method to find the closest temperature line in the measurement file to that of the SiPM
		 * 
		 * @param sipmTemp A double with the temperature of the SiPM in C
		 * @param filename A std::string with the name of the measurement file
		 * @param measurement A std::string with the name of the measurement of interest
		 * 
		 * @return An int with the line where the closest temperature is located
		 * */
	    static int GetClosestTemperatureLine(double sipmTemp, std::string filename, std::string measurement);

	   	
	    //double relativeGain;
};

#endif // ICALIBRATION_H
