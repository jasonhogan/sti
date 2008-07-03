

#include "ENET_GPIB_device.h"
#include "HP83711B.h"
#include "USB1408FS.h"
#include <math.h>
#include <time.h>
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

	//Scan laser

	//initialize frequency generator
	HP83711B hp83711b;

		//set the output power in dBm
	hp83711b.set_power(0);

	//initialize USB-1408FS DAQ
	USB1408FS usb1408fs;

	//specify channel to read
	int usb_channel = 7;

	//define loop variables
	
	int loops = 1; // number of scans to average over
    double start_freq = 1.4; // start point in GHz
    double freq_incr = .003; //increment frequency in GHz
    double freq_endpoint = 2.4; // endpoint in GHz   
    double power = 0.8; // output power in dBm
    bool change_vals = true; // have user defined values

	std::cout << "default values are as follows:" << std::endl;
    std::cout << "# of loops: " << loops << std::endl;
	std::cout << "USB input channel: " << usb_channel << std::endl;
    std::cout << "Start Frequency: " << start_freq << " GHz" << std::endl;
    std::cout << "End Frequency: " << freq_endpoint << " GHz" << std::endl;
    std::cout << "Frequency Increment: " << freq_incr << " GHz" << std::endl;
    std::cout << "Output Power: " << power << " dBm" << std::endl;
    std::cout << std::endl << "Do you want to change (1/0)? ";
    std::cin >> change_vals; 

    if(change_vals) {
   
        // user defined number of loops   
        std::cout << "Enter desired number of loops. Default value is:" << loops << std::endl << "# = ";
        std::cin >> loops;

        // user defined start frequency
        std::cout << "Enter desired start frequency in GHz. Default value is: " << start_freq << " GHz" << std::endl << "start freq = ";
        std::cin >> start_freq;
   
        std::cout << "Enter desired end frequency in GHz. Default value is: " << freq_endpoint << " GHz" << std::endl << "end freq = ";
        std::cin >> freq_endpoint;
   
        std::cout << "Enter desired frequency increment in GHz. Default value is: " << freq_incr << " GHz" << std::endl << "freq increment = ";
        std::cin >> freq_incr;
   
        std::cout << "Enter desired output power in dBm. Default value is: " << power << " dBm" << std::endl << "power = ";
        std::cin >> power;
   
    }
   
    double freq = start_freq; //frequency in GHz
    double freq_range = freq_endpoint - start_freq; // freq range in GHz
    int number_of_points = floor(freq_range / freq_incr) + 1;


	//prepare the hp83711b frequency synthesizer to scan 10 MHz at a time
	hp83711b.set_freq_increment(freq_incr);

	//set the start frequency at 1 GHz
	hp83711b.set_frequency(freq);

	//set the output power in dBm
	hp83711b.set_power(power);

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
	while(counter < loops) {
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
	bool save_data = true;

	std::cout << "Do you want to save this data (1/0) ?";
	std::cin >> save_data;

	if(save_data) {
		
		std::string filename_avg;
		std::string filename_alldata;
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


		filename_avg = "RbScan average on " + date_string;// + ".fig";
		std::cout << filename_avg << std::endl;

		filename_alldata = "RbScan all data on " + date_string;// + ".fig";
		std::cout << filename_alldata << std::endl;

		filename_raw_data = "RbScan raw data on " + date_string + ".csv"; 

		std::string path = "\\\\atomsrv1\\EP\\Data\\RbScannerAutoSave\\";

		std::string save_command_avg = "saveas(figure(1),'" + path + filename_avg + "','fig');";

		std::string save_command_alldata = "saveas(figure(2),'" + path + filename_alldata + "','fig');";

		std::string save_command_raw_data = "csvwrite('" + path + filename_raw_data + "',[dataFreq(:), davg(:)]);";

		std::cout << save_command_avg << std::endl;
		std::cout << save_command_alldata << std::endl;
		std::cout << save_command_raw_data << std::endl;
		
		engEvalString(ep, save_command_avg.c_str());

		engEvalString(ep, save_command_alldata.c_str());

		engEvalString(ep, save_command_raw_data.c_str());


	}

	return 0;
};