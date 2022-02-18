# Improved_Coincidence_Code

Read me: Coincidence_creation code		Last edit 17/02/2022
Main program is made such that a Settings.dat file provides all of the inputs.
Time difference histogram is kept at 40 bins (for now).
For additional information, contact "michael.heines@kuleuven.be"

Reading_dat.h:
Read_out structure: designed to provide program with initialization
Reading_dat()
	Inputs: See file
	Outputs: See file

File_chainer.h:
add_files_to_chain()
	Input:	Chain, start string, end string, directory of data
	Output:	Number of files in chain; return as Root Int (and add files to chain)
parts_per_batch()
	Input:	Number of directories, read directory, list of directories, list of mean times, minimal number of coincidences, half-life
	Output:	Number of datapoints per batch; return as vector

Time_functions.h:
get_meantimes()
	Input:	Number of directories, initial times (batches), durations (batches), half-life
	Output:	Mean times of the batch; return as a vector
meas_time()
	Input:	Directory of raw data
	Output:	Length of batch measurement; return as Root Double
get_meantime()
	Input:	Initial time (datapoint), duration (datapoint), half-life
	Output:	Meantime (datapoint); return as Root Double
start_time()
	Input:	Write directory, filename
	Output:	Initial time (datapoint); return as Root Double
meas_t()
	Input:	Write directory, filename
	Output:	Measurement time (datapoint); return as Root Double

Calibration.h:
calibration() 
	Input:	A prepared single energy spectrum, 3 calibration energy points and preset ranges in which the calibration peaks will be the highest point.
	Output:	Linear energy calibration parameters; returned as a vector {intercept, slope}
get_channel()
	Input:	A calibration vector [from calibration()] and the energy for which you want to know the channel number
	Output:	Channel number corresponding to the requested energy; returned as a Root double
calibrate()
	Input:	A directory, a filename, three calibration energies, six ranges within which the peaks lay
	Output:	Calibration parameters for both detectors; returned as a vector {intercept detector 0, Slope detector 0, Intercept detector 1, slope detector 2}

Coincidence_functions.h:
coincidence_creation()
	Input: 	A vector of filenames, a reading directory, a writing directory and the number of datapoints in the batch
	Output:	Void, but creates root trees with elements {coinc_time, t_diff, coinc_detector, coinc_e0, coinc_e1, multiple flag}
	Setup dependency: Histogram ranges can/should be adjusted to preference
get_coincrates()
	Input:	Directory, filename, channels corresponding to the energies of interest, width of the set energy range around the energies of interest
	Output:	Number of coincidences with both channels as trigger; returned as a vector {N01, N10}

Singles_functions.h:
populate_background()
	Input:	Filled histogram, empty histogram, maximal number of iterations
	Output:	Void, populate background histogram
create_single_spectrum()
	Input:	List of filenames, read directory, write directory, number of datapoints
	Output:	Void, Create histograms for pure spectrum, background, and background subtracted
bin_integral()
	Input:	Energy, histogram (bg subtracted), width of interval
	Output:	Bin integral of bg subtracted spectrum; return as Long64_t
get_singlerates()
	Input:	Directory, filename, peak positions in channel 0/1, width of interval
	Output:	Single countrates; return as vector


Complete_analysis.cpp:	Main program
Provide path+filename with inputs following the format in reading_dat.h
Void function that writes out a csv with activities and corresponding mean times.
Program is operational and provides the same output as previous stable version.

Future development:
Exponential fitting program --> Currently use python
Make build script to create .exe --> Would be faster, but more black-box like for the user?
