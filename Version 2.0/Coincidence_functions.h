#ifndef coincidencefunctions_H
#define coincidencefunctions_H

#include "File_chainer.h"
#include <iostream>
#include <vector>

#include <TROOT.h>


// Create coincidence root file as secondary data
void coincidence_creation(std::vector<TString> coincidences_filename, TString this_dir, TString write_dir, Int_t nr_datapoints, ULong64_t window, Int_t cutted_events, Int_t window_events, TString data_type_chain, TString data_type_tree, Int_t hist_bins, Double_t ns_window) {
	TChain* mychain = new TChain(data_type_chain);								// Chain the files
	add_files_to_chain(mychain, data_type_tree, ".root", this_dir);

	// Setup coincidence tree
	ULong64_t coinc_time;
	Float_t t_diff;
	UShort_t coinc_e0, coinc_e1, coinc_detector;
	bool coinc_bool;
	TString coinc_name = "Coincidence events ";
	
	// Define/set some variables/parameters
	ULong64_t t0, t1;
	Float_t dt;
	UShort_t e0, e1;
	bool multiple;
	Int_t nr_coincidences = 0;
	Double_t range_low, range_high;

	// Loop over number of desired data points within a batch
	for (Double_t k = 0; k < nr_datapoints; k++) {
		std::cout << coincidences_filename[k] << std::endl;
		TFile* hfile = TFile::Open(write_dir + coincidences_filename[k], "RECREATE");
		TTree* coinc_tree = new TTree("ctree", coinc_name);						
	}
		
	// Prepare assignments input tree
	TBranch* Energy_branch = mychain->GetBranch("Energy");							// Assign the branches
	TBranch* Channel_branch = mychain->GetBranch("Channel");
	TBranch* Time_branch = mychain->GetBranch("Timestamp");
	UShort_t energy, channel;										// Define the datatypes of the leaves
	ULong64_t time;
	mychain->SetBranchAddress("Energy", &energy);								// Sets the branches to a variable
	mychain->SetBranchAddress("Channel", &channel);
	mychain->SetBranchAddress("Timestamp", &time);
	const Int_t entry = mychain->GetEntries();
	printf("%d entries in chain \n", entry);
	

	// Loop over number of desired data points within a batch
	for (Double_t k = 0; k < nr_datapoints; k++) {
		std::cout << coincidences_filename[k] << std::endl;
		TFile* hfile = TFile::Open(write_dir + coincidences_filename[k], "RECREATE");
		TTree* coinc_tree = new TTree("ctree", coinc_name);						// Make a new tree for the coincidence events
		coinc_tree->Branch("coinc_time", &coinc_time, "coinc_time/L");					// Make a timestamp branch
		coinc_tree->Branch("t_diff", &t_diff, "t_diff/F");						// Make a time difference branch
		coinc_tree->Branch("coinc_detector", &coinc_detector, "coinc_detector/s");			// Make a detector branch
		coinc_tree->Branch("coinc_e0", &coinc_e0, "coinc_e0/s");					// Make a trigger energy branch
		coinc_tree->Branch("coinc_e1", &coinc_e1, "coinc_e1/s");					// Make a coincidence energy branch
		coinc_tree->Branch("multiple_flag", &coinc_bool, "multiple flag/O");				// If a particle is not the first coincidence to a trigger

		TH1F* hcoinc = new TH1F("hcoinc", "Energy spectrum", hist_bins, 0, hist_bins);			// Can be used to check present coincidence energies
		TH1F* htdiff = new TH1F("htdiff", "Time difference", 40, -ns_window * 1000, ns_window * 1000);	// Can be used to check the coincidence window

		range_low = cutted_events + round(k * entry / nr_datapoints);
		range_high = round((k + 1) * entry / nr_datapoints) - cutted_events;

		// Loop that finds coincidences and puts them in a new tree
		for (Int_t i = range_low; i < range_high; i++) {						// Iterate over all events
			mychain->GetEntry(i);
			multiple = false;

			if (channel == 0) {									// If the event is in detector 0: Check events in 1 detector and see if anything in the other one is in coincidence
				t0 = time;
				e0 = energy;

				for (Int_t j = -window_events; j < window_events; j++) {			// Check window_events before and after
					mychain->GetEntry(i + j);
					if (channel == 1) {							// Only check second detector
						t1 = time;
						e1 = energy;

						if (t1 < t0) {							// Make dt positive (for some reason you can't directly use value if it is negative
							dt = t0 - t1;
						}
						else {
							dt = t1 - t0;
						}

						if (dt <= window) {						// If in coincidence, add event to newly created tree
							if (t1 < t0) {
								dt *= -1;
							}
							nr_coincidences += 1;
							hcoinc->Fill(energy);
							htdiff->Fill(dt);

							coinc_time = t0;					// Fill detector 0 in coincidence tree
							t_diff = dt;
							coinc_detector = 0;
							coinc_e0 = e0;
							coinc_e1 = e1;
							coinc_bool = multiple;
							coinc_tree->Fill();

							coinc_time = t1;					// Fill detector 1 in coincidence tree
							t_diff = -dt;
							coinc_detector = 1;
							coinc_e0 = e1;
							coinc_e1 = e0;
							coinc_bool = multiple;
							coinc_tree->Fill();

							multiple = true;					// On next iteration for same trigger, note there was already a coincidence
						}
					}
				}
			}
		}
		// Write (and potentially draw) the energy spectrum of coincidences
		// hcoinc->Draw();
		// htdiff->Draw();
		hcoinc->Write("", TObject::kOverwrite);
		htdiff->Write("", TObject::kOverwrite);

		// Write (and potentially print) the coincidence tree
		// coinc_tree->Print();
		coinc_tree->Write("", TObject::kOverwrite);
		hfile->TFile::Close("R");
	}

	std::cout << "Saved Coincidence trees" << std::endl;
}


// Get the coincidence rates from secondary data
std::vector<Int_t> get_coincrates(TString directory, TString filename, std::vector<Double_t> channels0, std::vector<Double_t> channels1, std::vector<Int_t> hwidth) {
	TFile* f = new TFile(directory + filename);								// Open file
	TTree* mytree; f->GetObject("ctree", mytree);

	Long64_t time;
	Float_t tdiff;
	UShort_t energy, energy_c, detector;
	bool flag;
	Int_t N01 = 0;
	Int_t N10 = 0;

	TBranch* coinc_time_branch = mytree->GetBranch("coinc_time");						// Setup branches for reading
	TBranch* t_diff_branch = mytree->GetBranch("t_diff");
	TBranch* coinc_det_branch = mytree->GetBranch("coinc_detector");
	TBranch* coinc_e0_branch = mytree->GetBranch("coinc_e0");
	TBranch* coinc_e1_branch = mytree->GetBranch("coinc_e1");
	TBranch* mult_flag_branch = mytree->GetBranch("multiple_flag");
	mytree->SetBranchAddress("coinc_time", &time);
	mytree->SetBranchAddress("t_diff", &tdiff);
	mytree->SetBranchAddress("coinc_detector", &detector);
	mytree->SetBranchAddress("coinc_e0", &energy);
	mytree->SetBranchAddress("coinc_e1", &energy_c);
	mytree->SetBranchAddress("multiple_flag", &flag);

	int nr_entries = mytree->GetEntries();
	//printf("%d entries in coincidence file \n", nr_entries);
	
	for (int i = 0; i < nr_entries; i++) {									// Loop over secondary data events
		mytree->GetEntry(i);

		if (detector == 0) {										// If in detector 0
			if (abs(energy - channels0[0]) <= hwidth[0]) {						// Trigger energy?
				if (abs(energy_c - channels1[1]) <= hwidth[1]) {
					N01 += 1;
				}
			}
			else if (abs(energy - channels0[1]) <= hwidth[1]) {					// Coincidence energy range?
				if (abs(energy_c - channels1[0]) <= hwidth[0]) {
					N10 += 1;
				}
			}
		}
	}
	f->TFile::Close();

	return { N01, N10 };
}

#endif
