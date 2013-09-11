
#ifndef STI_FPGA_FPGA_BITLINEEVENT_H
#define STI_FPGA_FPGA_BITLINEEVENT_H

#include "FPGATypes.h"
#include "BitLineEvent.h"
#include "FPGA_BitLineEvent_DeviceWrapper.h"
#include "EtraxBus.h"

namespace STI
{
namespace FPGA
{


template<int N=32>
class FPGA_BitLineEvent : public STI::TimingEngine::BitLineEvent<N>
{
public:

	using STI::TimingEngine::SynchronousEvent::getTime;
	using STI::TimingEngine::SynchronousEvent::getEventNumber;
	using STI::TimingEngine::BitLineEvent<N>::getValue;

	FPGA_BitLineEvent(const STI::TimingEngine::EventTime& time, FPGA_Device* device) : 
	  STI::TimingEngine::BitLineEvent<N>(time), fpgaDeviceWrapper(device)
	{
		engineRAMBlock = fpgaDeviceWrapper.getParsingEngineRAMBlock();
//		engineRAMBlock = device_f->parsingEngineRAMBlock;
	}
	FPGA_BitLineEvent(const FPGA_BitLineEvent &copy) : STI::TimingEngine::BitLineEvent<N>(copy) { }

	~FPGA_BitLineEvent() {}

	//Read the contents of the time register for this event from the FPGA
	uInt32 readBackTime()
	{
		return fpgaDeviceWrapper.getRAMBus()->readDataFromAddress( timeAddress );
//		return device_f->ramBus->readDataFromAddress( timeAddress );
	}
	//Read the contents of the value register for this event from the FPGA
	uInt32 readBackValue()
	{
		return fpgaDeviceWrapper.getRAMBus()->readDataFromAddress( valueAddress );
//		return device_f->ramBus->readDataFromAddress( valueAddress );
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
		fpgaDeviceWrapper.getRAMBus()->writeDataToAddress( time32, timeAddress );
		fpgaDeviceWrapper.getRAMBus()->writeDataToAddress( getValue(), valueAddress );
//		device_f->ramBus->writeDataToAddress( time32, timeAddress );
//		device_f->ramBus->writeDataToAddress( getValue(), valueAddress );
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

//	FPGA_Device* device_f;

	FPGA_BitLineEvent_DeviceWrapper fpgaDeviceWrapper;

	FPGA_RAM_Block_ptr engineRAMBlock;

};

typedef FPGA_BitLineEvent<> FPGA_Event;	//shortcut for a 32 bit FPGA event

}
}

#endif
