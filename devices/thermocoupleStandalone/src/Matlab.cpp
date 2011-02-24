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

#include "Matlab.h"


MATLABPLOTTER::MATLABPLOTTER()
{
	//stuff	
	// Start the MATLAB engine 
	
	if (!(ep = engOpen(NULL))) {
		MessageBox ((HWND)NULL, (LPCWSTR)"Can't start MATLAB engine", 
			(LPCWSTR) "thermocoupleStandaloneMain.cpp", MB_OK);
		exit(-1);
	}


}
std::string MATLABPLOTTER::createFilename()
{
	// creates a date & time stamped data file
	std::string filename;

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

	filename = "thermocouple data on " + date_string + ".csv"; 

	return filename;

}
void MATLABPLOTTER::saveVector(std::string filename)
{
		
	std::string path = "\\\\epsrv1\\EP\\data\\ThermocoupleAutoSave\\";
	std::string save_command = "dlmwrite('" + path + filename + "', inputData, '-append');";

	
	engEvalString(ep, save_command.c_str());

}
void MATLABPLOTTER::plotVector(std::vector <double> &inputVector)
{
	
	mxArray *data_input = NULL;

		double *input_data_ptr = (double*) calloc(inputVector.size(), sizeof(double));
		if (input_data_ptr==NULL) exit(1);


	for(unsigned int i=0;i < inputVector.size(); i++) {
		double stra = inputVector.at(i);
		
		input_data_ptr[i] = stra;
		}


	data_input = mxCreateDoubleMatrix(1, inputVector.size(), mxREAL);
	memcpy((char*) mxGetPr(data_input), input_data_ptr, inputVector.size()*sizeof(double));

	/*
	 * Place the variable data_input into the MATLAB workspace
	 */

	engEvalString(ep, "clear inputData;");
	engPutVariable(ep, "inputData", data_input);

	/* Plot the result
	 */
	
	engEvalString(ep, "close all;");
	engEvalString(ep, "figure(1);");
	engEvalString(ep, "plot(inputData);");


	free(input_data_ptr);

}

void MATLABPLOTTER::plotfreqscan(std::vector <double> &FREQ_vector, std::vector <double> &DAQ_vector)
{
	
	mxArray *data_freq = NULL;
	mxArray *data_DAQ = NULL;
		//Scan laser


		double *freq_data_ptr = (double*) calloc(FREQ_vector.size(), sizeof(double));
		if (freq_data_ptr==NULL) exit(1);
		double *DAQ_data_ptr = (double*) calloc(DAQ_vector.size(), sizeof(double));
		if (DAQ_data_ptr==NULL) exit(1);

	for(unsigned int i=0;i < DAQ_vector.size(); i++) {
		double stra = FREQ_vector.at(i);
		
		double strb = DAQ_vector.at(i);
		freq_data_ptr[i] = stra;
		DAQ_data_ptr[i] = strb;
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

}


void MATLABPLOTTER::plotlockpoints(std::vector <double> &FITFREQ_vector, std::vector <double> &FITDAQ_vector)
{
	
	mxArray *fit_data_freq = NULL;
	mxArray *fit_data_DAQ = NULL;
		//Scan laser


		double *fit_freq_data_ptr = (double*) calloc(FITFREQ_vector.size(), sizeof(double));
		if (fit_freq_data_ptr==NULL) exit(1);
		double *fit_DAQ_data_ptr = (double*) calloc(FITDAQ_vector.size(), sizeof(double));
		if (fit_DAQ_data_ptr==NULL) exit(1);

	for(unsigned int i=0;i < FITDAQ_vector.size(); i++) {
		double stra = FITFREQ_vector.at(i);
		
		double strb = FITDAQ_vector.at(i);
		fit_freq_data_ptr[i] = stra;
		fit_DAQ_data_ptr[i] = strb;
		}


	fit_data_freq = mxCreateDoubleMatrix(1, FITFREQ_vector.size(), mxREAL);
	memcpy((char*) mxGetPr(fit_data_freq), fit_freq_data_ptr, FITFREQ_vector.size()*sizeof(double));

	fit_data_DAQ = mxCreateDoubleMatrix(1, FITFREQ_vector.size(), mxREAL);
	memcpy((char*) mxGetPr(fit_data_DAQ), fit_DAQ_data_ptr, FITFREQ_vector.size()*sizeof(double));

	/*
	 * Place the variable dataFreq & dataDAQ into the MATLAB workspace
	 */

	engPutVariable(ep, "fitdataFreq", fit_data_freq);
	engPutVariable(ep, "fitdataDAQ", fit_data_DAQ);

	/* Plot the result
	 */
	engEvalString(ep, "i=1");
	for(unsigned int j=1; j <= FITFREQ_vector.size(); j++) {
		engEvalString(ep, "hold on;");
		engEvalString(ep, "plot(fitdataFreq(i)',fitdataDAQ(i)', 's','MarkerSize',10, 'MarkerFaceColor','g','MarkerEdgeColor', 'r');");
		engEvalString(ep, "i=i+1;");
	}

	free(fit_DAQ_data_ptr);
	free(fit_freq_data_ptr);

}


void MATLABPLOTTER::savedata(bool save_data)
{
	
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
		std::string save_command_raw_data = "csvwrite('" + path + filename_raw_data + "',[dataFreq(:), dataDAQ(:)]);";

	
		engEvalString(ep, save_command_plot.c_str());
		engEvalString(ep, save_command_raw_data.c_str());
	}

}
