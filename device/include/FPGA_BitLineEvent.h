
#ifndef STI_FPGA_FPGA_BITLINEEVENT_H
#define STI_FPGA_FPGA_BITLINEEVENT_H

#include "FPGA_Device.h"
#include "BitLineEvent.h"
#include "FPGATypes.h"
#include "EtraxBus.h"

namespace STI
{
namespace FPGA
{

template<int N=32>
class FPGA_BitLineEvent : public STI::TimingEngine::BitLineEvent<N>
{
public:
	FPGA_BitLineEvent(const STI::TimingEngine::EventTime& time, FPGA_Device* device) : 
	  BitLineEvent<N>(time), device_f(device)
	{
		engineRAMBlock = device_f->parsingEngineRAMBlock;
	}
	FPGA_BitLineEvent(const FPGA_BitLineEvent &copy) : BitLineEvent<N>(copy) { }

	//Read the contents of the time register for this event from the FPGA
	uInt32 readBackTime()
	{
		return device_f->ramBus->readDataFromAddress( timeAddress );
	}
	//Read the contents of the value register for this event from the FPGA
	uInt32 readBackValue()
	{
		return device_f->ramBus->readDataFromAddress( valueAddress );
	}

protected:

	virtual void setupEvent()
	{
		time32 = static_cast<uInt32>( getTime() / 10 );	//in clock cycles! (1 cycle = 10 ns)
		timeAddress  = engineRAMBlock->getWrappedAddress( 2*getEventNumber() );
		valueAddress = engineRAMBlock->getWrappedAddress( 2*getEventNumber() + 1 );
	}
	virtual void loadEvent()
	{
		//write the event to RAM
		device_f->ramBus->writeDataToAddress( time32, timeAddress );
		device_f->ramBus->writeDataToAddress( getValue(), valueAddress );
	}
	virtual void playEvent()
	{
//		device_f->waitForEvent(getEventNumber());
	}
//	virtual void collectMeasurementData() = 0;
	virtual void collectMeasurements(STI::TimingEngine::TimingMeasurementVector& measurementsOut) = 0;
	virtual void publishMeasurements(const STI::TimingEngine::TimingMeasurementVector& measurements) {}

	virtual void reloadEvent()
	{
		loadEvent();
	}

private:
	uInt32 timeAddress;
	uInt32 valueAddress;
	uInt32 time32;

	FPGA_Device* device_f;

	FPGA_RAM_Block_ptr engineRAMBlock;

};

typedef FPGA_BitLineEvent<> FPGA_Event;	//shortcut for a 32 bit FPGA event

}
}

#endif
