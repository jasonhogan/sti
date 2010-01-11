// David Johnson, Kasevich Group, 12/28/2008

#include "ENET_GPIB_device.h"
#include "MARCONI2022D.h"

#include <windows.h>
#include <iostream> //cin & cout commands
#include <string> //needed for string manipulations
#include <sstream> //needed for conversion of int to string
#include <vector> //needed to be able to use vectors for data storage

int main(int argc, char* argv[])

{
	//define initial state
	double freq = 0.660;
	double MHzFreq = 700;
	double outputPower = -10.0;

	MARCONI2022D marconi2022d;

	marconi2022d.set_frequency(freq);
	marconi2022d.set_power(outputPower);
	marconi2022d.output_on();

	while(1)
	{

	std::cout << "Frequency(in MHz)?";
	std::cin >> MHzFreq;
	marconi2022d.set_frequency((MHzFreq/1000));

	}


	return 0;
};
