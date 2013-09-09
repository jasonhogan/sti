
#ifndef STI_TIMINGENGINE_DYNAMICSYNCHRONOUSEVENT_H
#define STI_TIMINGENGINE_DYNAMICSYNCHRONOUSEVENT_H

#include "SynchronousEvent.h"
#include "DynamicValueListener.h"
#include "DeviceTypes.h"
#include "TimingEngineTypes.h"

namespace STI
{
namespace TimingEngine
{

class DynamicSynchronousEvent : public SynchronousEvent, public DynamicValueListener
{
public:
	DynamicSynchronousEvent(double time) :  SynchronousEvent(time) {} //provide as well, so no linked value is an option
		
//	DynamicSynchronousEvent(double time, STI::Device::STI_Device* device)    //provide as well, so no linked value is an option?
//		: SynchronousEvent(time, device) {}
	DynamicSynchronousEvent(double time, const TimingEventVector_ptr& sourceEvents);
	~DynamicSynchronousEvent();

	virtual void refresh(const DynamicValueEvent& evt);

protected:

	void addSourceEvents(const TimingEventVector_ptr& sourceEvents);

	TimingEventVector_ptr sourceEvents_l;

private:
	
	virtual void updateValue(const TimingEventVector_ptr& sourceEvents) = 0;

	DynamicValueVector dynamicValues;
};

}
}

#endif
