#ifndef STI_TIMINGENGINE_PARSINGRESULTSHANDLER_H
#define STI_TIMINGENGINE_PARSINGRESULTSHANDLER_H

#include "TimingEngineTypes.h"
#include "DeviceID.h"
#include "EngineID.h"

#include <string>

namespace STI
{
namespace TimingEngine
{

//LocalParsingResultsHandler
class ParsingResultsHandler
{
public:
	ParsingResultsHandler(
		const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		ParsingResultsTarget& resultsTarget);
	
	bool parseSucceeded();

	void returnResults(bool success, const std::string& errors, 
		const STI::TimingEngine::DeviceTimingEventsMap& eventsOut);

private:
	STI::Device::DeviceID deviceID_l;
	STI::TimingEngine::EngineInstance engineInstance_l;
	
	ParsingResultsTarget& target;
	bool success_l;
};

//class ParsingResultsHandler
//{
//public:
//
////	ParsingResultsHandler(const STI::Device::DeviceID& device_id);
//
//	STI::Device::DeviceID deviceID;
//	STI::TimingEngine::EngineID engineID;
//	
//	virtual bool parseSucceeded() = 0;
//
////	void returnResults(const STI::Types::TDevice& deviceID, const EngineID& id, bool success, const std::string& errors, const std::vector<TimingEvent>& eventsOut);
//	//void returnResults(const STI::Device::DeviceID& deviceID, const STI::TimingEngine::EngineID& id, 
//	//	bool success, const std::string& errors, const STI::TimingEngine::TimingEventVector& eventsOut);
//	
//	virtual void returnResults(bool success, const std::string& errors, 
//		const STI::TimingEngine::DeviceTimingEventsMap& eventsOut) = 0;
//};



}
}

#endif

