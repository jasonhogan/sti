#ifndef STI_TIMINGENGINE_TIMINGMEASUREMENT_H
#define STI_TIMINGENGINE_TIMINGMEASUREMENT_H

#include "EventTime.h"
#include "Channel.h"

#include <MixedValue.h>

#include <string>

namespace STI
{
namespace TimingEngine
{

class TimingMeasurement
{
public:

	TimingMeasurement(const STI::TimingEngine::Channel& channel, unsigned eventNumber)
		: time_l(0), channel_l(channel),
		eventNumber_l(eventNumber), description_l("") {}
	TimingMeasurement(const TimingMeasurement& meas)
		: time_l(meas.time_l), channel_l(meas.channel_l), value_l(meas.value_l),
		eventNumber_l(meas.eventNumber_l), description_l(meas.description_l) {}
	virtual ~TimingMeasurement() {}

	const EventTime& time() const { return time_l; }
//	unsigned short channel() const { return channel_l; }
	const STI::TimingEngine::Channel& channel() const { return channel_l; }
//	const STI::Utils::MixedValue_ptr& command() const { return command_l; }
	const STI::Utils::MixedValue& measuredValue() const { return value_l; }
	
	unsigned eventNum() const { return eventNumber_l; }
	std::string getDescription() const { return description_l; }

protected:
	
	friend class SynchronousEvent;

	EventTime time_l;
	STI::TimingEngine::Channel channel_l;
	STI::Utils::MixedValue value_l;
//	STI::Utils::MixedValue_ptr command_l;
	unsigned eventNumber_l;
	std::string description_l;

};

}
}

#endif
