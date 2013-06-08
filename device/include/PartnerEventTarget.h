#ifndef STI_TIMINGENGINE_PARTNEREVENTTARGET_H
#define STI_TIMINGENGINE_PARTNEREVENTTARGET_H

#include "TimingEngineTypes.h"
#include "DeviceTypes.h"
#include "TimingEvent.h"

namespace STI
{
namespace TimingEngine
{

class PartnerEventTarget
{
public:
	PartnerEventTarget(STI::TimingEngine::TimingEventVector_ptr& partnerEvents);
	void addEvent(const STI::Device::DeviceIDString& deviceID, TimingEvent_ptr& evt);

private:
	STI::TimingEngine::TimingEventVector_ptr& partnerEvents_l;
};

}
}

#endif

