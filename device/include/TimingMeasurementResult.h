#ifndef STI_TIMINGENGINE_TIMINGMEASUREMENTRESULT_H
#define STI_TIMINGENGINE_TIMINGMEASUREMENTRESULT_H

#include "TimingMeasurement.h"

#include <MixedValue.h>

namespace STI
{
namespace TimingEngine
{





class TimingMeasurementResult : public TimingMeasurement
{
public:
	
	TimingMeasurementResult(const TimingMeasurement_ptr& meas) : 
	  measurement(meas) {}

	virtual const EventTime& time() const = 0;
	virtual const STI::TimingEngine::Channel& channel() const = 0;
	virtual const STI::Utils::MixedValue& measuredValue() const = 0;
	virtual unsigned eventNum() const = 0;
	virtual std::string getDescription() const = 0;

//	STI::Utils::MixedValue& value() { return measurement->getValue(); }
	void setValue(const T& value) { measurement->setValue(value); }

private:

	TimingMeasurement_ptr measurement;
};


}
}

#endif
