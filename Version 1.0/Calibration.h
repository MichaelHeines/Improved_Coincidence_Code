#ifndef calibration_H
#define calibration_H

#include <vector>
#include <iostream>

std::vector<Double_t> calibration(TH1F* histo, std::vector<Double_t> calibs, std::vector<Int_t> ranges) {
	// Hisograms used in this function should be put through background subtraction first
	std::vector<Double_t> peaks(3);
	std::vector<Double_t> CtoE(2);
	Int_t binmax;

	// Get the values corresponding to the maximal bins within set ranges
	histo->GetXaxis()->SetRangeUser(ranges[0], ranges[1]);
	binmax = histo->GetMaximumBin();
	peaks[0] = histo->GetXaxis()->GetBinCenter(binmax);
	histo->GetXaxis()->SetRangeUser(ranges[2], ranges[3]);
	binmax = histo->GetMaximumBin();
	peaks[1] = histo->GetXaxis()->GetBinCenter(binmax);
	histo->GetXaxis()->SetRangeUser(ranges[4], ranges[5]);
	binmax = histo->GetMaximumBin();
	peaks[2] = histo->GetXaxis()->GetBinCenter(binmax);

	//histo->Draw();
	// Formulas for linear regression
	Double_t sumy = peaks[0] + peaks[1] + peaks[2];
	Double_t sumx = calibs[0] + calibs[1] + calibs[2];
	Double_t sumxy = peaks[0] * calibs[0] + peaks[1] * calibs[1] + peaks[2] * calibs[2];
	Double_t sumx2 = calibs[0]* calibs[0] + calibs[1]* calibs[1] + calibs[2]* calibs [2];
	Double_t n = 3;
	CtoE[0] = (sumx2 * sumy - sumx * sumxy) / (n * sumx2 - sumx * sumx);										// intercept
	CtoE[1] = (n * sumxy - sumx * sumy) / (n * sumx2 - sumx * sumx);											// slope
	
	return CtoE;
}



Double_t get_channel(std::vector<Double_t> calibs, Double_t energy) {
	Double_t channel;
	channel = calibs[0] + energy * calibs[1];

	return channel;
}



std::vector<Double_t> calibrate(TString directory, TString filename, std::vector<Double_t> calibrations, std::vector<Int_t> range) {
	// Make quick calibrations for both detectors, accurate to about 1 Channel
	TFile* f = new TFile(directory + filename);									// Load the root file
	TH1F* h0; f->GetObject("h0_peaks", h0);
	TH1F* h1; f->GetObject("h1_peaks", h1);
	std::vector<Double_t> calibrate_out(4);

	std::vector<Double_t> cal0 = calibration(h0, calibrations, range);
	std::vector<Double_t> cal1 = calibration(h1, calibrations, range);
	calibrate_out = { cal0[0], cal0[1] , cal1[0], cal1[1] };

	return calibrate_out;
}

#endif
