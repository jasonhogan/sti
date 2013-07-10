
#ifndef STI_TIMINGENGINE_MEASUREMENTCALLBACK_H
#define STI_TIMINGENGINE_MEASUREMENTCALLBACK_H

#include <boost/shared_ptr.hpp>

class MeasurementCallback;
typedef boost::shared_ptr<MeasurementCallback> MeasurementCallback_ptr;


class MeasurementCallback
{
public:
	virtual void handleResult(const STI::Types::TMeasurement& measurement) = 0;
};


#endif

