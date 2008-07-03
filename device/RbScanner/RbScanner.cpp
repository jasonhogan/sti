/*************************************************************************
 *
 * Name:   RbScanner.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to scan the Rb Spectrum using a function generator
 *
 * David Johnson 6/3/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include "RbScanner.h"

//===========================================================================

RBSCANNER::RBSCANNER()
{
	//stuff	
	//initialize frequency generator
	HP83711B hp83711b;

		//set the output power in dBm
	hp83711b.set_power(0);

	//initialize USB-1408FS DAQ
	USB1408FS usb1408fs;

		//initialize matlab engine for plotting
	Engine *ep;
	mxArray *data_freq = NULL;
	mxArray *data_DAQ = NULL;
	
	// Start the MATLAB engine 
	
	if (!(ep = engOpen(NULL))) {
		MessageBox ((HWND)NULL, (LPSTR)"Can't start MATLAB engine", 
			(LPSTR) "RbScanner.cpp", MB_OK);
		exit(-1);
	}

}

//===========================================================================

void RBSCANNER::scan_rb(int usb_channel, double start_freq, double end_freq, double freq_incr, double rf_power, bool save_data)
{

		//Scan laser

	double freq = start_freq; //frequency in GHz
	
	//prepare the hp83711b frequency synthesizer to scan 10 MHz at a time
	hp83711b.set_freq_increment(freq_incr);

	//set the start frequency at 1 GHz
	hp83711b.set_frequency(freq);

	//set the output power in dBm
	hp83711b.set_power(rf_power);

	//define vectors for storing the data

	std::vector <double> DAQ_vector;
	std::vector <double> FREQ_vector;

	hp83711b.output_on();

	hp83711b.set_frequency(freq);
	
	Sleep(100);

	//loop from start freq to start + interval
	while(freq <= end_freq) {

		FREQ_vector.push_back(freq); //record function generator frequency

		DAQ_vector.push_back(usb1408fs.read_input_channel(usb_channel)); //take data

		// change the frequency
		hp83711b.increment_frequency_up();
		freq = freq + freq_incr;

		Sleep(50); //wait for the function generator to settle. spec'd time is 20ms

		}	

		double *freq_data_ptr = (double*) calloc(FREQ_vector.size(), sizeof(double));
		if (freq_data_ptr==NULL) exit(1);
	double *DAQ_data_ptr = (double*) calloc(DAQ_vector.size(), sizeof(double));
		if (DAQ_data_ptr==NULL) exit(1);

	for(unsigned int i=0;i < DAQ_vector.size(); i++) {
		double stra = FREQ_vector.at(i);
		
		double strb = DAQ_vector.at(i);
		freq_data_ptr[i] = stra;
		DAQ_data_ptr[i] = strb;

		//	std::cout << stra << " , " << strb << std::endl;
		//	output_file << stra << " , " << strb << std::endl;

		//std::cout << freq_data_ptr[i*sizeof(double)] << std::endl;
		}


	data_freq = mxCreateDoubleMatrix(1, FREQ_vector.size(), mxREAL);
	memcpy((char*) mxGetPr(data_freq), freq_data_ptr, FREQ_vector.size()*sizeof(double));

	data_DAQ = mxCreateDoubleMatrix(1, FREQ_vector.size(), mxREAL);
	memcpy((char*) mxGetPr(data_DAQ), DAQ_data_ptr, FREQ_vector.size()*sizeof(double));

	/*
	 * Place the variable dataFreq & dataDAQ into the MATLAB workspace
	 */

	engPutVariable(ep, "dataFreq", data_freq);
	engPutVariable(ep, "dataDAQ", data_DAQ);

	/* Plot the result
	 */
	//engEvalString(ep, "hold on;");
	engEvalString(ep, "figure(1);");
	engEvalString(ep, "plot(dataFreq(:)',dataDAQ(:)');");
	engEvalString(ep, "title('Rb Scan');");
	engEvalString(ep, "xlabel('Frequency (GHz)');");
	engEvalString(ep, "ylabel('Absorbtion (V)');");

	free(DAQ_data_ptr);
	free(freq_data_ptr);

	FREQ_vector.clear();
	DAQ_vector.clear();

	if(save_data) {
		
		std::string filename_plot;
		std::string filename_raw_data;

		struct tm newtime;
		__int64 local_time;
		char time_buf[26];
		errno_t err;

		_time64( &local_time );

		// Obtain coordinated universal time: 
		err = _localtime64_s( &newtime, &local_time );
		if (err)
			{
			printf("Invalid Argument to _gmtime64_s.");
			}
   
		// Convert to an ASCII representation 
		err = asctime_s(time_buf, 26, &newtime);
		if (err)
		{
			printf("Invalid Argument to asctime_s.");
		}

		std::string date_string = time_buf;

		size_t found;

		found=date_string.find_first_of(":");
		
		while (found!=std::string::npos)
			{
			date_string[found]='_';
			found=date_string.find_first_of(":",found+1);
			}

		found=date_string.find_first_of("\n");
		
		while (found!=std::string::npos)
			{
			date_string.erase(found, 1);
			found=date_string.find_first_of("\n",found+1);
			}


		filename_plot = "RbScan plot on " + date_string;
		filename_raw_data = "RbScan raw data on " + date_string + ".csv"; 

		std::string path = "\\\\atomsrv1\\EP\\Data\\RbScannerAutoSave\\";
		std::string save_command_plot = "saveas(figure(1),'" + path + filename_plot + "','fig');";
		std::string save_command_raw_data = "csvwrite('" + path + filename_raw_data + "',[dataFreq(:), davg(:)]);";

	
		engEvalString(ep, save_command_plot.c_str());
		engEvalString(ep, save_command_raw_data.c_str());
	}

}

//===========================================================================
