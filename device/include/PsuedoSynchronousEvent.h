#ifndef STI_TIMINGENGINE_PSUEDOSYNCHRONOUSEVENT_H
#define STI_TIMINGENGINE_PSUEDOSYNCHRONOUSEVENT_H

#include "TimingEngineTypes.h"
#include "DeviceTypes.h"
#include "SynchronousEvent.h"
//#include "TimingEvent.h"

#include <vector>

namespace STI
{
namespace TimingEngine
{

class PsuedoSynchronousEvent : public SynchronousEvent
{
public:
	PsuedoSynchronousEvent(EventTime time, const RawEventVector& events, STI::Device::STI_Device* device);
	PsuedoSynchronousEvent(const PsuedoSynchronousEvent& copy);

private:
	void setupEvent() {}
	void loadEvent() {}
	void playEvent();
	void collectMeasurements(TimingMeasurementVector& measurementsOut);
	void publishMeasurements(const TimingMeasurementVector& measurements);
	void reloadEvent() {}

	bool getEventIndex(unsigned eventNum, unsigned& k);

protected:
	const RawEventVector* events_l;
	STI::Device::STI_Device* device_l;
};

}
}

#endif

