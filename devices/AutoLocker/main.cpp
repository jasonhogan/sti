

#include "ENET_GPIB_device.h"
#include "USB1408FS.h" //definition of the USB1408FS class
#include "AGILENT8648A.h"
#include "MARCONI2022D.h"
#include "Matlab.h"
#include "AutoLocker.h"
#include "RbScanner.h"
#include "WhichLock.h"
#include "GetLock.h"
#include "Vortex6000.h"

#include <windows.h>
#include <iostream> //cin & cout commands
#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage

int main(int argc, char* argv[])

{
	//define initial state
	std::vector <double> DAQ_vector;
	std::vector <double> voltage_vector;
	std::vector <double> FREQ_vector;
	std::vector <double> FITDAQ_vector;
	std::vector <double> FITFREQ_vector;
	double offsetGHz = .388;
//	double freqDiffGHz;
//	double tempVoltage;
	double GHzToMHz = 1000;

	AUTOLOCKER autolocker;
//	AGILENT8648A agilent8648a;
//	RBSCANNER rbscanner;
	WHICHLOCK whichlock;
	MATLABPLOTTER matlabplotter;
	USB1408FS usb1408fs;
	//Vortex6000 vortex6000;
//	GETLOCK getLock (vortex6000);
	//MARCONI2022D marconi2022d;

	bool notLocked = true;
	bool rightLock = false;
	bool noDiff = true;
	bool enable_lock = false;
	bool cont = false;

//	int i;
//	int new_transition;

	std::string message = "The laser is unlocked!";
	std::string subject = "laser lock status";
	std::vector <std::string> recipients;
	recipients.push_back("david.m.johnson@stanford.edu");
	recipients.push_back("sdickers@stanford.edu");

	whichlock.LockedTo(.388, matlabplotter, usb1408fs);

	std::cout << "Continue?...";
	std::cin >> cont;
	if (!cont) {return 0;}


	notLocked = false;
	rightLock = true;
	//autolocker.enable_vortex_loop(notLocked, rightLock, usb1408fs, getLock, vortex6000);


	return 0;
};
