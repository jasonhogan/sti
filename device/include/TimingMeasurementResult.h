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
	TimingMeasurementResult(const TimingMeasurement& meas)
		: TimingMeasurement(meas) {}

	STI::Utils::MixedValue& value() { return value_l; }
};


}
}

#endif
