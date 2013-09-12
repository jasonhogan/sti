#ifndef STI_TIMINGENGINE_PARTNEREVENTHANDLER_H
#define STI_TIMINGENGINE_PARTNEREVENTHANDLER_H

#include "TimingEngineTypes.h"
//#include "TimingEvent.h"

namespace STI
{
namespace TimingEngine
{

class PartnerEventHandler
{
public:
	PartnerEventHandler();

	void addEvent(TimingEvent_ptr& evt);
	void setEventTarget(STI::TimingEngine::PartnerEventTarget_ptr& eventTarget);

private:
	STI::TimingEngine::PartnerEventTarget_ptr partnerEventTarget;

};

}
}

#endif

