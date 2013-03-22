
//#include "rs232Controller.h"

#include "PTSBCDConverter.h"

#include <iostream>
using namespace std;



int main(int argc, char* argv[])
{
//	rs232Controller* serialController = new rs232Controller("COM3", 115200, 8, "None", 1);

	PTSBCDConverter convert;

	double freq = 0;
	
//	while(freq < 10)
	while(true)
	{
		cout << "Enter Freq (0-9): ";
		cin >> freq;
		
		convert.setFrequency(freq);
		cout << endl << "SpinCore Dec: " << convert.getSpinCoreDecimal() << endl;
//		convert.set10MHzBCD(freq);
//		convert.getPTSDec();


//		set_pts(convert.getSpinCoreDecimal());
	}


	return 0;
}

