
#include "LocalTimingEvent.h"
#include "DynamicValue.h"


using STI::TimingEngine::LocalTimingEvent;
using STI::TimingEngine::EventTime;
using STI::TimingEngine::TextPosition;
using STI::TimingEngine::DynamicValue_ptr;
using STI::Utils::MixedValue;
using STI::TimingEngine::ScheduledMeasurement_ptr;
using STI::TimingEngine::Channel;

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

const Channel& LocalTimingEvent::channel() const
{
	return channel_l;
}

const MixedValue& LocalTimingEvent::value() const
{	
	if(hasDynamicValue) {
		return dynamicValue_l->getValue();
	}
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

const std::string& LocalTimingEvent::description() const
{
	return description_l;
}

//bool LocalTimingEvent::getMeasurement(TimingMeasurement_ptr& measurement) const
//{
//	if(measurement_l != 0) {
//		measurement = measurement_l;
//		return true;
//	}
//	return false;
//}

bool LocalTimingEvent::getDynamicValue(DynamicValue_ptr& dynamicValue) const
{
	dynamicValue = dynamicValue_l;
	return hasDynamicValue && (dynamicValue != 0);
}

//void LocalTimingEvent::makeScheduledMeasurement(const STI::TimingEngine::Channel& channel, unsigned eventNumber)
//{	
//	measurement_l = TimingMeasurement_ptr( new LocalTimingMeasurement(channel, eventNumber) );
//}

