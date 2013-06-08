#ifndef STI_TIMINGENGINE_PARSINGRESULTSTARGET_H
#define STI_TIMINGENGINE_PARSINGRESULTSTARGET_H

#include "TimingEngineTypes.h"
#include "DeviceID.h"
#include "EngineID.h"

#include <string>

namespace STI
{
namespace TimingEngine
{

class ParsingResultsTarget
{
public:
	virtual ~ParsingResultsTarget() {}

	virtual void handleParsingResults(
		const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		bool success, const std::string& errors, 
		const STI::TimingEngine::TimingEventVector_ptr& eventsOut) = 0;
//		const STI::TimingEngine::DeviceTimingEventsMap& eventsOut) = 0;
};


}
}

#endif

