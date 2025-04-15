// #ifndef CEvent_H
// #define CEvent_H

// #include <TMatrixD.h>
// #include <TMatrixDSym.h>
// #include <TMatrixDSymEigen.h>
// #include <TVectorD.h>
// #include <vector>

// class CEvent { 
//     public:
//         CEvent();

//         // These are set when you call the CEvent
//         // Event GetEvent() const { return ev; }
        
//         // int GetMaxMUSICID() const { return GetMaxMUSICID; }
//         // int GetMaxAmplitudePixelID() const { return MaxAmplitdePixelID; }
//         // double GetAverageAmplitude() const { return AverageAmplitude; }
//         // int GetSumAmplitude() const { return SumAmplitude; }
//         // int GetSaturedPixels() const { return SaturedPixels; }


//         // double GetMaxAmplitude() const { return MaxEventAmplitde; }
//         // int GetPeakTime() const { return GetPeakTime; }

//         // double GetMaxAmplitduePixelIDAtTimeBin() const { return MaxAmplitduePixelIDAtTimeBin; }
//         // double GetMaxAmplitudeAtTimeBin() const { return MaxAmplitudeAtTimeBin; }

//         // // These will return an array with 256 pixels of data

//         // std::vector<double> GetAmplitudeValues() const { return AmplitudeValues; }
//         // std::vector<int> GetPeakTimeValues() const { return PeakTimeValues; }
//         // std::vector<int> GetPedestalValues() const { return PedestalValues; }
//         // std::vector<int> GetTrace() const { return Trace; }
//         // // ADD CALIBRATION SOMEHOW

//         // // These are set when you set them 
//         // // HLED or Test Event
//         // // 
//         // // TH2F hcam_intial
//         // // TH2F hcam_cut
//         // // Surviving Pixel Amplitude array
//         //     // remove duplicates on Get
//         //     // Surviving Pixel ID array
//         // // TH2F connected
//         //     // Bring in Neighbor Pixel Stuff
//         //     // Peak time bins earliest thing
        
//         // // TH2F hcam_cleaned 


//         // TVectorD GetEigenValues() const { return eigenValues; }
//         // TMatrixD GetEigenVectors() const { return eigenVectors; }
//         // std::vector<double> GetSigmas() const;
//         // void AddRow(const std::vector<double>& row);
//         // void ComputeCovarianceMatrix();
//         // void ComputeEigenDecomposition();
    
//     private:
//         int numFeatures;
//         std::vector<std::vector<double>> data;
//         TMatrixD matrix;
//         TMatrixDSym covarianceMatrix;
//         TVectorD eigenValues;
//         TMatrixD eigenVectors;
// };
// #endif
