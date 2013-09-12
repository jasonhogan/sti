#ifndef STI_TIMINGENGINE_SCHEDULEDMEASUREMENT_H
#define STI_TIMINGENGINE_SCHEDULEDMEASUREMENT_H

#include "TimingEngineTypes.h"

namespace STI
{
namespace TimingEngine
{

class ScheduledMeasurement
{
public:
	
	ScheduledMeasurement(const TimingEvent_ptr& measurementEvent);

//	ScheduledMeasurement(const STI::TimingEngine::Channel& channel, unsigned eventNumber)
//		: TimingMeasurement(channel, eventNumber), scheduled(false) {}
	
	void setScheduleStatus(bool enabled);
	bool isScheduled() const;
	
	const EventTime& scheduledTime() const;
	const std::string& scheduledDescription() const;

	void setDescription(const std::string& description);
	void setTime(const EventTime& time);

	bool getMeasurementEvent(TimingEvent_ptr& measurementEvent);

private:

	bool scheduled;

	EventTime time_l;
	std::string description_l;

	TimingEvent_ptr measurementEvent_l;
};

}
}

#endif
