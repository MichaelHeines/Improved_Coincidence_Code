#ifndef filechainer_H
#define filechainer_H

#include <iostream>
#include <vector>

#include <TROOT.h>
#include <TChain.h>
#include <TSystemDirectory.h>


// Add files from directory starting with startstring and ending with endstring to a chain
Int_t add_files_to_chain(TChain* chain, TString startstring, TString endstring, TString directory) {
	const char* dirname = directory;
	printf("Current directory is %s \n", dirname);
	//std::cout << dirname << std::endl;
	const char* ext = ".root";
	std::vector<TString> filenames;
	int added = 0;
	TSystemDirectory dir(dirname, dirname);
	TList* files = dir.GetListOfFiles();

	if (files) {
		TSystemFile* file;
		TString fname;
		TIter next(files);

		while ((file = (TSystemFile*)next())) {
			fname = file->GetName();

			if (!file->IsDirectory() && fname.EndsWith(endstring) && fname.BeginsWith(startstring)) {					//get  list of file names within directory.
				filenames.push_back(fname);
				++added;
			}
		}

		sort(filenames.begin(), filenames.end());
		for (int i = 0; i < added - 1;) {
			if (filenames[i + 1].Sizeof() < filenames[i].Sizeof()) {
				TString temp = filenames[i];
				filenames[i] = filenames[i + 1];
				filenames[i + 1] = temp;
				i = 0;
			}
			i++;
		}

		Int_t i = 0;
		for (std::vector<TString>::iterator it = filenames.begin(); it != filenames.end(); ++it) {

			if (i < 50) {
				//std::cout << *it << std::endl;
				chain->Add(dirname + *it);
			}
			i++;
		}
	}
	printf("%d files have been added \n", added);
	return added;
}


// Calculate the number of data points within a batch
std::vector<Int_t> parts_per_batch(const Int_t nr_dirs, TString data_dir, std::vector<TString> ls_dirs, std::vector<Double_t> mean_times, Int_t minimal_coinc, Double_t half_life, TString data_type_chain, TString data_type_tree, Double_t coinc_to_tot) {
	//Double_t coinc_to_tot = 601 / 189046414.0;								// M108: 601/189046414.0; M120: 829 / 291994800.0			// Check how many coincidences in the last batch before running!
	Double_t desired_final_filelength = minimal_coinc / coinc_to_tot;					// about 110.000
	//std::cout << desired_final_filelength << std::endl;
	Double_t time_diff;
	Int_t entries;
	std::vector<Int_t> per_batch(nr_dirs);

	for (Int_t i = 0; i < nr_dirs; i++) {
		TChain* mytree = new TChain(data_type_chain);
		add_files_to_chain(mytree, data_type_tree, ".root", data_dir + ls_dirs[i]);
		entries = mytree->GetEntries();
		time_diff = mean_times[nr_dirs - 1] - mean_times[i];
		per_batch[i] = round(entries / (desired_final_filelength * exp(time_diff * 0.693/ half_life)));		// Add factor 2(0) in exponent for same size absolute(relative) error; This method provides roughly the same number of data points for the same measurement time (with factor 1 in exponent)
	}

	return per_batch;
}

#endif
