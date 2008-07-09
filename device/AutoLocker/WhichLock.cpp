/*************************************************************************
 *
 * Name:   WhichLock.cpp
 *
 * C++ Windows source code for determining the locking transition
 *
 * Susannah Dickerson 7/8/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

#include "WhichLock.h"


// Constructor
WHICHLOCK::WHICHLOCK ()
{
	 //NOTE-- All frequencies referenced to the Rb 85 F=3 to F'=4 transition
	//     for increased precision.
	double cFreqsGHz[] = {5.212381925, 5.284603925, 5.441544425, -1.550078686,
		-1.393138186, -1.126486086, 2.822319439, 2.851692439, 2.915092439,
		-0.184040000, -0.120640000, 0.000000000, 5.248492925, 5.326963175,
		5.363074175, -1.471608436, -1.338282386, -1.259812136, 2.837005939,
		2.868705939, 2.883392439, -0.152340000, -0.092020000, -0.060320000};
	std::string cLabels[] = {"Rb87 repump F=1 to F'=0 transition",
		"Rb87 repump F=1 to F'=1 transition", 
		"Rb87 repump F=1 to F'=2 transition",
		"Rb87 cooling F=2 to F'=1 transition", 
		"Rb87 cooling F=2 to F'=2 transition",
		"Rb87 cooling F=2 to F'=3 transition", 
		"Rb85 repump F=2 to F'=1 transition",
		"Rb85 repump  F=2 to F'=2 transition", 
		"Rb85 repump F=2 to F'=3 transition",
		"Rb85 cooling F=3 to F'=2 transition", 
		"Rb85 cooling F=3 to F'=3 transition",
		"Rb85 cooling F=3 to F'=4 transition", 
		"Rb87 repump F'=0 to F'=1 crossover",
		"Rb87 repump F'=0 to F'=2 crossover", 
		"Rb87 repump F'=1 to F'=2 crossover",
		"Rb87 cooling F'=1 to F'=2 crossover", 
		"Rb87 cooling F'=1 to F'=3 crossover",
		"Rb87 cooling F'=2 to F'=3 crossover", 
		"Rb85 repump F'=1 to F'=2 crossover",
		"Rb85 repump  F'=1 to F'=3 crossover", 
		"Rb85 repump F'=2 to F'=3 crossover",
		"Rb85 cooling F'=2 to F'=3 crossover", 
		"Rb85 cooling F'=2 to F'=4 crossover",
		"Rb85 cooling F'=3 to F'=4 crossover"};
	int i;

	freqsGHz = new double [LABELLENGTH];
	labels = new std::string [LABELLENGTH];

	lockedTo = -1;

	for (i = 0; i < LABELLENGTH; i++)
	{
		freqsGHz[i] = cFreqsGHz[i];
		labels[i] = cLabels[i];
	}

	windowGHz = 0.04;

	//Rb Scanner	
	start_freq = 1.4; // start point in GHz
    freq_incr = .003; //increment frequency in GHz
    end_freq = 2.4; // endpoint in GHz   
    rf_power = 0.8; // output power in dBm
	usb_channel = 7;

	//set the output power in dBm
	hp83711b.set_power(0);

}



// Destructor
WHICHLOCK::~WHICHLOCK ()
{
	delete[] freqsGHz;
	delete[] labels;
}


/*************************************************************************
 * freqDiff-- Public
 * Description-- given a new transition, accesses the stored lock
 *         transition and calculates the frequency difference.
 * Input-- newTransition, an integer corresponding to the position of the
 *             new transition in freqsGHz.
 *         freqDiffGHz, a pointer to the location to store the difference.
 * Return-- bool. False if finding the frequency difference was successful.
 *************************************************************************/
bool WHICHLOCK::freqDiff (int newTransition, double* freqDiffGHz)
{
	if (newTransition > LABELLENGTH || newTransition < 0) {
		std::cerr << "Error WHICHLOCK::freqDiff-- unrecognized transition" <<
			std::endl;
		return (1);
	}

	if (lockedTo == -1) {
		std::cerr << "Error WHICHLOCK::freqDiff -- freqDiff called " << 
			"without initialization of lock." << std::endl;
	}

	*freqDiffGHz = freqsGHz[newTransition] - freqsGHz[lockedTo];

	return (0);
}

/*************************************************************************
 * LockedTo-- Public
 * Description-- determines which transition the laser is locked to
 * Input-- DAQ_vector, a vector of amplitudes
 *         FREQ_vector, the corresponding frequencies
 *         offsetGHz, the offset of the sidebands from the laser
 *             frequency, in GHz
 *         usb1408fs, address of the usb object
 * Output-- a description of the locking line.
 * Return-- bool, False if a lock is found, True if an error occurs.         
 * Requires-- IsLocked, freqsGHz, and labels.
 *************************************************************************/
bool WHICHLOCK::LockedTo(double offsetGHz, MATLABPLOTTER &matlabplotter, 
						 USB1408FS &usb1408fs)
{
	int i;
	bool lockTestList[LABELLENGTH];
	double error;
	int numLocks = 0;
	std::vector <double> DAQ_vector;
	std::vector <double> FREQ_vector;
	std::vector <double> FITDAQ_vector;
	std::vector <double> FITFREQ_vector;

	//run RbScanner
	scan_rb (FREQ_vector, DAQ_vector, usb1408fs);

	// Test each transition
	for (i = 0; i < LABELLENGTH; i++)
	{
		lockTestList[i] = isLocked (DAQ_vector, FREQ_vector,
			freqsGHz[i] + offsetGHz,&error, FITDAQ_vector, FITFREQ_vector);
		if(lockTestList[i]){
			std::cerr << "Locked to the " << labels[i] << "." << std::endl;
			std::cerr << "Error in lock: " << error << " GHz." << std::endl;
			lockedTo = i;
			numLocks++;
		
			plot(DAQ_vector, FREQ_vector, FITDAQ_vector, FITFREQ_vector,
				matlabplotter);
		}
	}

	// Check whether only one lock is truly possible
	if (numLocks != 1) {
		std::cerr << "Undetermined Lock." << std::endl;
		std::cerr << numLocks << " possible locks found." << std::endl;
		std::cerr << "Try increasing WHICHLOCK::windowGHz. " <<
			"Default: 0.04 GHz" << std::endl;

		return (1);
	}

	return (0);
}


/*************************************************************************
 * scan_rb-- Public
 * Description-- scans sidebands to produce a Rb spectrum
 * Input-- FREQ_vector, a vector in which the frequencies will be stored
 *		   DAQ_vector, a vector to store the corresponding amplitudes
 *         usb1408fs, the address of the usb object
 * Output-- none
 * Requires-- getParameters()
 *************************************************************************/

void WHICHLOCK::scan_rb(std::vector <double> &FREQ_vector,
						std::vector <double> &DAQ_vector, USB1408FS &usb1408fs)
{
	//Scan laser

	double freq; //frequency in GHz
	
	getParameters();
	freq = start_freq; //frequency in GHz

	//prepare the hp83711b frequency synthesizer to scan 10 MHz at a time
	hp83711b.set_freq_increment(freq_incr);

	//set the start frequency at 1 GHz
	hp83711b.set_frequency(freq);

	//set the output power in dBm
	hp83711b.set_power(rf_power);

	hp83711b.output_on();

	hp83711b.set_frequency(freq);
	
	Sleep(100);

	//loop from start freq to start + interval
	while(freq <= end_freq) {

		FREQ_vector.push_back(freq); //record function generator frequency

		//take data
		DAQ_vector.push_back(usb1408fs.read_input_channel(usb_channel)); 

		// change the frequency
		hp83711b.increment_frequency_up();
		freq = freq + freq_incr;

		//wait for the function generator to settle. spec'd time is 20ms
		Sleep(50); 

	}	
}

/*************************************************************************
 * plot-- Private
 * Description-- plots the data in matlab
 * Input-- DAQ_vector, a vector of amplitudes.
 *		   FREQ_vector, a vector of correspoinding frequencies.
 *         FITDAQ_vector, a vector of amplitudes to be plotted as points.
 *         FITFREQ_vector, a vector of corresponding frequencies.
 *         matlabplotter, the address of the MATLABPLOTTER object
 * Output-- none
 *************************************************************************/
void WHICHLOCK::plot(std::vector <double>& DAQ_vector,
					 std::vector <double>& FREQ_vector,
					 std::vector <double>& FITDAQ_vector,
					 std::vector <double>& FITFREQ_vector,
					 MATLABPLOTTER &matlabplotter)
{
	bool save_data = true;

	matlabplotter.plotfreqscan(FREQ_vector, DAQ_vector);

	matlabplotter.plotlockpoints(FITFREQ_vector, FITDAQ_vector);

	std::cout << "Do you want to save the data (1/0)?";
    std::cin >> save_data;

	matlabplotter.savedata(save_data);
}

/*************************************************************************
 * isLocked-- Private
 * Description-- determines whether the laser is locked to a specific 
 *         transition, given a lockpoint. IsLocked predicts where the Rb87      
 *         cooling transitions should be found and checks for maximums.        
 * Input-- DAQ_vector, a vector of amplitudes
 *         FREQ_vector, the corresponding frequencies 
 *         lockpoint, the reference transition frequency plus the offset, 
 *             in GHz.
 *         error_p, pointer to where the error in the lock should be stored
 *         FITDAQ_vector, the vector where the expected peak amplitudes
 *             should be stored.
 *         FITFREQ_vector, the vector where the expected peak frequencies
 *             should be stored
 * Return-- True or False for locked or unlocked respectively.
 * Requires-- testForPeaks, buildKeyFreq
 *************************************************************************/
bool WHICHLOCK::isLocked(std::vector <double>& DAQ_vector,
						 std::vector <double>& FREQ_vector,
						 double lockpointGHz, double* error_p,
						 std::vector<double> &FITDAQ_vector,
						 std::vector<double> &FITFREQ_vector)
{
	int i;
	double range[2];
	double rangeTemp;
	double keyFreq[KEYLENGTH];
	double diffs[KEYLENGTH];
	int trueMax[KEYLENGTH];
	int pos;
	bool inRange;
	bool foundPeaks;

	range[0]=FREQ_vector.front();
	range[1]=FREQ_vector.back();

	// range[0] must be less than range[1] for isInRange within buildKeyFreq
	if (range[0] > range[1]) {
		rangeTemp = range[0];
		range[0] = range[1];
		range[1] = rangeTemp;
	}

	// determine whether the key Rb 87 cooling transitions should be in the
	//    range of the scan.
	inRange = buildKeyFreq(keyFreq, lockpointGHz, range);
	if (!inRange) {return (0);}

	// test whether peaks are found at the expected frequencies
	foundPeaks = testForPeaks(DAQ_vector, FREQ_vector, keyFreq, trueMax);

	if (foundPeaks) {
		
		// Calculate the error in the lock determination.
		for (i = 0; i < KEYLENGTH; i++)
		{
			diffs[i] = FREQ_vector.at(trueMax[i]) - keyFreq[i];
		}

		*error_p = findErr(diffs, KEYLENGTH);

		// Prepare the vector containing the info about the expected peaks
		FITFREQ_vector.clear();
		FITDAQ_vector.clear();
		for (i = 0; i < KEYLENGTH; i++)
		{
			pos = position(FREQ_vector, keyFreq[i]);
			FITFREQ_vector.push_back(FREQ_vector.at(pos));
			FITDAQ_vector.push_back(DAQ_vector.at(pos));
		}
		
		return(1);
	}

	return (0);
}



/*************************************************************************
 * buildKeyFreq-- Private
 * Description-- determines which key frequencies (Rb 87 cooling) fall 
 *         into the range scanned. Accounts for mirror images about the
 *         locking frequency.
 * Input-- keyFreqGHz, a pointer to an array in which the key frequencies
 *             are to be stored.
 *         lockpoint, the reference transition frequency plus the offset, 
 *             in GHz.
 *         range, the range of the scanned spectrum; a pointer to a array
 *             of two elements
 * Return-- True if all three key Rb 87 cooling transitions are found in
 *         the data. False if one or more do not.
 * Requires-- isInRange
 *
 * NOTE-- All frequencies referenced to the Rb 85 F=3 to F'=4 transition
 *		  for increased precision. 
 *		  The KeyFreq87GHz are {Rb 87 F'=1 and F'=3 crossover, Rb 87 F'=2 
 *		  and F'=3 crossover, Rb 87 F=2 to F'=3 transition} respectively.
 *		  Any changes to this length requires an edit of KEYLENGTH
 *        #define'd in the header.
 *************************************************************************/
bool WHICHLOCK::buildKeyFreq(double* keyFreqGHz, 
							 double lockpointGHz, double* range)
{
	double KeyFreq87GHz[]={-1.338282386, -1.259812136, -1.126486086}; 
	int i;

	for (i = 0; i < 3; i++)
	{
		keyFreqGHz[i] = KeyFreq87GHz[i] - lockpointGHz;
	}

	//Try reversing the sign of the key frequencies if they are not in range
	if (!isInRange(keyFreqGHz,3,range)) {
		for (i = 0; i < 3; i++)
		{
			keyFreqGHz[i] = -(KeyFreq87GHz[i] - lockpointGHz);
		}
		//Check whether the reveresed frequencies are in range
		if (!isInRange(keyFreqGHz, 3, range))
		{
			return (0);
		}
	}

	return (1);
}



/*************************************************************************
 * isInRange-- Private
 * Description-- determines whether given frequencies fall into a given 
 *         range.             
 * Input-- freqList, a pointer to an array in which the frequencies of
 *             interest are stored.
 *         length, the length of freqList. 
 *         range, the range of the scanned spectrum; a pointer to a array
 *             of two elements
 * Return-- True if all frequencies are found in the indicated range.
 *         False if one or more do not.
 *************************************************************************/
bool WHICHLOCK::isInRange(double* freqList, int length, double* range)
{
	int i;

	for (i = 0; i < length; i++)
	{
		if(freqList[i] < range[0] || freqList[i] > range[1]){
			return (0);
		}
	}

	return (1);
}



/*************************************************************************
 * testForPeaks-- Private
 * Description-- determines whether given frequencies are located near
 *         peaks of the spectrum. The global variable WHICHLOCK::windowGHz
 *         is roughly related to the width of the peak looked for.
 * Input-- DAQ_vector, a vector of amplitudes
 *         FREQ_vector, the corresponding frequencies
 *         keyFreq, a pointer to an array of frequencies where peaks are
 *             expected
 *         trueMax, a pointer to an array of positions at which the true
 *             maximums of the data near the keyFreqs are located
 * Return-- a bool. True if all keyFreqs are at peaks, False otherwise.
 *************************************************************************/
bool WHICHLOCK::testForPeaks(std::vector <double>& DAQ_vector,
							 std::vector <double>& FREQ_vector,
							 double* keyFreq, int* trueMax)
{
	int i;
	int j;
	double resolution = fabs(FREQ_vector.at(1)-FREQ_vector.at(0));
	int steps = (int) ceil(WHICHLOCK::windowGHz/resolution);
	int bounds[]={-steps, 0 , steps};
	int posList[KEYLENGTH][3];
	std::vector <double> ampVector;
	

	for (i = 0; i < KEYLENGTH; i++)
	{
		for (j = 0; j < 3; j++)
		{
			// Find the vector position a given number of "steps" away from
			//     the expected max.
			posList[i][j] = position(FREQ_vector, keyFreq[i]) + bounds[j];
			if(posList[i][j] < 0 || 
				posList[i][j] > (signed int) FREQ_vector.size() - 1) {
				std::cerr << "WHICHLOCK::testForPeak--Peaks expected close " <<
					"to end of frequency scan. Possibly expand scan by " <<
					WHICHLOCK::windowGHz << " GHz." << std::endl;
				return (0);
			}
			else {
				// Record the amplitude at the three positions
				ampVector.push_back(DAQ_vector.at(posList[i][j]));
			}
		}

		if (findMax(ampVector, 0, 2) != 1) {
			return (0);
		}
		else {
			// Record the location of the actual peak for error analysis
			//     in isLocked
			trueMax[i] = findMax(DAQ_vector, posList[i][0], posList[i][2]);
		}
		ampVector.clear();
	}

	return (1);
}


/*************************************************************************
 * position-- Private
 * Description-- Finds the first element in the vector that most nearly 
 *         matches the given element.
 * Input-- myVector, a vector of elements to be searched
 *         element, the element to be looked for.
 * Return-- the position in myVector of the element.
 * NOTE-- this function works only when myVector increases or decreases
 *         monotonically
 *************************************************************************/
int WHICHLOCK::position(std::vector <double>& myVector, double element)
{
	double oldDiff = fabs(myVector.at(0)-element);
	double newDiff = fabs(myVector.at(1)-element);
	int i = 1;	

	while(newDiff < oldDiff && i < (signed int) myVector.size() - 1)
	{
		i++;
		oldDiff = newDiff;
		newDiff = fabs(myVector.at(i)-element);
	}

	if(i == myVector.size() + 1){
		std::cerr << "WHICHLOCK::position--Warning: nearest value" <<
			" at end of vector" << std::endl;
	}

	return (i - 1);
}



/*************************************************************************
 * findMax-- Private
 * Description-- return the position of the largest element of the vector
 *         between the positions "start" and "end".
 * Input-- myVector, a vector of elements to be searched.
 *         start, the first element to be compared.
 *         end, the last element to be compared.
 * Return-- the position in myVector of the element.
 *************************************************************************/
int WHICHLOCK::findMax(std::vector <double>& myVector,
					   unsigned int start, unsigned int end)
{
	unsigned int i;
	double tempMax = myVector.at(start);
	unsigned int tempMaxPos = start;

	if (end >= myVector.size()) {
		std::cerr << "Error in WHICHLOCK::findMax" << std::endl;
	}

	for (i = start + 1; i <= end; i++)
	{
		if (myVector.at(i) > tempMax)
		{
			tempMax = myVector.at(i);
			tempMaxPos = i;
		}
	}

	return (tempMaxPos);
}



/*************************************************************************
 * findErr-- Private
 * Description-- use a least squares method to find the error in the lock
 *         prediction.
 * Input-- diffs, a pointer to an array of differences between the
 *             location in frequency of the true maximum of the data and
 *			   the expected location.
 *         length, the length of the array.
 * Return-- the error in the lock prediction.
 * Requires-- leastSquaresSum
 *************************************************************************/
double WHICHLOCK::findErr (double* diffs, int length)
{
	double stepSize = 0.001;
	double stepSum = 0;
	double newSum;
	double oldSum;
	int minFine = 5; //precision: stepSize/10^(minFine + 1)
	int fineness = 0;
	int sign = 1;
	int i;

	while (fineness <= minFine)
	{
		stepSize /= 10;

		// Initialize oldSum and newSum
		i = 0;
		oldSum = leastSquaresSum(diffs, length, stepSum + sign*i*stepSize);
		i = 1;
		newSum = leastSquaresSum(diffs, length, stepSum + sign*i*stepSize);

		// Check direction to iterate
		if (newSum > oldSum) {
			sign = -1;
			newSum = leastSquaresSum(diffs, length, stepSum + sign*i*stepSize);
			if (newSum > oldSum) {fineness++; continue;}
		}

		// Iterate until you find the minimum
		while (newSum < oldSum)
		{
			i++;
			oldSum = newSum;
			newSum = leastSquaresSum(diffs, length, stepSum + sign*i*stepSize);
		}

		// Record the distance to the minimum
		// Reset the fineness of the step size.
		stepSum += sign*(i - 1)*stepSize;
		fineness++;
	}

	return (stepSum);
}



/*************************************************************************
 * leastSquaresSum-- Private
 * Description-- find the sum of the squares of the elements of myArray,
 *         each offset by the same step.
 * Input-- myArray, a pointer to an array.
 *         length, the length of the array.
 *         step, the offset to be applied to each element.
 * Return-- the sum of the squares.
 *************************************************************************/
double WHICHLOCK::leastSquaresSum(double* myArray, int length, double step)
{
	int i;
	double sum = 0;

	for (i = 0; i < length; i++)		
	{
		sum += pow(myArray[i] + step, 2);
	}

	return (sum);
}



/*************************************************************************
 * getParameters-- Private
 * Description-- Prints default RbScanner values and accepts user input to
 *         change them.
 * Input-- user input
 * Output-- user instructions
 *************************************************************************/
void WHICHLOCK::getParameters ()
{
    bool change_vals = true; // have user defined values

	std::cout << "default values are as follows:" << std::endl;
	std::cout << "USB input channel: " << usb_channel << std::endl;
    std::cout << "Start Frequency: " << start_freq << " GHz" << std::endl;
    std::cout << "End Frequency: " << end_freq << " GHz" << std::endl;
    std::cout << "Frequency Increment: " << freq_incr << " GHz" << std::endl;
    std::cout << "Output Power: " << rf_power << " dBm" << std::endl;
    std::cout << std::endl << "Do you want to change (1/0)? ";
    std::cin >> change_vals; 

    if(change_vals) {

        // user defined start frequency
        std::cout << "Enter desired start frequency in GHz. " <<
			"Default value is: " << start_freq << " GHz" << std::endl <<
			"start freq = ";
        std::cin >> start_freq;
   
        std::cout << "Enter desired end frequency in GHz. " <<
			"Default value is: " << end_freq << " GHz" << std::endl << 
			"end freq = ";
        std::cin >> end_freq;
   
        std::cout << "Enter desired frequency increment in GHz." <<
			" Default value is: " << freq_incr << " GHz" << std::endl <<
			"freq increment = ";
        std::cin >> freq_incr;
   
        std::cout << "Enter desired output power in dBm." << 
			" Default value is: " << rf_power << " dBm" << std::endl <<
			"power = ";
        std::cin >> rf_power;
    }
}
