/*************************************************************************
 *
 * Name:   AutoLocker.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to auto-lock the master laser to Rb
 *
 * David Johnson 7/8/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include "AutoLocker.h"

//===========================================================================

AUTOLOCKER::AUTOLOCKER()
{
	//stuff	




}

//===========================================================================

void AUTOLOCKER::enable_lock() 
{

	system("putty -load \"ep-timing1\" -m enable_lock.txt");

}

//===========================================================================

void AUTOLOCKER::disable_lock() 
{
	// disable lock
	system("putty -load \"ep-timing1\" -m disable_lock.txt");

}

void AUTOLOCKER::enable_vortex_loop(bool notLocked, bool rightLock, USB1408FS &usb1408fs, GETLOCK &getLock, Vortex6000 &vortex6000)
{

	//use vortex GPIB controller to zero-out applied voltage on piezo

	feedback_signal = 0; // signal applied to current input on vortex controller from CsLock board
	set_point_voltage = 3.5; // voltage level for abs(feedback_signal) above which increment piezo_voltage by piezo_adjustment
	piezo_adjustment = 0.1; // amount to adjust piezo voltage when feedback_signal above threshold_voltage 


	//specify channel to read
	vortex_current_usb_channel = 6;

	int i;

	measured_pv = vortex6000.get_piezo_voltage();
	pv = measured_pv;

	while(!notLocked & rightLock) {
		measured_pv = vortex6000.get_piezo_voltage();
		if (pv != measured_pv) {
			std::cerr << "Discrepancy between measured & expected piezo voltage." << std::endl;
			std::cerr << "Measured PV: " << measured_pv << std::endl;
		}

		std::cerr << "piezo voltage: " << pv << std::endl;
		avg_signal = 0;

		while( fabs(avg_signal) < set_point_voltage ) {
			
			for(i=1; i < 10; i++) {
				feedback_signal = feedback_signal + usb1408fs.read_input_channel(vortex_current_usb_channel);
				Sleep(100);
			}

			avg_signal = 0.1 * feedback_signal;
			feedback_signal = 0;

			std::cerr << "avg_signal: " << avg_signal << std::endl;

			/*if(getLock.lockVoltage > 0) {
				getLock.lockVoltage = getLock.lockVoltage - 0.02;
				if (getLock.lockVoltage < 0)
					getLock.lockVoltage = 0;
				getLock.setVortexVoltage(getLock.lockVoltage, vortex6000);
				
				pv = pv - piezo_adjustment;
				std::cerr << "**piezo voltage: " << pv << std::endl;
				vortex6000.set_piezo_voltage(pv);
			}*/
		}
		
		if(avg_signal > set_point_voltage) {
			pv = pv - piezo_adjustment;
			std::cerr << "**piezo voltage: " << pv << std::endl;
			vortex6000.set_piezo_voltage(pv);
		}

		if(avg_signal < (-1.0*set_point_voltage)) {
			pv = pv + piezo_adjustment;
			std::cerr << "**piezo voltage: " << pv << std::endl;
			vortex6000.set_piezo_voltage(pv);
		}

		//check to see that feedback signal changed & thus laser is still locked
		Sleep(1000); //wait for it to settle
		feedback_signal = 0;		
		for(i=1; i < 10; i++) {
			feedback_signal = feedback_signal + usb1408fs.read_input_channel(vortex_current_usb_channel);
			Sleep(100);
			}
		check_avg_signal = 0.1 * feedback_signal;
		feedback_signal = 0;

		if( fabs(check_avg_signal - avg_signal) < 1) {
			// laser has fallen out of lock
			std::cerr << "Laser is out of lock! Fix it!" << std::endl;
			std::cerr << "\a\a\a\a\a\a\a"; //beeps
			notLocked = true;
		}
		else {
			std::cerr << "Laser is still locked. Huzzah!" << std::endl;
		}


	}
}
