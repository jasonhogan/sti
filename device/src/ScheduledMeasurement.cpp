

#include "ScheduledMeasurement.h"
#include "TimingEvent.h"

using namespace STI::TimingEngine;

ScheduledMeasurement::ScheduledMeasurement(const TimingEvent_ptr& measurementEvent) : 
measurementEvent_l(measurementEvent) 
{
	setScheduleStatus(false);
	setDescription(measurementEvent_l->description());
}

bool ScheduledMeasurement::getMeasurementEvent(TimingEvent_ptr& measurementEvent)
{
	measurementEvent = measurementEvent_l;

	return (measurementEvent != 0);
}

void ScheduledMeasurement::setScheduleStatus(bool enabled)
{
	scheduled = enabled;
}

bool ScheduledMeasurement::isScheduled() const
{
	return scheduled;
}


const EventTime& ScheduledMeasurement::scheduledTime() const
{
	return time_l;
}

const std::string& ScheduledMeasurement::scheduledDescription() const
{
	return description_l;
}


void ScheduledMeasurement::setTime(const EventTime& time)
{
	time_l = time;
}

void ScheduledMeasurement::setDescription(const std::string& description)
{
	description_l = description;
}
