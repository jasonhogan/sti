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
	
	unsigned numberOfEvents() const;

	const TimingEvent_ptr& at(unsigned int i) const;
//	const TimingEvent_ptr& operator[](unsigned int i) const;
	const TimingEvent_ptr& back() const;
	
	void add(const TimingEvent_ptr& evt);

private:
	TimingEventVector timingEvents;

};

}
}

#endif
