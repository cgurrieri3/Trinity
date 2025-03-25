#include <string>
#include <sstream>

#include "IUtilities.h"

IUtilities::IUtilities(){

}
IUtilities::~IUtilities(){

}

uint64_t IUtilities::ConvertUTCtoUnix(const std::string& utcTimeStr) {
    // Convert UTC time string to tm structure
    std::tm utcTime = {};
    std::istringstream ss(utcTimeStr);
    ss >> std::get_time(&utcTime, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        std::cerr << "Invalid UTC time format" << std::endl;
        return 0;
    }

    // Convert tm structure to Unix timestamp
    std::time_t unixTime = std::mktime(&utcTime);

    // Extract microseconds and convert to Unix timestamp format
    int microsec;
    ss >> microsec;

    // Calculate Unix timestamp with microseconds
    uint64_t unixTimestamp = static_cast<uint64_t>(unixTime) * 1000000ULL + microsec;

    return unixTimestamp;
}

std::string IUtilities::ConvertUnixToUTCfromFile(long long unixTimestampInNanoseconds) {

    // Add an extra "0" at the end
    unixTimestampInNanoseconds *= 10;

    std::chrono::system_clock::time_point tp = std::chrono::time_point<std::chrono::system_clock>(
        std::chrono::nanoseconds(unixTimestampInNanoseconds));

    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::chrono::system_clock::duration sinceEpoch = tp.time_since_epoch();
    std::chrono::nanoseconds ns = std::chrono::duration_cast<std::chrono::nanoseconds>(sinceEpoch);

    // Calculate the hours, minutes, and seconds
    int hours = ns.count() / 3600000000000;
    int minutes = (ns.count() % 3600000000000) / 60000000000;
    int seconds = (ns.count() % 60000000000) / 1000000000;
    int nanoseconds = ns.count() % 1000000000;


	std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S")
        << "." << std::setw(9) << std::setfill('0') << nanoseconds;

    return oss.str();
}


int IUtilities::GetMaximumPixelID(std::vector<float>amplitude,int musicID){
	int iStartPixel, iEndPixel;
	//looks for pixels in specified music ID if none start from pixel#0
	if(musicID != -1){
		iStartPixel = musicID*8;
		iEndPixel = musicID*8+8;
	}else{
		iStartPixel = 0;
		iEndPixel = amplitude.size();
	}

	int max = 0;
	int maxID = 0;
	//looks for the pixel with highest amplitude
	for(int i = iStartPixel; i<iEndPixel; i++){
		if(amplitude[i]>max){
			max = amplitude[i];
			maxID = i;
		}
	}

	return maxID;
}

int IUtilities::GetMaximumPixelID(std::vector<float>amplitude,std::vector<int> roiPixels){
	int iStartPixel, iEndPixel;


	int max = 0;
	int maxID = 0;
	//If no region of interest pixels specified look for highest amplitude out of pixels starting from pixel#0 -> amplitude.size()
	if(roiPixels.size() == 0){
		iStartPixel = 0;
		iEndPixel = amplitude.size();

		for(int i = iStartPixel; i<iEndPixel; i++){
			if(amplitude[i]>max){
				max = amplitude[i];
				maxID = i;
			}
		}
	//if roi pixels specified looks for pixel with highest amplitude
	}else{
		for(int i = 0; i<roiPixels.size(); i++){
			if(amplitude[roiPixels[i]]>max){
				max = amplitude[roiPixels[i]];
				maxID = roiPixels[i];
			}
		}
	}
	return maxID;
}

float IUtilities::GetMaximum(std::vector<float>amplitude,int musicID){
	int iStartPixel, iEndPixel;
	//if music ID is none  start from music #0
	if(musicID != -1){
		iStartPixel = musicID*8;
		iEndPixel = musicID*8+8;
	}else{
		iStartPixel = 0;
		iEndPixel = amplitude.size();
	}
	//find max amplitude out of vector
	float max = 0;
	int maxID = 0;
	for(int i = iStartPixel; i<iEndPixel; i++){
		if(amplitude[i]>max){
			max = amplitude[i];
			maxID = i;
		}
	}

	return max;
}

float IUtilities::GetMaximum(std::vector<float>amplitude,std::vector<int> roiPixels){
	int iStartPixel, iEndPixel;


	float max = 0;
	int maxID = 0;
	//if no roiPixels specified starts sequentially from pixel#0 -> pixel#amplitde.size()
	//returns max amplitude
	if(roiPixels.size() == 0){
		iStartPixel = 0;
		iEndPixel = amplitude.size();

		for(int i = iStartPixel; i<iEndPixel; i++){
			if(amplitude[i]>max){
				max = amplitude[i];
				maxID = i;
			}
		}
	}else{
		for(int i = 0; i<roiPixels.size(); i++){
			if(amplitude[roiPixels[i]]>max){
				max = amplitude[roiPixels[i]];
				maxID = roiPixels[i];
			}
		}
	}
	return max;
}

int IUtilities::GetMinimumPixelID(std::vector<float>amplitude,int musicID){
	int iStartPixel, iEndPixel;
	
	//looks for pixels in specified music ID
	if(musicID != -1){
		iStartPixel = musicID*8;
		iEndPixel = musicID*8+8;
	}else{
		iStartPixel = 0;
		iEndPixel = amplitude.size();
	}
	//returns min amplitude ID out of pixels in music
	float min = 4096;
	int minID = 0;
	for(int i = iStartPixel; i<iEndPixel; i++){
		if(amplitude[i]<min){
			min = amplitude[i];
			minID = i;
		}
	}

	return minID;
}

int IUtilities::GetMinimumPixelID(std::vector<float>amplitude,std::vector<int> roiPixels){
	int iStartPixel, iEndPixel;

	//if no roiPixels specified starts sequentially from pixel#0 -> pixel#amplitude.size()

	//returns min amplitude ID out of pixels in roiPixels vector
	float min = 4096;
	int minID = 0;

	if(roiPixels.size() == 0){
		iStartPixel = 0;
		iEndPixel = amplitude.size();

		for(int i = iStartPixel; i<iEndPixel; i++){
			if(amplitude[i]<min){
				min = amplitude[i];
				minID = i;
			}
		}
	}else{
		for(int i = 0; i<roiPixels.size(); i++){
			if(amplitude[roiPixels[i]]<min){
				min = amplitude[roiPixels[i]];
				minID = roiPixels[i];
			}
		}
	}
	return minID;
}

float IUtilities::GetMinimum(std::vector<float>amplitude,int musicID){
	int iStartPixel, iEndPixel;

	//looks for pixels in specified music ID if none start from pixel 0	
	if(musicID != -2){
		iStartPixel = musicID*8;
		iEndPixel = musicID*8+8;
	}else{
		iStartPixel = 0;
		iEndPixel = amplitude.size();
	}

	float min = 4096;
	int minID = 0;
	//returns min amplitude out of pixels in music
	for(int i = iStartPixel; i<iEndPixel; i++){
		if(amplitude[i]<min){
			min = amplitude[i];
			minID = i;
		}
	}

	return min;
}

float IUtilities::GetMinimum(std::vector<float>amplitude,std::vector<int> roiPixels){
	int iStartPixel, iEndPixel;

	//if no roiPixels specified starts sequentially from pixel#0 -> pixel#amplitde.size()
	//returns min amplitude out of pixels in roiPixels vector

	float min = 4096;
	int minID = 0;

	if(roiPixels.size() == 0){
		iStartPixel = 0;
		iEndPixel = amplitude.size();

		for(int i = iStartPixel; i<iEndPixel; i++){
			if(amplitude[i]<min){
				min = amplitude[i];
				minID = i;
			}
		}
	}else{
		for(int i = 0; i<roiPixels.size(); i++){
			if(amplitude[roiPixels[i]]<min){
				min = amplitude[roiPixels[i]];
				minID = roiPixels[i];
			}
		}
	}
	return min;
}

float IUtilities::GetPedestalAverage(std::vector<int> trace, int tWindow,int pad, bool isUnCorrelated){
	//create window for which to view trace samples
	std::vector<int> traceWindow = std::vector<int>(0);
	std::vector<int>::iterator itr;
	float average;
	float sum=0;
	int counter = 0;
	//if isUnCorrelated is true, then following pad samples are skipped
	//calculates sum of all trace values in vector
	if(isUnCorrelated){
		for(int i = 0; i+tWindow<trace.size(); i+=(tWindow+pad)){
			traceWindow = std::vector<int>(trace.begin()+i,trace.begin()+i+tWindow);
			sum += std::accumulate(traceWindow.begin(),traceWindow.end(),0);
			counter++;
			traceWindow.clear();
		}
	//same as above expect pad samples not skipped
	}else{
		sum = std::accumulate(trace.begin(),trace.end(),0);
		counter = 1;
		tWindow = trace.size();
	}
	//returns average trace value
	average = sum/(float)(counter*tWindow);
	return average;
}


std::vector<std::string> IUtilities::GetFilesInDirectory(std::string dirname, std::string extension){
	//looks through all files in directory with given extension
	std::vector<std::string> files;
	std::string fileName;
	DIR *dir = opendir(dirname.c_str());

	if(dir){
		dirent *element;
		while ((element = readdir(dir))){
			fileName = element->d_name;
			if(fileName.length()>=extension.length()){
				if(fileName.compare(fileName.length() - extension.length(),extension.length(),extension) == 0){
					files.push_back(fileName);
				}
			}
		}

	}
	return files;
}

int IUtilities::GetHVChannel(int pixelID){
	// given pixelID calculates hVChannel # bottom left 1, top left 2, bottom right 3, top right 4
	int hVChannel;
	if ((pixelID >= 0 && pixelID <= 31) || (pixelID >= 64 && pixelID <= 95)) {
		hVChannel = 1;
    } else if ((pixelID >= 128 && pixelID <= 159) || (pixelID >= 192 && pixelID <= 223)) {
		hVChannel = 2;
	}else if ((pixelID >= 32 && pixelID <= 63) || (pixelID >= 96 && pixelID <= 127)) {
		hVChannel = 3;
	}else if ((pixelID >= 160 && pixelID <= 191) || (pixelID >= 224 && pixelID <= 255)) {
		hVChannel = 4;
    }
	return hVChannel;
}

double IUtilities::Interpolate(double x1, double y1, double x2, double y2, double xNew){
//uses linear interpolation between data points and returns slopes
	double slope = (y2 - y1)/(x2 - x1);
    double offset = y2 - x2*slope;

   return slope*xNew + offset;
}
