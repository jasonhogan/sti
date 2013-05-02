#ifndef STI_TIMINGENGINE_TIMINGEVENTGROUP_H
#define STI_TIMINGENGINE_TIMINGEVENTGROUP_H

#include <TimingEngineTypes.h>

namespace STI
{
namespace TimingEngine
{

class TimingEventGroup
{
public:
	double time() const;
	
	unsigned numberOfEvents() const { return timingEvents.size(); }

	const TimingEvent_ptr& at(unsigned int i) const { return timingEvents.at(i); }
//	const TimingEvent_ptr& operator[](unsigned int i) const;
	const TimingEvent_ptr& back() const { return timingEvents.back(); }
	
	void add(const TimingEvent_ptr& evt) { timingEvents.push_back(evt); }

private:
	TimingEventVector timingEvents;

};

}
}

#endif
