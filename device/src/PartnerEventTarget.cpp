
#include "PartnerEventTarget.h"

using STI::TimingEngine::PartnerEventTarget;
using STI::TimingEngine::TimingEvent_ptr;
using STI::TimingEngine::DeviceTimingEventsMap;

PartnerEventTarget::PartnerEventTarget(STI::TimingEngine::DeviceTimingEventsMap& partnerEventsMap)
: partnerEvents(partnerEventsMap)
{
}

void PartnerEventTarget::addEvent(const STI::Device::DeviceIDString& deviceID, TimingEvent_ptr& evt)
{
	partnerEvents[deviceID].push_back(evt);
}