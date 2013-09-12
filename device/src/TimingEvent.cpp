
#include "TimingEvent.h"

using STI::TimingEngine::TimingEvent;
using STI::TimingEngine::ScheduledMeasurement_ptr;

bool TimingEvent::getMeasurement(ScheduledMeasurement_ptr& measurement) const
{
	//Returns true if there is a measurement reference and the reference has been copied.
	return ((!measurement_l.expired()) && (measurement = measurement_l.lock()));
}
void TimingEvent::setMeasurement(ScheduledMeasurement_ptr& measurement)
{
	measurement_l = measurement;
}
