#ifndef Reading_dat_H
#define Reading_dat_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include <TROOT.h>


// Line1: comment line for user --> skip
// Line2: read dir string
// Line3: write dir string
// Line4: data dir string
// Line5: output csv string
// Line6: coincidence filename string
// Line7: single filename string
// Line8: ls dirs vector of strings
// Line9: indices vector of strings
// Line10: -----
// Line11: month_0, day_0, hour_0, minute_0
// Line12: days
// Line13: months
// Line14: hours
// Line15: minutes
// Line16: -----
// Line17: half_life, E1, E2, ratio_to_activity
// Line18: calibration energies
// Line19: ranges
// Line20: hwidths
// Line21: cutted_events, window_events, minimal coinc
// Line22: process_raw
// Line23: -----
// Line24: dataype_chain, datatype_tree
// Line25: hist bins, ns window, coinc_to_total (run for entire last batch or estimate)
// Line26: -----


// Structure made specifically for initialization
struct Read_out {
	TString dat_name, data_dir, write_dir, output_file, coincidences_filename, singles_filename, datatype_chain, datatype_tree;
	Double_t month_0, day_0, hour_0, half_life, E0, E1, ratio_to_activity, ns_window, coinc_to_total;
	Int_t nr_dirs, cutted_events, window_events, minimal_coinc, hist_bins;
	std::vector<TString> *ls_dirs, *indices;
	std::vector<Double_t> *months, *days, *hours, calibrations;
	std::vector<Int_t> ranges, hwidth;
	bool process_raw;
};



Read_out Reading_dat(TString dat_name, TString data_dir, TString write_dir, TString output_file, TString coincidences_filename, TString singles_filename, TString datatype_chain, TString datatype_tree, Double_t month_0, Double_t day_0, Double_t hour_0, Double_t half_life, Double_t E0, Double_t E1, Double_t ratio_to_activity, Double_t ns_window, Double_t coinc_to_total, Int_t nr_dirs, Int_t cutted_events, Int_t window_events, Int_t minimal_coinc, Int_t hist_bins, std::vector<TString> *ls_dirs, std::vector<TString> *indices, std::vector<Double_t> *months, std::vector<Double_t> *days, std::vector<Double_t> *hours, std::vector<Double_t> calibrations, std::vector<Int_t> ranges, std::vector<Int_t> hwidth, bool process_raw) {
	//	In: Filename and initialized variables
	//	Out: Read_out structure
	std::ifstream infile(dat_name);
	std::string line;
	std::string delimiter = ", ";

	Double_t minute_0, temp;
	std::vector<Double_t> *minutes = new std::vector<Double_t>(1, 0.0);
	Read_out output;

	try {
		std::getline(infile, line);
		std::getline(infile, line);
		line.erase(line.size() - 1, line.size());
		data_dir = line;
		std::getline(infile, line);
		line.erase(line.size() - 1, line.size());
		write_dir = line;
		std::getline(infile, line);
		line.erase(line.size() - 1, line.size());
		output_file = line;
		std::getline(infile, line);
		line.erase(line.size() - 1, line.size());
		coincidences_filename = line;
		std::getline(infile, line);
		line.erase(line.size() - 1, line.size());
		singles_filename = line;
		std::getline(infile, line);
		std::stringstream(line) >> nr_dirs;
		
		if (nr_dirs == 0){
			std::cerr << "ERROR: nr_dirs can't be 0; check if the format is correct." << std::endl;
		}
		std::getline(infile, line);
		line.erase(line.size() - 1, line.size());
		try{
			for (Int_t i = 0; i < nr_dirs; i++){
				if (i == 0){
					(*ls_dirs)[i] = line.substr(0, line.find(delimiter));
					line.erase(0, line.find(delimiter) + delimiter.length());
				}
				else{
					ls_dirs->push_back(line.substr(0, line.find(delimiter)));
					line.erase(0, line.find(delimiter) + delimiter.length());
				}
			}
			std::getline(infile, line);
			line.erase(line.size() - 1, line.size());
			for (Int_t i = 0; i < nr_dirs; i++){
				if (i == 0){
					(*indices)[i] = line.substr(0, line.find(delimiter));
					line.erase(0, line.find(delimiter) + delimiter.length());
				} else{
					indices->push_back(line.substr(0, line.find(delimiter)));
					line.erase(0, line.find(delimiter) + delimiter.length());
				}
				
			}
		}
		catch(...){
			std::cerr << "ERROR: Number of files not appropriate.";
			exit(0);
		}
		std::getline(infile, line);
		std::getline(infile, line);
		try{
			std::stringstream(line.substr(0, line.find(delimiter))) >> month_0;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> day_0;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> hour_0;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> minute_0;
			hour_0 += minute_0/60.0;
		}
		catch(...){
			std::cerr << "ERROR: Not enough initial time conditions.";
			exit(0);
		}
		std::getline(infile, line);
		std::getline(infile, line);
		try{
			for (Int_t i = 0; i < nr_dirs; i++){
				std::stringstream(line.substr(0, line.find(delimiter))) >> temp;
				if(i == 0){
					(*months)[i] = temp;
				}
				else{
					months->push_back(temp);
				}
				line.erase(0, line.find(delimiter) + delimiter.length());
			}
			std::getline(infile, line);
			for (Int_t i = 0; i < nr_dirs; i++){
				std::stringstream(line.substr(0, line.find(delimiter))) >> temp;
				if(i == 0){
					(*days)[i] = temp;
				}
				else{
					days->push_back(temp);
				}
				line.erase(0, line.find(delimiter) + delimiter.length());
			}
			std::getline(infile, line);
			for (Int_t i = 0; i < nr_dirs; i++){
				std::stringstream(line.substr(0, line.find(delimiter))) >> temp;
				if(i == 0){
					(*hours)[i] = temp;
				}
				else{
					hours->push_back(temp);
				}
				line.erase(0, line.find(delimiter) + delimiter.length());
			}
			std::getline(infile, line);
			for (Int_t i = 0; i < nr_dirs; i++){
				std::stringstream(line.substr(0, line.find(delimiter))) >> temp;
				if(i == 0){
					(*minutes)[i] = temp;
				}
				else{
					minutes->push_back(temp);
				}
				line.erase(0, line.find(delimiter) + delimiter.length());
				(*hours)[i] += (*minutes)[i]/60.0;
			}
		}
		catch(...){
			std::cerr << "ERROR: Number of times not appropriate.";
			exit(0);
		}
		std::getline(infile, line);
		std::getline(infile, line);
		try{
			std::stringstream(line.substr(0, line.find(delimiter))) >> half_life;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> E0;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> E1;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> ratio_to_activity;
			line.erase(0, line.find(delimiter) + delimiter.length());
		}
		catch(...){
			std::cerr << "ERROR: Number of entries on line 18 is not appropriate.";
			exit(0);
		}
		std::getline(infile, line);
		try{
			for (Int_t i = 0; i < 3; i++){
				std::stringstream(line.substr(0, line.find(delimiter))) >> calibrations[i];
				line.erase(0, line.find(delimiter) + delimiter.length());
			}
		}
		catch(...){
			std::cerr << "ERROR: Number of calibration peaks is not appropriate.";
			exit(0);
		}
		std::getline(infile, line);
		try{
			for (Int_t i = 0; i < 6; i++){
				std::stringstream(line.substr(0, line.find(delimiter))) >> ranges[i];
				line.erase(0, line.find(delimiter) + delimiter.length());
			}
		}
		catch(...){
			std::cerr << "ERROR: Number of calibration ranges is not appropriate.";
			exit(0);
		}
		
		std::getline(infile, line);
		try{
			for (Int_t i = 0; i < 2; i++){
				std::stringstream(line.substr(0, line.find(delimiter))) >> hwidth[i];
				line.erase(0, line.find(delimiter) + delimiter.length());
			}
		}
		catch(...){
			std::cerr << "ERROR: Number of width intervals is not appropriate.";
			exit(0);
		}
		std::getline(infile, line);
		try{
			std::stringstream(line.substr(0, line.find(delimiter))) >> cutted_events;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> window_events;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> minimal_coinc;
			line.erase(0, line.find(delimiter) + delimiter.length());
		}
		catch(...){
			std::cerr << "ERROR: Number of Coincidence parameters is not appropriate.";
			exit(0);
		}
		std::getline(infile, line);
		try{
			std::stringstream(line) >> process_raw;
		}
		catch(...){
			std::cerr << "ERROR: process raw parameter is not appropriate.";
			exit(0);
		}
		std::getline(infile, line);
		std::getline(infile, line);
		line.erase(line.size() - 1, line.size());
		try{
			std::stringstream(line.substr(0, line.find(delimiter))) >> datatype_chain;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> datatype_tree;
			line.erase(0, line.find(delimiter) + delimiter.length());
		}
		catch(...){
			std::cerr << "ERROR: Number of datatypes is not appropriate.";
			exit(0);
		}
		std::getline(infile, line);
		try{
			std::stringstream(line.substr(0, line.find(delimiter))) >> hist_bins;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> ns_window;
			line.erase(0, line.find(delimiter) + delimiter.length());
			std::stringstream(line.substr(0, line.find(delimiter))) >> coinc_to_total;
			line.erase(0, line.find(delimiter) + delimiter.length());
		}
		catch(...){
			std::cerr << "ERROR: Number of datatypes is not appropriate.";
			exit(0);
		}
	}
	catch(...){
		std::cerr << "ERROR: Dat file not appropriate." << std::endl;
		exit(0);
	}
	infile.close();

	output.dat_name = dat_name;
	output.data_dir = data_dir;
	output.write_dir = write_dir;
	output.output_file = output_file;
	output.coincidences_filename = coincidences_filename;
	output.singles_filename = singles_filename;
	output.datatype_chain = datatype_chain;
	output.datatype_tree = datatype_tree;
	output.month_0 = month_0;
	output.day_0 = day_0;
	output.hour_0 = hour_0;
	output.half_life = half_life;
	output.E0 = E0;
	output.E1 = E1;
	output.ratio_to_activity = ratio_to_activity;
	output.ns_window = ns_window;
	output.coinc_to_total = coinc_to_total;
	output.nr_dirs = nr_dirs;
	output.cutted_events = cutted_events;
	output.window_events = window_events;
	output.minimal_coinc = minimal_coinc;
	output.hist_bins = hist_bins;
	output.ls_dirs = ls_dirs;
	output.indices = indices;
	output.months = months;
	output.days = days;
	output.hours = hours;
	output.calibrations = calibrations;
	output.ranges = ranges;
	output.hwidth = hwidth;
	output.process_raw = process_raw;

	std::cout << "Loaded settings" << std::endl;
	return output;
}


#endif