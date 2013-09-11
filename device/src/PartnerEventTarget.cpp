
#include "PartnerEventTarget.h"

using STI::TimingEngine::PartnerEventTarget;
using STI::TimingEngine::TimingEvent_ptr;

PartnerEventTarget::PartnerEventTarget(STI::TimingEngine::TimingEventVector_ptr& partnerEvents)
: partnerEvents_l(partnerEvents)
{
}

void PartnerEventTarget::addEvent(TimingEvent_ptr& evt)
{
	partnerEvents_l->push_back(evt);
}

