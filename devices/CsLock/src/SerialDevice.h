#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

// Filename: SerialDevice.h
//
// Purpose: 


//ASSUMES:  #define DIOLatch16 FALSE	
// set TRUE for original DIO Latch board (16 out/16 in), FALSE for new (24 out/8 in)



#include <string>
#include <vector>
#include "SerialData.h"
#include "utils.h"

// libraries for implementing direct access to CPU addresses for timing-board
#ifndef _MSC_VER
//This should only get loaded when cross compiling (i.e., never in windows).
//However, we should use a better preprocessor flag than this hack...
#include <err.h>
#endif

#ifdef HAVE_BUS_SPACE_H
extern "C" {
#  include <bus_space.h>
}
#endif

#ifdef _MSC_VER
void _stdcall Out32(short PortAddress, short data);
#endif

class SerialDevice
{
public:

	SerialDevice(std::string deviceName);
	SerialDevice(std::string deviceName, int address);
	virtual ~SerialDevice() {};

	//Access functions
//	virtual DeviceID deviceID() const=0;
	virtual std::string deviceType() const=0;
	virtual std::string deviceName() const;
	void setDeviceName(std::string deviceName);
	void setLPT1Address(int address);
	int LPT1Address();

	void setAddress(int address);
	int address();

	void writeData(SerialData data);

	// implement multiple writes to CPU address at a time for speed increase 3/19/2008
	void setupAddress();
	
	std::vector<SerialData> SerBuf;

	int DSerialBufferBuild(unsigned long data, int numbits, 
							int location, bool bEnablePolarity);

	void DSerialSendData(int address, int length);


private:

	int address_l;
	std::string deviceName_l;

	int	LPT1Address_l;

#ifdef HAVE_BUS_SPACE_H
	// variables for setting the address for writing via CPU addresses using bus_space_write
	static bus_space_tag_t  tag;
	bus_space_handle_t      ioh, ioh1;
	uint32_t                addr;
	Int32                   old_speed;

#endif

};

#endif
