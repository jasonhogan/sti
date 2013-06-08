
#include "PartnerEventTarget.h"

using STI::TimingEngine::PartnerEventTarget;
using STI::TimingEngine::TimingEvent_ptr;
using STI::TimingEngine::DeviceTimingEventsMap;

PartnerEventTarget::PartnerEventTarget(STI::TimingEngine::TimingEventVector_ptr& partnerEvents)
: partnerEvents_l(partnerEvents)
{
}

void PartnerEventTarget::addEvent(const STI::Device::DeviceIDString& deviceID, TimingEvent_ptr& evt)
{
	partnerEvents_l->push_back(evt);
}