

#ifndef GENERICSERIALCONTROLLER_H
#define GENERICSERIALCONTROLLER_H

#include <string>

class GenericSerialController
{
public: 
	virtual std::string queryDevice(std::string commandString, int sleepTimeMS /*= 100*/, int readLength /*= 30*/) = 0;
	virtual std::string queryDeviceSingleChar(std::string commandString,
                                                   int sleepTimeMS,
                                                   int charDelayMS,
                                                   std::string terminator,
                                                   bool echo) = 0;
	virtual void commandDevice(std::string commandString) = 0;
	
};

#endif
