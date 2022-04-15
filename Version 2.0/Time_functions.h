#ifndef timefunctions_H
#define timefunctions_H

#include <iostream>
#include <vector>

#include <TROOT.h>
#include <TFile.h>

// Calculate the meantime for a number of directories
std::vector<Double_t> get_meantimes(const Int_t nr_dirs, std::vector<Double_t> init_times, std::vector<Double_t> durations, Double_t half_life) {
	std::vector<Double_t> mean_times(nr_dirs);
	Double_t decay_constant = log(2) / half_life;

	for (Int_t i = 0; i < nr_dirs; i++) {
		mean_times[i] = init_times[i] + (durations[i] / 86400.0) + (1 / decay_constant) * log(decay_constant * durations[i] / 86400.0) - (1 / decay_constant) * log(exp(decay_constant * durations[i] / 86400.0) - 1);
	}

	return mean_times;
}


// Determine the duration of the measurement
Double_t meas_time(TString read_dir, TString filename, TString datatype_chain, TString datatype_tree) {
	TChain* mytree = new TChain(datatype_chain);
	add_files_to_chain(mytree, datatype_tree, ".root", read_dir + filename);				// Get the relevant data in a chain

	TBranch* Time_branch = mytree->GetBranch("Timestamp");
	ULong64_t time;
	mytree->SetBranchAddress("Timestamp", &time);
	const Int_t entry = mytree->GetEntries();								// Number of entries in the root file

	Double_t measurement_time;										// Length of measurement
	Double_t final_time;
	Double_t initial_time;

	mytree->GetEntry(1);
	initial_time = time / 1.0e12;
	mytree->GetEntry(entry - 1);
	final_time = time / 1.0e12;
	measurement_time = final_time - initial_time;
	//printf("Measured for %d \n", measurement_time);

	return measurement_time;
}



// For a single file, calculate the meantime
Double_t get_meantime(Double_t init_t, Double_t duration, Double_t half_life) {
	Double_t mean_t;
	Double_t decay_constant = log(2) / half_life;

	mean_t = init_t + (duration / 86400.0) + (1 / decay_constant) * log(decay_constant * duration / 86400.0) - (1 / decay_constant) * log(exp(decay_constant * duration / 86400.0) - 1);

	return mean_t;
}


// Determine the start time of a single measurement
Double_t start_time(TString write_dir, TString filename) {
	TFile* f = new TFile(write_dir + filename);								// Load the root file
	TTree* mytree; f->GetObject("ctree", mytree);
	TBranch* coinc_time_branch = mytree->GetBranch("coinc_time");
	Long64_t time;
	mytree->SetBranchAddress("coinc_time", &time);

	const Int_t entry = mytree->GetEntries();								// Number of entries in the root file
	Double_t initial_time;
	mytree->GetEntry(1);
	initial_time = time / 1.0e12;

	return initial_time;
}


// Determine the measurement time for a datapoint
Double_t meas_t(TString write_dir, TString filename) {
	TFile* f = new TFile(write_dir + filename);								// Load the root file
	TTree* mytree; f->GetObject("ctree", mytree);
	TBranch* coinc_time_branch = mytree->GetBranch("coinc_time");
	Long64_t time;
	mytree->SetBranchAddress("coinc_time", &time);
	const Int_t entry = mytree->GetEntries();								// Number of entries in the root file

	Double_t measurement_time;										// Length of measurement
	Double_t final_time;
	Double_t initial_time;

	mytree->GetEntry(1);
	initial_time = time / 1.0e12;
	mytree->GetEntry(entry - 1);
	final_time = time / 1.0e12;
	measurement_time = final_time - initial_time;

	return measurement_time;
}
#endif
