#ifndef STI_TIMINGENGINE_SCHEDULEDMEASUREMENT_H
#define STI_TIMINGENGINE_SCHEDULEDMEASUREMENT_H

#include "EventTime.h"
#include "TimingMeasurement.h"

namespace STI
{
namespace TimingEngine
{

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

}
}

#endif
