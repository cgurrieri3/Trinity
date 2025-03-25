R__LOAD_LIBRARY(libExACT.so)
#include <iostream>

int AddSiPMInfoToEvents(std::string dataFile, std::string calibFile, std::string measurementFile){
	IFile *f = new IFile(dataFile,"UPDATE");

	ICalibration *calib = new ICalibration(calibFile);
	IEvent *ev = new IEvent();
	ISiPM *sipmInfo = new ISiPM(512);

	//cout<<"Here"<<endl;

	f->treeHLED->SetBranchAddress("Events", &ev);
	TBranch *sipmBranch = f->treeHLED->Branch("SiPM","ISiPM",&sipmInfo,64000,99);
	
	std::vector<float> hv;
	std::vector<float> sipmTemp;


	std::vector<double> absoluteGain = std::vector<double>(512);
	std::vector<double> optXTalk = std::vector<double>(512);
	std::vector<double> tempCorrection = std::vector<double>(512);
	std::vector<double> amplitudeToPE = std::vector<double>(512);
	std::vector<double> chargeToPE = std::vector<double>(512);


	double hvSettingPx;
	double relOverV;
	double overV;
	//cout<<"Pixel\tGain\tTCorection\tXTalk\tAmpl/PE\tChg/PE\tRel Ovr V\tTemp\tHV"<<endl;
	for(int i = 0; i<f->treeHLED->GetEntries(); i++){
		f->treeHLED->GetEntry(i);
		hv = ev->Gethv();
		sipmTemp = ev->GetSiPMTemp();
		//cout<<"Loop"<<endl;
		for(int j = 0; j<512; j++){

			//cout<<"i"<<endl;
			hvSettingPx = hv[IUtilities::GetHVChannel(j)];
			relOverV = ICalibration::GetRelativeOverVoltage(j,hvSettingPx,44);
			overV = ICalibration::GetOverVoltage(j,hvSettingPx);
			tempCorrection[j] = ICalibration::GetRelativeGain(j,sipmTemp[j/16],hvSettingPx);

			//cout<<"Arr"<<endl;
			
			absoluteGain[j] = 1E6*ICalibration::GetMeasurementFromFile(j,sipmTemp[j/16],hvSettingPx, measurementFile,"GAIN","ABS");
			
			//cout<<"Hang"<<endl;
			optXTalk[j] = 0.01*ICalibration::GetMeasurementFromFile(j,sipmTemp[j/16],hvSettingPx, measurementFile,"OC","REL");
			
			amplitudeToPE[j] = calib->GetAmplitude(j)*relOverV;
			chargeToPE[j] = calib->GetCharge(j)*relOverV;	

			//cout<<j<<"\t"<<absoluteGain[j]<<"\t"<<tempCorrection[j]<<"\t"<<optXTalk[j]<<"\t"<<amplitudeToPE[j]<<"\t"<<chargeToPE[j]<<"\t"<<relOverV<<"\t"<<sipmTemp[j/16]<<"\t"<<hvSettingPx<<"\t"<<overV<<endl;
		}
		if(i%100 == 0){
			cout<<"Processed Events: "<<i<<endl;
		}
		sipmInfo->SetGain(absoluteGain);
		sipmInfo->SetOptXTalk(optXTalk);
		sipmInfo->SetTCorrection(tempCorrection);
		sipmInfo->SetAmplToPE(amplitudeToPE);
		sipmInfo->SetChgToPE(chargeToPE);

		sipmBranch->Fill();


		
	}
	f->ITFile->Write();
	f->ITFile->Close();

	cout<<"Added Branch SiPM"<<endl;
	return 0;
}