#ifndef STI_TIMINGENGINE_BLOCKINGMEASUREMENTRESULTSTARGET_H
#define STI_TIMINGENGINE_BLOCKINGMEASUREMENTRESULTSTARGET_H

#include "TimingEngineTypes.h"
#include "MeasurementResultsTarget.h"
#include "EngineID.h"

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

namespace STI
{
namespace TimingEngine
{


class BlockingMeasurementResultsTarget : public MeasurementResultsTarget
{
public:
	
	BlockingMeasurementResultsTarget(const EngineInstance& engineInstance);

	bool getData(TimingMeasurementVector_ptr& data, unsigned timeout_ms, bool keepTrying);
	void stopWaiting();
	void reset();

	void appendData(const EngineInstance& engineInstance, const TimingMeasurementVector& newData, unsigned numberScheduledMeasurements);


private:

	void reset(boost::unique_lock<boost::mutex>& lock);

	TimingMeasurementVector_ptr data_l;

	bool stop;
	bool dataReady;

	EngineInstance engineInstance_l;

	mutable boost::mutex resultsMutex;
	mutable boost::condition_variable_any resultsCondition;

};


}
}

#endif

