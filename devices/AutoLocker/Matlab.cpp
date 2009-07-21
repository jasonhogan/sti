/*************************************************************************
 *
 * Name:   Matlab.cpp
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
#include <iostream>
#include <fstream>

//===========================================================================

MATLABPLOTTER::MATLABPLOTTER()
{
	//stuff	
	// Start the MATLAB engine 
	
	if (!(ep = engOpen(NULL))) {
		MessageBox ((HWND)NULL, (LPSTR)"Can't start MATLAB engine", 
			(LPSTR) "Matlab.cpp", MB_OK);
		exit(-1);
	}
	engEvalString(ep, "iter=1;");
	engEvalString(ep, "color_codes=['r' 'g' 'b' 'k' 'y'];");

}

//===========================================================================

void MATLABPLOTTER::plotData(std::vector <double> &timeVector, std::vector <double> &signalVector, bool new_figure)
{
	
	mxArray *data_time = NULL;
	mxArray *data_Signal = NULL;
		//Scan laser


		double *time_data_ptr = (double*) calloc(timeVector.size(), sizeof(double));
		if (time_data_ptr==NULL) exit(1);
		double *Signal_data_ptr = (double*) calloc(signalVector.size(), sizeof(double));
		if (Signal_data_ptr==NULL) exit(1);

	for(unsigned int i=0;i < signalVector.size(); i++) {
		double stra = timeVector.at(i);
		
		double strb = signalVector.at(i);
		time_data_ptr[i] = stra;
		Signal_data_ptr[i] = strb;
		}


	data_time = mxCreateDoubleMatrix(1, timeVector.size(), mxREAL);
	memcpy((char*) mxGetPr(data_time), time_data_ptr, timeVector.size()*sizeof(double));

	data_Signal = mxCreateDoubleMatrix(1, timeVector.size(), mxREAL);
	memcpy((char*) mxGetPr(data_Signal), Signal_data_ptr, timeVector.size()*sizeof(double));

	/*
	 * Place the variable dataTime & dataSignal into the MATLAB workspace
	 */

	engPutVariable(ep, "dataTime", data_time);
	engPutVariable(ep, "dataSignal", data_Signal);

	/* Plot the result
	 */
	
	if (new_figure) {
		engEvalString(ep, "close;");
		engEvalString(ep, "figure;");
	}
	else {
	engEvalString(ep, "hold on;");
	}
	
	engEvalString(ep, "plot(dataTime(:)',dataSignal(:)',color_codes(mod(iter,2)+1));");
	engEvalString(ep, "title('Scope Trace');");
	engEvalString(ep, "xlabel('Time (seconds)');");
	engEvalString(ep, "ylabel('Fabry-Perot (Volts)');");
	engEvalString(ep, "iter=iter+1;");

	free(Signal_data_ptr);
	free(time_data_ptr);

}
std::string MATLABPLOTTER::generateDate()
{
	//this generates the date string for the file name
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
	return date_string;
}


void MATLABPLOTTER::savedata(unsigned int number, double frequency, double power, std::vector <double> &timeVectorOff, std::vector <double> &signalVectorOff,
							 std::vector <double> &timeVectorSerrodyne, std::vector <double> &signalVectorSerrodyne)
{
	std::ofstream myfile;
	std::string filename_raw_data;

	std::string numberString;
	std::stringstream convert;
	convert << number;
	convert >> numberString;

	filename_raw_data = "serrodyneRawData on " + generateDate() + " " + numberString + ".csv"; 
	std::string path = "\\\\atomsrv1\\EP\\Data\\SerrodyneAutoSaveHighFreqLowPower\\";

	//std::string save_command_raw_data = "csvwrite('" + path + filename_raw_data + "',[dataTime(:), dataSignal(:)]);";

	//engEvalString(ep, save_command_raw_data.c_str());

	std::string fullPath = path + filename_raw_data;

	myfile.open(const_cast<char*>( fullPath.c_str()) );
	myfile << "Function generator frequency: " << frequency << " MHz" << std::endl;
	myfile << "Function generator output power: " << power << " dBM" << std::endl;
	myfile << "timeVectorOff, signalVectorOff, timeVectorSerrodyne, signalVectorSerrodyne" << std::endl;
	for(unsigned int i = 0; i < timeVectorOff.size(); i++)
	{
		myfile << timeVectorOff.at(i) << ", " << signalVectorOff.at(i) << ", " << timeVectorSerrodyne.at(i) << ", " << signalVectorSerrodyne.at(i) << std::endl;
	}
	myfile.close();

}


void MATLABPLOTTER::sendmail(std::string message, std::string subject, std::vector <std::string>& recipients)
{
	unsigned int i, j;
	unsigned int maxLength;
	std::string tempString = "";
	std::string recipientCommand;
	std::string messageCommand;
	

	for(i = 0, maxLength = recipients.at(0).size(); i < recipients.size(); i++)
	{
		if (recipients.at(i).size() > maxLength){
			maxLength = recipients.at(i).size();
		}
	}

	for (i = 0; i < recipients.size(); i++)
	{
		tempString += "'" + recipients.at(i);
		for (j = 0; j < maxLength - recipients.at(i).size(); j++)
		{
			tempString += " ";
		}
		if (i != recipients.size()-1){
			tempString += "' ; ";
		}
		else {
			tempString += "'";
		}
	}
	
	recipientCommand = "email_char_array = [" + tempString + "];";
	messageCommand = "sendmail(email_cell_array, '" + subject + "', '" + message + "');";

	engEvalString(ep, recipientCommand.c_str());
	engEvalString(ep, "email_cell_array = cellstr(email_char_array);");
	engEvalString(ep, "setpref('Internet', 'E_mail', 'TheRubidiumLock@stanford.edu');");
	engEvalString(ep, "setpref('Internet','SMTP_Server','smtp.stanford.edu');");
	engEvalString(ep, messageCommand.c_str());
}

//===========================================================================