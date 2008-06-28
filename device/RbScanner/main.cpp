

#include "ENET_GPIB_device.h"
#include "HP83711B.h"
#include "USB1408FS.h"
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <windows.h>

#include <iostream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>

#include "engine.h"

#define PI 3.14159265

int main(int argc, char* argv[])

{
	

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

	//Scan over Rb85 repump line (3 GHz blue of 85 cooling)
	// assumes laser is locked at Rb85 cooling

	//initialize frequency generator
	HP83711B hp83711b;

		//set the output power in dBm
	hp83711b.set_power(1);

	//initialize USB-1408FS DAQ
	USB1408FS usb1408fs;

	//specify channel to read
	int usb_channel = 7;

	//define loop variables
	
	double start_freq = 1.8;
	double freq = start_freq; //frequency in GHz
	double freq_incr = .003; //increment frequency in GHz
	double freq_range = .8; // range in GHz
	double freq_endpoint = freq_range + freq; // end freq in GHz
	int number_of_points = floor(freq_range / freq_incr) + 1; 


	//prepare the hp83711b frequency synthesizer to scan 10 MHz at a time
	hp83711b.set_freq_increment(freq_incr);

	//set the start frequency at 1 GHz
	hp83711b.set_frequency(freq);

	//set the output power in dBm
	hp83711b.set_power(1.8);

	//define vectors for storing the data

	std::vector <double> DAQ_vector;
	std::vector <double> FREQ_vector;


	engEvalString(ep, "counter = 0;");

	std::ostringstream num_points;
	num_points << number_of_points;
	std::string num_points_str = num_points.str();

	std::string full_command_str = "davg=zeros(1," + num_points_str + ");";
	
	engEvalString(ep, full_command_str.c_str());


	//engEvalString(ep, "davg=zeros(1,201);");

	//take the modulation off data point
	//FREQ_vector.push_back(0);
	//DAQ_vector.push_back(usb1408fs.read_input_channel(usb_channel));

	//turn on the function generator
	hp83711b.output_on();

	int counter = 0;
	while(1) {
	//set the start frequency at 1 GHz
		counter ++;
		freq = start_freq;
		hp83711b.set_frequency(freq);
	
		Sleep(100);

		//loop from start freq to start + interval
		while(freq <= freq_endpoint) {

			FREQ_vector.push_back(freq); //record function generator frequency

			//std::cerr << usb1408fs.read_input_channel(4) << std::endl;

			DAQ_vector.push_back(usb1408fs.read_input_channel(usb_channel)); //take data

			// change the frequency
			hp83711b.increment_frequency_up();
			freq = freq + freq_incr;

			//std::cout << freq << std::endl;

			Sleep(50); //wait for the function generator to settle. spec'd time is 20ms

		}	

/*	std::ofstream output_file;
	output_file.open ("data.txt");
*/



	double *freq_data_ptr = (double*) calloc(FREQ_vector.size(), sizeof(double));
		if (freq_data_ptr==NULL) exit(1);
	double *DAQ_data_ptr = (double*) calloc(DAQ_vector.size(), sizeof(double));
		if (DAQ_data_ptr==NULL) exit(1);

	for(unsigned int i=0;i < DAQ_vector.size(); i++) {
		double stra = FREQ_vector.at(i);
		
		//DAQ_weighted_avg.at(i) = ((counter-1)/counter)*DAQ_weighted_avg.at(i) + (1/counter)*DAQ_vector.at(i);

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
	engEvalString(ep, "counter = counter+1;");
	engEvalString(ep, "davg(:)=((counter-1)/counter)*davg + (1/counter)*dataDAQ;");
	engEvalString(ep, "figure(1);");
	engEvalString(ep, "hold off;");
	engEvalString(ep, "plot(dataFreq(:)',davg(:)');");
	engEvalString(ep, "title('Rb Scan');");
	engEvalString(ep, "xlabel('Frequency (GHz)');");
	engEvalString(ep, "ylabel('Absorbtion (V)');");
	engEvalString(ep, "figure(2);");
	engEvalString(ep, "hold on;");
	engEvalString(ep, "plot(dataFreq(:)',dataDAQ(:)');");
	engEvalString(ep, "title('Rb Scan');");
	engEvalString(ep, "xlabel('Frequency (GHz)');");
	engEvalString(ep, "ylabel('Absorbtion (V)');");

	free(DAQ_data_ptr);
	free(freq_data_ptr);

	FREQ_vector.clear();
	DAQ_vector.clear();
	//	output_file.close();


	std::cout << "loop #" << counter << std::endl;
	}
	return 0;
};