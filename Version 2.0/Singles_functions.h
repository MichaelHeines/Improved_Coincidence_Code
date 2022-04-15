#ifndef singlesfunctions_H
#define singlesfunctions_H

#include <iostream>
#include <vector>

#include <TROOT.h>
#include <TSpectrum.h>

#include "File_chainer.h"


// Function that estimates the background on a spectrum
void populate_background(TH1F* histogram, TH1F* bg_histogram, Int_t xmax){
	//histogram->Draw("L");
	TSpectrum* s = new TSpectrum();
	Double_t* source = new Double_t[xmax];
	for (int i = 0; i < xmax; i++) source[i] = histogram->GetBinContent(i + 1);
	s->Background(source, xmax, 4, 1, 0, kFALSE, 3, kTRUE);
	for (int i = 0; i < xmax; i++) bg_histogram->SetBinContent(i + 1, source[i]);
	bg_histogram->SetLineColor(kRed);
	//bg_histogram->Draw("SAME L");
	//gPad->SetLogy();
	//histogram->SetXTitle("Energy (channels)");
	//histogram->SetYTitle("Counts");
}


// Create spectra for the single gammas
void create_single_spectrum(std::vector<TString> singles_filename, TString read_dir, TString write_dir, Int_t nr_datapoints, TString data_type_chain, TString data_type_tree, Int_t hist_bins) {
	TChain* mytree = new TChain(data_type_chain); 
	add_files_to_chain(mytree, data_type_tree, ".root", read_dir);					// Get relevant data into chain

	TBranch* Energy_branch = mytree->GetBranch("Energy");						// Assign the branches
	TBranch* Channel_branch = mytree->GetBranch("Channel");
	TBranch* Time_branch = mytree->GetBranch("Timestamp");
	UShort_t energy;										// Define the datatypes of the leaves
	UShort_t channel;
	ULong64_t time;
	mytree->SetBranchAddress("Energy", &energy);							// Sets the branches to a variable
	mytree->SetBranchAddress("Channel", &channel);
	mytree->SetBranchAddress("Timestamp", &time);
	const Int_t entry = mytree->GetEntries();							// Number of entries in the root file
	Int_t cutted_events = 3;									// Number of events cutted for coincidences
	Double_t range_low, range_high;

	for (Double_t j = 0; j < nr_datapoints; j++) {
		TFile* hfile = TFile::Open(write_dir + singles_filename[j], "RECREATE");
		TH1F* h0 = new TH1F("h0", "Energy spectrum_0", hist_bins, 0, hist_bins);		// Initialize histogram and canvas
		TH1F* h1 = new TH1F("h1", "Energy spectrum_1", hist_bins, 0, hist_bins);
		TH1F* bg0 = new TH1F("bg0", "The background of histo channel 0", hist_bins, 0, hist_bins);
		TH1F* bg1 = new TH1F("bg1", "The background of histo channel 1", hist_bins, 0, hist_bins);

		range_low = cutted_events + round(j * entry / nr_datapoints);
		range_high = round((j + 1) * entry / nr_datapoints) - cutted_events;

		for (Int_t i = range_low; i < range_high; i++) {					// Iterate over leaves
			mytree->GetEntry(i);								// Get a leaf in all of the branches

			if (channel == 0) {								// Fill the energy leave into the correct histogram
				h0->Fill(energy);
			}
			else if (channel == 1) {
				h1->Fill(energy);
			}
		}

		TH1F* h0_peaks = (TH1F*)h0->Clone("h0");						// Obtain background subtracted histograms
		TH1F* h1_peaks = (TH1F*)h1->Clone("h1");
		populate_background(h0, bg0, hist_bins);
		populate_background(h1, bg1, hist_bins);
		h0_peaks->Add(bg0, -1);
		h1_peaks->Add(bg1, -1);

		h0->Write("h0", TObject::kOverwrite);
		h1->Write("h1", TObject::kOverwrite);
		bg0->Write("bg0", TObject::kOverwrite);
		bg1->Write("bg1", TObject::kOverwrite);
		h0_peaks->Write("h0_peaks", TObject::kOverwrite);
		h1_peaks->Write("h1_peaks", TObject::kOverwrite);
		hfile->TFile::Close("R");
	}

	std::cout << "Wrote single countrate histograms" << std::endl;
}



//calculates integral of bins
ULong64_t bin_integral(Double_t energy, TH1F* histogram, Int_t hwidth) {
	Int_t peak = round(energy);
	histogram->GetXaxis()->SetRangeUser(peak - hwidth, peak + hwidth);
	Int_t lower = peak - hwidth;
	Int_t upper = peak + hwidth;
	ULong64_t integral;

	integral = histogram->Integral(histogram->FindBin(lower), histogram->FindBin(upper));
	//histogram->Draw();

	return integral;
}



std::vector<Double_t> get_singlerates(TString directory, TString filename, std::vector<Double_t> channels0, std::vector<Double_t> channels1, std::vector<Int_t> hwidth) {
	TFile* f = new TFile(directory + filename);								// Load the root file
	TH1F* h0_peaks; f->GetObject("h0_peaks", h0_peaks);
	TH1F* h1_peaks; f->GetObject("h1_peaks", h1_peaks);

	std::vector<Double_t> counts0(2), counts1(2);

	for (Int_t i = 0; i < 2; i++) {										// Gets the bin count rates for one detector
		ULong64_t bin_I0 = bin_integral(channels0[i], h0_peaks, hwidth[i]);
		counts0[i] = bin_I0;
		ULong64_t bin_I1 = bin_integral(channels1[i], h1_peaks, hwidth[i]);
		counts1[i] = bin_I1;
	}
	f->TFile::Close("R");

	return { counts0[0], counts0[1], counts1[0], counts1[1] };
}

#endif
