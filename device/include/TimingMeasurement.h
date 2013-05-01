#ifndef STI_TIMINGENGINE_TIMINGMEASUREMENT_H
#define STI_TIMINGENGINE_TIMINGMEASUREMENT_H

#include "EventTime.h"

#include <MixedValue.h>

#include <string>

namespace STI
{
namespace TimingEngine
{

class TimingMeasurement
{
public:

	TimingMeasurement(unsigned short channel, unsigned eventNumber)
		: time_l(0), channel_l(channel),
		eventNumber_l(eventNumber), description_l("") {}
	TimingMeasurement(const TimingMeasurement& meas)
		: time_l(meas.time_l), channel_l(meas.channel_l), value_l(meas.value_l),
		eventNumber_l(meas.eventNumber_l), description_l(meas.description_l) {}

	const EventTime& time() const { return time_l; }
	unsigned short channel() const { return channel_l; }
	const STI::Utils::MixedValue& value() const { return value_l; }
	
	unsigned eventNum() const { return eventNumber_l; }
	std::string getDescription() const { return description_l; }

protected:
	
	EventTime time_l;
	unsigned short channel_l;
	STI::Utils::MixedValue value_l;
	unsigned eventNumber_l;
	std::string description_l;

};

class ScheduledMeasurement : public TimingMeasurement
{
public:
	ScheduledMeasurement(unsigned short channel, unsigned eventNumber)
		: TimingMeasurement(channel, eventNumber), scheduled(false) {}
	
	void setScheduleStatus(bool enabled) { scheduled = enabled; }
	bool isScheduled() const { return scheduled; }

	void setTime(const EventTime& time) { time_l = time; }
	void setDescription(std::string desc) { description_l = desc; }

private:
	bool scheduled;
};

class TimingMeasurementResult : public TimingMeasurement
{
public:
	TimingMeasurementResult(const TimingMeasurement& meas)
		: TimingMeasurement(meas) {}

	STI::Utils::MixedValue& value() { return value_l; }
};


}
}

#endif
