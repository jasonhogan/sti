
#include "LocalTimingEvent.h"

using STI::TimingEngine::LocalTimingEvent;
using STI::TimingEngine::EventTime;
using STI::TimingEngine::TextPosition;
using STI::Utils::MixedValue;
using STI::TimingEngine::ScheduledMeasurement_ptr;

bool LocalTimingEvent::operator==(const TimingEvent& rhs) const
{
	return this->time() == rhs.time();
}

bool LocalTimingEvent::operator!=(const TimingEvent& rhs) const
{
	return !((*this) == rhs);
}
bool LocalTimingEvent::operator<(const TimingEvent& rhs) const
{
	return this->time() < rhs.time();
}

bool LocalTimingEvent::operator>(const TimingEvent& rhs) const
{
	return ( !((*this) < rhs) && ((*this) != rhs) );
}

const EventTime& LocalTimingEvent::time() const
{
	return time_l;
}

unsigned short LocalTimingEvent::channelNum() const
{
	return channelNum_l;
}

const MixedValue& LocalTimingEvent::value() const
{
	return value_l;
}

unsigned LocalTimingEvent::eventNum() const
{
	return eventNumber_l;
}

bool LocalTimingEvent::isMeasurementEvent() const
{
	return isMeasurement_l;
}

const TextPosition& LocalTimingEvent::position() const
{
	return position_l;
}


bool LocalTimingEvent::getMeasurement(ScheduledMeasurement_ptr& measurement) const
{
	if(measurement_l != 0) {
		measurement = measurement_l;
		return true;
	}
	return false;
}

