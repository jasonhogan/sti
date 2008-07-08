/*************************************************************************
 *
 * Name:   GetLock.h
 *
 * C++ Windows header for scanning and locking the Vortex
 *
 * Susannah Dickerson 7/8/2008
 * Dave Johnson
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

#include "GetLock.h"


// Constructor
GETLOCK::GETLOCK()
{
	lockVoltage = 0;
	start_voltage = 0; // start point in GHz
	voltage_incr = .001; //increment frequency in GHz
	end_voltage = .260; // endpoint in GHz   
	usb_input_channel = 3;
	usb_output_channel = 0;

	GHzToV = .062857142857; // .700 GHz == .044 V
}

// Destructor
GETLOCK::~GETLOCK()
{
}


bool GETLOCK::lock (double* offsetGHz_p, MATLABPLOTTER &matlabplotter, AGILENT8648A &agilent8648a)
{
	double globalMinV;
	double rangeV = 0.45 * GHzToV; // roughly twice the expected width of the Rb 85 cooling transitions.
	double shiftV = .008; // roughly the piezo voltage difference between the global minimum and the Rb 85 cooling peak. 
	double offsetGHz;
	double globalMaxV;
	bool noScan;
	bool noLock;

	std::vector <double> voltage_vector;
	std::vector <double> DAQ_vector;

	getParameters();

	noScan = scan(voltage_vector, DAQ_vector);
	if (noScan) {return (1);)
	plot(voltage_vector, DAQ_vector);

	globalMinV = findGlobalMin (voltage_vector, DAQ_vector);

	std::cout << "Offset Lock frequency: " << *offsetGHz_p << " GHz" << std::endl;

	std::cout << "Enter desired offset lock frequency in GHz. Default value is: " << *offsetGHz_p << " GHz" << std::endl << "frequency = ";
	std::cin >> *offsetGHz_p;

	agilent8648a.output_on();
	agilent8648a.set_frequency(*offsetGHz_p);

	start_voltage = globalMinV - shiftV + *offsetGHz_p * GHzToV - rangeV / 2;
	end_voltage = globalMinV - shiftV + *offsetGHz_p * GHzToV + rangeV / 2;

	voltage_vector.clear();
	DAQ_vector.clear();
		
	noScan = scan(voltage_vector, DAQ_vector);
	if (noScan) {return (1);)
	plot(voltage_vector, DAQ_vector, matlabplotter);

	globalMaxV = findGlobalMax(voltage_vector, DAQ_vector);
	if (noLock) {return (1);}

	setLockVoltage(globalMaxV);

	return(0);
}

void GETLOCK::getParameters ()
{
    bool change_vals = true; // have user defined values

	std::cout << "default values are as follows:" << std::endl;
	std::cout << "USB input channel: " << usb_input_channel << std::endl;
	std::cout << "USB output channel: " << usb_output_channel << std::endl;
    std::cout << "Start Voltage: " << start_voltage << " GHz" << std::endl;
    std::cout << "End Voltage: " << end_voltage << " GHz" << std::endl;
    std::cout << "Voltage Increment: " << voltage_incr << " GHz" << std::endl;
    std::cout << std::endl << "Do you want to change (1/0)? ";
    std::cin >> change_vals; 

    if(change_vals) {

        // user defined start frequency
        std::cout << "Enter desired start voltage in volts. Default value is: " << start_voltage << " V" << std::endl << "start voltage = ";
        std::cin >> start_voltage;
   
        std::cout << "Enter desired end voltage in volts. Default value is: " << end_voltage << " V" << std::endl << "end voltage = ";
        std::cin >> end_voltage;
   
        std::cout << "Enter desired voltage increment in volts. Default value is: " << voltage_incr << " V" << std::endl << "voltage increment = ";
        std::cin >> voltage_incr;
    }
}


void GETLOCK::setLockVoltage (double voltage)
{
	USB1408FS usb1408fs;

	lockVoltage = voltage;

	usb1408fs.set_output_voltage(usb_output_channel, lockVoltage);
}

void GETLOCK::plot(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector, MATLABPLOTTER &matlabplotter)
{
	matlabplotter.plotfreqscan(voltage_vector, DAQ_vector);

	std::cout << "Do you want to save the data (1/0)?";
    std::cin >> save_data;

	matlabplotter.savedata(save_data);
}



bool GETLOCK::scan (std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector)
{
	USB1408FS usb1408fs;
	double voltage = start_voltage;
	bool noScan;

	while(voltage <= end_voltage) {

		usb1408fs.set_output_voltage(usb_output_channel, voltage); //set the output voltage to the back of the vortex

		voltage_vector.push_back(voltage); //record DAQ output voltage

		DAQ_vector.push_back(usb1408fs.read_input_channel(usb_input_channel)); //take data

		// change the frequency
		voltage = voltage + voltage_incr;

		Sleep(10); //wait for the DAQ to settle. spec'd rate is 10 KS/s
	}

	if (voltage_vector.size() == 0 || DAQ_vector.size() == 0) {
		std::cerr <<std::endl << "Error in GETLOCK::scan-- no data received" << std::endl;
		return (1);
	}

	return (0);
}




/*************************************************************************
 * findGlobalMin-- Public
 * Description-- return the laser voltage corresponding to the smallest 
 *             element of the photodiode voltage.
 * Input-- voltage_vector, a vector of laser voltages.
 *         DAQ_vector, a vector of corresponding photodiode voltages.
 * Return-- the value of LASERV_vector corresponding to the smallest
 *             element of PDV_vector.
 *************************************************************************/
double GETLOCK::findGlobalMin(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector)
{
	int i;
	double tempMin = DAQ_vector.at(0);
	unsigned int tempMinPos = 0;

	if ((unsigned int) end >= DAQ_vector.size()) {
		std::cerr << "Error in IDTRANSITIONS::findGlobalMin" << std::endl;
	}

	for (i = start + 1; i <= end; i++)
	{
		if (DAQ_vector.at(i) < tempMin)
		{
			tempMin = DAQ_vector.at(i);
			tempMinPos = i;
		}
	}

	return (voltage_vector.at(tempMinPos));
}


/*************************************************************************
 * findGlobalMax-- Public
 * Description-- return the laser voltage corresponding to the greatest 
 *             element of the photodiode voltage.
 * Input-- LASERV_vector, a vector of laser voltages.
 *         PDV_vector, a vector of corresponding photodiode voltages.
 * Return-- the value of LASERV_vector corresponding to the greatest
 *             element of PDV_vector.
 *************************************************************************/
double GETLOCK::findGlobalMax(std::vector <double>& voltage_vector, std::vector <double>& DAQ_vector)
{
	unsigned int i;
	unsigned int tempMaxPos = 0;
	double tempMax = DAQ_vector.at(0);
	double globalMin;
	float nearEnd = .1;

	for (i = 0; i < DAQ_vector.size(); i++)
	{
		if (DAQ_Vector.at(i) < tempMax)
		{
			tempMax = DAQ_vector.at(i);
			tempMaxPos = i;
		}
	}

	if ((float) i/DAQ_vector.size() > 1 - nearEnd || (float) i/DAQ_vector.size() < nearEnd) {
		std::cerr << "Warning: Transition found near edge of scan" << std::endl;
	}

	return (voltage_vector.at(tempMaxPos));
}