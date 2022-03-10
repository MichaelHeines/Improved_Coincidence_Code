#include <iostream>
#include <vector>

#include <TROOT.h>

#include "File_chainer.h"
#include "Time_functions.h"
#include "Calibration.h"
#include "Coincidence_functions.h"
#include "Singles_functions.h"
#include "Reading_dat.h"

#include "Exponential_fit.h"

// INPUT: .dat file, for format see Readin_dat.h
// Planned adaptations: exponential fitting, build an exe? --> Faster, cleaner, but more black-box like
// OUTPUT: Currently csv; Fitting in python is nice and easy, keep it for now but made empty header Exponential_fit.h that can be implemented

int main(int argc, char** argv) {
	if (argc != 2){
		std::cout << "ERROR: Should have exactly one input parameter" << std::endl;
		std::cout << "Please use ./main.exe directory/settings.dat" << std::endl;
		return 0;
	}	
	TString dat_name = 	argv[1];				//"/mnt/ksf9/H2/user/u0148069/share/my.public/Coincidence_creation/Settings_M108.dat";
	TString data_dir, write_dir, output_file, coincidences_filename, singles_filename, datatype_chain, datatype_tree;
	Double_t month_0, day_0, hour_0, half_life, E0, E1, ratio_to_activity, ns_window, coinc_to_total;
	Int_t nr_dirs, cutted_events, window_events, minimal_coinc, hist_bins;
	std::vector<TString> *ls_dirs_ptr = new std::vector<TString>(1, " "), *indices_ptr = new std::vector<TString>(1, " ");
	std::vector<Double_t> *months_ptr = new std::vector<Double_t>(1, 0.0), *days_ptr = new std::vector<Double_t>(1, 0.0), *hours_ptr = new std::vector<Double_t>(1, 0.0);
	std::vector<TString> ls_dirs, indices;
	std::vector<Double_t> months, days, hours;
	std::vector<Double_t> calibrations(3);
	std::vector<Int_t> ranges(6), hwidth(2);
	bool process_raw;
	Read_out loading;
	loading = Reading_dat(dat_name, data_dir, write_dir, output_file, coincidences_filename, singles_filename, datatype_chain, datatype_tree, month_0, day_0, hour_0, half_life, E0, E1, ratio_to_activity, ns_window, coinc_to_total, nr_dirs, cutted_events, window_events, minimal_coinc, hist_bins, ls_dirs_ptr, indices_ptr, months_ptr, days_ptr, hours_ptr, calibrations, ranges, hwidth, process_raw);
	
	data_dir = loading.data_dir;
	write_dir = loading.write_dir;
	output_file = loading.output_file;
	coincidences_filename = loading.coincidences_filename;
	singles_filename = loading.singles_filename;
	datatype_chain = loading.datatype_chain;
	datatype_tree = loading.datatype_tree;
	month_0 = loading.month_0;
	day_0 = loading.day_0;
	hour_0 = loading.hour_0;
	half_life = loading.half_life;
	E0 = loading.E0;
	E1 = loading.E1;
	ratio_to_activity = loading.ratio_to_activity;
	ns_window = loading.ns_window;
	coinc_to_total = loading.coinc_to_total;
	nr_dirs = loading.nr_dirs;
	cutted_events = loading.cutted_events;
	window_events = loading.window_events;
	minimal_coinc = loading.minimal_coinc;
	hist_bins = loading.hist_bins;
	ls_dirs = *loading.ls_dirs;
	indices = *loading.indices;
	months = *loading.months;
	days = *loading.days;
	hours = *loading.hours;
	calibrations = loading.calibrations;
	ranges = loading.ranges;
	hwidth = loading.hwidth;
	process_raw = loading.process_raw;

	// General initialization
	std::vector<Double_t> calibs0(2), calibs1(2), channels0(2), channels1(2);									// Vectors with doubles of length 2;
	std::vector<Double_t> singles(4), calibs(4);																// Vectors with doubles of length 4;
	std::vector<Int_t> doubles(2);																				// Vectors with integers of length 2;
	std::vector<Double_t> init_times(nr_dirs), durations(nr_dirs), mean_times(nr_dirs);
	std::vector<Int_t> per_batch(nr_dirs);
	TString index, current_dir, second_index;
	Double_t measurement_time, ratio, N12, N1, N2, init_t, mean_t, activity, rel_err;
	ULong64_t window = 1000 * ns_window;
	
	// Calculate the mean time and number of data points per batch as defined in file chainer header file
	for (Int_t i = 0; i < nr_dirs; i++) {
		init_times[i] = 30 * (months[i] - month_0) + (days[i] - day_0) + ((hours[i] - hour_0) / 24.0);
		Double_t measurement_time = meas_time(data_dir, ls_dirs[i]);
		durations[i] = measurement_time;
	}

	mean_times = get_meantimes(nr_dirs, init_times, durations, half_life);
	per_batch = parts_per_batch(nr_dirs, data_dir, ls_dirs, mean_times, minimal_coinc, half_life, datatype_chain, datatype_tree, coinc_to_total);
	
	for (Int_t i = 0; i < nr_dirs; i++) {
		printf("%d datapoints in this batch \n", per_batch[i]);
	}

	

	// Process primary data --> Single histogram & coincidence tree
	for (Int_t i = 0; i < nr_dirs; i++) {
		current_dir = data_dir + ls_dirs[i];
		std::vector<TString> coincidences_file(per_batch[i]), singles_file(per_batch[i]);
		
		//std::cout << current_dir << std::endl;
		for (Int_t j = 0; j < per_batch[i]; j++) {
			second_index = std::to_string(j);
			index = indices[i] + second_index;
			coincidences_file[j] = index + coincidences_filename;
			singles_file[j] = index + singles_filename;
		}
		
		if (process_raw == true) {																							// Full analysis, or start from created trees and histograms
			coincidence_creation(coincidences_file, current_dir, write_dir, per_batch[i], window, cutted_events, window_events, datatype_chain, datatype_tree, hist_bins, ns_window);
			create_single_spectrum(singles_file, current_dir, write_dir, per_batch[i], datatype_chain, datatype_tree, hist_bins);
		}
	}
	

	
	// Process secondary data --> Obtain activity values in csv format
	std::ofstream Activity_file(write_dir + output_file);
	Activity_file << "Folder " <<  ";" << "A (from Tl)" << ";" << "A relerr (from Tl)" << ";" << "Mean times" << std::endl;

	for (Int_t i = 0; i < nr_dirs; i++) {
		std::vector<TString> coincidences_file(per_batch[i]), singles_file(per_batch[i]);

		for (Int_t j = 0; j < per_batch[i]; j++) {
			second_index = std::to_string(j);
			index = indices[i] + second_index;
			coincidences_file[j] = index + coincidences_filename;
			singles_file[j] = index + singles_filename;

			calibs = calibrate(write_dir, singles_file[j], calibrations, ranges);
			calibs0 = { calibs[0], calibs[1] };
			calibs1 = { calibs[2], calibs[3] };
			channels0 = { get_channel(calibs0, E0), get_channel(calibs0, E1) };											// Coincidence energies are specific to the isotope
			channels1 = { get_channel(calibs1, E0), get_channel(calibs1, E1) };
			singles = get_singlerates(write_dir, singles_file[j], channels0, channels1, hwidth);						// {det0 low E, det0 highE, det1 lowE, det1 highE}
			doubles = get_coincrates(write_dir, coincidences_file[j], channels0, channels1, hwidth);					// Low energy photon in {det 0, det1}
					
			N1 = singles[0] + singles[2];
			N2 = singles[1] + singles[3];
			N12 = doubles[0] + doubles[1];
			ratio = N1 * N2 / (2 * N12);

			measurement_time = meas_t(write_dir, coincidences_file[j]);													// Duration of datapoint
			init_t = init_times[i] + start_time(write_dir, coincidences_file[j]) / 86400.0;
			mean_t = get_meantime(init_t, measurement_time, half_life);
			activity = ratio * ratio_to_activity / measurement_time;													// Correction specific to isotope
			rel_err = sqrt(1 / N12);
			
			Activity_file << index << ';' << activity << ";" << rel_err << ";" << mean_t << std::endl;					// Error is only statistical error due to Poisson statistics!
		}
	}

	std::cout << "Ding Dong, analysis is done" << std::endl;
	// Apply an exponential fit on the csv data. For now, use quick and dirty lmfit in python
	return 1;
}

