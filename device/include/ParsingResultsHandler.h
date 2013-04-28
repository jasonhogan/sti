#ifndef STI_TIMINGENGINE_PARSINGRESULTSHANDLER_H
#define STI_TIMINGENGINE_PARSINGRESULTSHANDLER_H

#include "TimingEngineTypes.h"
#include "DeviceID.h"

namespace STI
{
namespace TimingEngine
{

class ParsingResultsHandler
{
public:
//	void returnResults(const STI::Types::TDevice& deviceID, const EngineID& id, bool success, const std::string& errors, const std::vector<TimingEvent>& eventsOut);
	void returnResults(const STI::Device::DeviceID& deviceID, const STI::TimingEngine::EngineID& id, 
		bool success, const std::string& errors, const STI::TimingEngine::TimingEventVector& eventsOut);
};



}
}

#endif

