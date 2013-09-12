

#include "BlockingMeasurementResultsTarget.h"
#include "TimingMeasurement.h"

#include <boost/thread/locks.hpp>

using STI::TimingEngine::BlockingMeasurementResultsTarget;
using STI::TimingEngine::TimingMeasurementVector;
using STI::TimingEngine::TimingMeasurementVector_ptr;
using STI::TimingEngine::EngineInstance;


BlockingMeasurementResultsTarget::BlockingMeasurementResultsTarget(const EngineInstance& engineInstance) :
engineInstance_l(engineInstance)
{
	stop = false;
	dataReady = false;

	data_l = TimingMeasurementVector_ptr(new TimingMeasurementVector());
}

bool BlockingMeasurementResultsTarget::getData(TimingMeasurementVector_ptr& data, unsigned timeout_ms, bool keepTrying)
{
	boost::unique_lock< boost::mutex > lock(resultsMutex);
	
	boost::system_time wakeTime;

	stop = false;

	do {
		wakeTime = boost::get_system_time()
		+ boost::posix_time::milliseconds( static_cast<long>(timeout_ms) );

		while(!dataReady && !stop && wakeTime > boost::get_system_time())
		{
			//This while loops avoid spurious wakeups (glitches); doesn't leave until the timeout is reached.
			resultsCondition.timed_wait(lock, wakeTime);
		}

	} while(!dataReady && keepTrying && !stop);
	
	data = data_l;

	return dataReady;

}

void BlockingMeasurementResultsTarget::stopWaiting()
{
	boost::unique_lock< boost::mutex > lock(resultsMutex);
	stop = true;
	resultsCondition.notify_all();
}

void BlockingMeasurementResultsTarget::appendData(const EngineInstance& engineInstance, const TimingMeasurementVector& newData, unsigned numberScheduledMeasurements)
{
	boost::unique_lock<boost::mutex> lock(resultsMutex);
	
	if(engineInstance_l != engineInstance) {
		//data_l contains old data. Reset.
		reset(lock);
		
		//Set the local EngineInstance to be the EngineInstance that is returning data.
		if(engineInstance_l.id == engineInstance.id) {
			engineInstance_l.parseTimestamp = engineInstance.parseTimestamp;
			engineInstance_l.playTimestamp = engineInstance.playTimestamp;
		}
	}

	if(data_l->size() < numberScheduledMeasurements) {
		data_l->insert(data_l->end(), newData.begin(), newData.end());
	}

	if(data_l->size() >= numberScheduledMeasurements) {
		//Data is ready.  Stop getData() from blocking.
		dataReady = true;

		resultsCondition.notify_all();
	}
}

void BlockingMeasurementResultsTarget::reset()
{
	boost::unique_lock<boost::mutex> lock(resultsMutex);

	reset(lock);
}

void BlockingMeasurementResultsTarget::reset(boost::unique_lock<boost::mutex>& lock)
{
	dataReady = false;
	data_l->clear();
}
