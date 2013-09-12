#ifndef STI_TIMINGENGINE_TIMINGMEASUREMENT_H
#define STI_TIMINGENGINE_TIMINGMEASUREMENT_H

#include "EventTime.h"
#include "TimingEngineTypes.h"
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

class TimingMeasurement
{
public:
	virtual ~TimingMeasurement() {}

	virtual const EventTime& time() const = 0;
	virtual const STI::TimingEngine::Channel& channel() const = 0;
	virtual const STI::Utils::MixedValue& measuredValue() const = 0;
	virtual unsigned eventNum() const = 0;
	virtual std::string getDescription() const = 0;

	virtual STI::Utils::MixedValue& value() = 0;

	template<typename T>
	void setValue(const T& value)
	{
		value().setValue(value);
	}
};

//class TimingMeasurement
//{
//public:
//
//	virtual ~TimingMeasurement() {}
//
//	virtual const EventTime& time() const = 0;
//	virtual const STI::TimingEngine::Channel& channel() const = 0;
//	virtual const STI::Utils::MixedValue& measuredValue() const = 0;
//	
//	virtual unsigned eventNum() const = 0;
//	virtual std::string getDescription() const = 0;
//
//private:
//
//	friend class SynchronousEvent;
//	friend class TimingMeasurementResult;
//	friend class ScheduledMeasurement;
//
//	virtual STI::Utils::MixedValue& getValue() = 0;
//	virtual void setTime(const EventTime& time) = 0;
//	virtual void setDescription(std::string desc) = 0;
//
//};

}
}

#endif
