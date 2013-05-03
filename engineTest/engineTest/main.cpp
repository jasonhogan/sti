
//#include <LocalEventEngineManager.h>

#include "testDevice.h"

#include "ParsingResultsTarget.h"
#include "TextPosition.h"
#include "LocalTimingEvent.h"
#include "ParsingResultsHandler.h"

#include <iostream>
using namespace std;


class TestParsingResultsTarget : public STI::TimingEngine::ParsingResultsTarget
{
public:

	void handleParsingResults(
		const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		bool success, const std::string& errors, 
		const STI::TimingEngine::DeviceTimingEventsMap& eventsOut)
	{
		if(!success) {
			cout << "Parsing Errors: " << errors << endl;
		}
	}
};

//typedef boost::shared_ptr<TestParsingResultsTarget> PartnerEventTarget_ptr

//class STI_Device : public Device
//{
//	readChannel() = 0;
//	writeChannel() = 0;
//	parseDeviceEvente() = 0;
//}
//
//class Device : public DeviceTimingEngineInterface, ...
//{
//	read();
//	write();
////	parseEvents();	(rename in DeviceTimingEngineInterface)
//}

int main(int argc, char **argv)
{
//	STI::TimingEngine::LocalEventEngineManager manager;

	testDevice test1("Test", "192.168.0.1", 0);
	test1.start();

	test1.write(2, 123);


	TestParsingResultsTarget parsingTarget;

	std::set<const STI::TimingEngine::EngineID> ids;
	test1.getEventEngineManager().getEngineIDs(ids);
	
	STI::TimingEngine::EngineInstance instance(*ids.begin());
	instance.parseTimestamp.timestamp = 0;
	
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler(
		new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance, parsingTarget));

	STI::TimingEngine::TimingEventVector events;
	STI::TimingEngine::TextPosition pos("main", 1);
	STI::TimingEngine::TimingEvent_ptr evt = STI::TimingEngine::TimingEvent_ptr( 
		new STI::TimingEngine::LocalTimingEvent(100, 2, "Hello", 0, pos) );

	events.push_back( evt );

	test1.getEventEngineManager().parse(instance, events, parsingHandler);

	test1.getEventEngineManager().load(instance);

	return 0;
}

