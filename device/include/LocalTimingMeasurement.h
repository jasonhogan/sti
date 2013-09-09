#ifndef STI_TIMINGENGINE_LOCALTIMINGMEASUREMENT_H
#define STI_TIMINGENGINE_LOCALTIMINGMEASUREMENT_H

#include "TimingMeasurement.h"
#include "EventTime.h"
#include "Channel.h"
#include <MixedValue.h>

#include <string>

namespace STI
{
namespace TimingEngine
{

//class ScheduledMeasurement
//{
//	TimingEvent_ptr sourceEvent;
//};
//
//class TimingMeasurement
//{
//	const STI::Utils::MixedValue& measuredValue() const { return value_l; }
//
//	friend class TimingMeasurementResult;
//
//private:
//	STI::Utils::MixedValue value_l;		??
//};
//
//class TimingMeasurementResult
//{
//	void setValue(const STI::Utils::MixedValue& result);
//
//	TimingMeasurement_ptr sourceEvent;
//
//};
//
//SynchronousEvent
//...
//	virtual void collectMeasurements(TimingMeasurementResultVector& measurementsOut) = 0;
//...


class LocalTimingMeasurement : public TimingMeasurement
{
public:

	LocalTimingMeasurement(const EventTime& time, const STI::TimingEngine::Channel& channel, 
		unsigned eventNumber, const std::string& description)
		: time_l(time), channel_l(channel),
		eventNumber_l(eventNumber), description_l(description) {}
//	LocalTimingMeasurement(const TimingMeasurement& meas)
//		: time_l(meas.time_l), channel_l(meas.channel_l), value_l(meas.value_l),
//		eventNumber_l(meas.eventNumber_l), description_l(meas.description_l) {}
	virtual ~LocalTimingMeasurement() {}

	const EventTime& time() const { return time_l; }
//	unsigned short channel() const { return channel_l; }
	const STI::TimingEngine::Channel& channel() const { return channel_l; }
//	const STI::Utils::MixedValue_ptr& command() const { return command_l; }
	const STI::Utils::MixedValue& measuredValue() const { return value_l; }
	
	unsigned eventNum() const { return eventNumber_l; }
	std::string getDescription() const { return description_l; }

	STI::Utils::MixedValue& value() { return value_l; }
//	void setTime(const EventTime& time) { time_l = time; }
//	void setDescription(std::string desc) { description_l = desc; }

private:

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
