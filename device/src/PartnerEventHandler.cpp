
#include "PartnerEventHandler.h"
#include "PartnerEventTarget.h"

using STI::TimingEngine::PartnerEventHandler;
using STI::TimingEngine::TimingEvent_ptr;

PartnerEventHandler::PartnerEventHandler()
{
}

void PartnerEventHandler::addEvent(TimingEvent_ptr& evt)
{
	if(partnerEventTarget != 0) {
		partnerEventTarget->addEvent(evt);
	}
}

void PartnerEventHandler::setEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& eventTarget)
{
	partnerEventTarget = eventTarget;
}

