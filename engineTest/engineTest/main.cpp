
//#include <LocalEventEngineManager.h>

#include "testDevice.h"

#include "ParsingResultsTarget.h"
#include "TextPosition.h"
#include "LocalTimingEvent.h"
#include "ParsingResultsHandler.h"
#include "QueuedEventEngineManager.h"
#include "DocumentationOptions.h"

#include <iostream>
using namespace std;

//python
//
//event(time, channel, value)
//realtime(startTime, &loadMotFunction)
//
//
//def function
//    event(...)
//
//def realtime(startTime, function)
//   if(!playing) addRealtimeEvent(startTime) return
//   enableRealTime = true
//   function()
//   enableRealTime = false
//
//can implement the rest of the functionality in python with wrapper functions

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
			cout << "Parsing Errors: " << endl << errors << endl;
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
void memtest();
void playtest();

#include <boost/thread.hpp>

void test()
{
	int x;
	cout << "Waiting" << endl;
	cin >> x;
}

int main(int argc, char **argv)
{
	//boost::thread thr1( &test );
	//thr1.join();
	//cout << "joined" << endl;


//	memtest();
	playtest();
	return 0;
}

void playtest()
{
//	STI::TimingEngine::LocalEventEngineManager manager;

	testDevice test1("Test", "192.168.0.1", 0);
	test1.start();

	TestParsingResultsTarget parsingTarget;

	std::set<const STI::TimingEngine::EngineID> ids;
	test1.getEventEngineManager().getEngineIDs(ids);
	std::set<const STI::TimingEngine::EngineID>::iterator id = ids.begin();
	
	STI::TimingEngine::EngineInstance instance(*id);
	instance.parseTimestamp.timestamp = 0;
	instance.playTimestamp.timestamp = 2;
	id++;
	STI::TimingEngine::EngineInstance instance2(*id);
	instance2.parseTimestamp.timestamp = 22;
	instance2.playTimestamp.timestamp = 40;
	
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler(
		new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance, parsingTarget));
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler2(
		new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance2, parsingTarget));
	
	//Make an event list and parse it
	{
		STI::TimingEngine::TimingEventVector_ptr events = 
			STI::TimingEngine::TimingEventVector_ptr(new STI::TimingEngine::TimingEventVector());
		STI::TimingEngine::TextPosition pos("main", 1);
		STI::TimingEngine::TimingEvent_ptr evt;

		unsigned stop = 100;
		for(unsigned i = 0; i < stop; i++) {
			evt = STI::TimingEngine::TimingEvent_ptr( 
				new STI::TimingEngine::LocalTimingEvent(100 + i, 2, "Hello " + STI::Utils::valueToString(i), 0, pos) );
			events->push_back( evt );
		}
		test1.getEventEngineManager().parse(instance, events, parsingHandler);
//		test1.getEventEngineManager().parse(instance2, events, parsingHandler2);
	}

	test1.getEventEngineManager().load(instance);
//	test1.getEventEngineManager().load(instance2);
	
//	{
//		STI::TimingEngine::QueuedEventEngineManager queuedManager(test1.getEventEngineManager(), 2);
//	}
	
	STI::TimingEngine::DocumentationOptions_ptr docOptions( 
		new STI::TimingEngine::LocalDocumentationOptions(true) );
	STI::TimingEngine::QueuedEventEngineManager queuedManager(test1.getEventEngineManager(), 2);

	queuedManager.play(instance,0, 1000, 0, docOptions);

	while( !queuedManager.inState(instance.id, STI::TimingEngine::WaitingForTrigger) )
	{
		cout << "Preparing..." << endl;
	}
	queuedManager.trigger(instance);
	

//	while(true) {};
	int x;
	cin >> x;

	test1.getEventEngineManager().clear(instance.id);
//	test1.getEventEngineManager().clear(instance2.id);

}

void memtest()
{
//	STI::TimingEngine::LocalEventEngineManager manager;

	testDevice test1("Test", "192.168.0.1", 0);
	test1.start();

	test1.write(2, 123);


	TestParsingResultsTarget parsingTarget;

	std::set<const STI::TimingEngine::EngineID> ids;
	test1.getEventEngineManager().getEngineIDs(ids);
	std::set<const STI::TimingEngine::EngineID>::iterator id = ids.begin();
	
	STI::TimingEngine::EngineInstance instance(*id);
	instance.parseTimestamp.timestamp = 0;
	id++;
	STI::TimingEngine::EngineInstance instance2(*id);
	instance.parseTimestamp.timestamp = 22;
	
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler(
		new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance, parsingTarget));
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler2(
		new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance2, parsingTarget));
	
	//Make an event list and parse it
	int run = true;
	unsigned count = 0;
	while (run > 0)
	{
		count++;
		{
			STI::TimingEngine::TimingEventVector_ptr events = 
				STI::TimingEngine::TimingEventVector_ptr(new STI::TimingEngine::TimingEventVector());
			STI::TimingEngine::TextPosition pos("main", 1);
			STI::TimingEngine::TimingEvent_ptr evt;
			
			unsigned stop = 10000;
			unsigned j = 0;
			for(unsigned i = 0; i < stop; i++) {
				evt = STI::TimingEngine::TimingEvent_ptr( 
					new STI::TimingEngine::LocalTimingEvent(100 + i *(j+1), 2, "Hello " + i, 0, pos) );
				events->push_back( evt );

				//if(i==stop-1) {
				//	cin >> i;
				//	j++;
				//}
			}
			test1.getEventEngineManager().parse(instance, events, parsingHandler);
		}

//		test1.getEventEngineManager().parse(instance2, events, parsingHandler2);
		test1.getEventEngineManager().clear(instance.id);
		
	//	cin >> run;
	}

	test1.getEventEngineManager().load(instance);
	test1.getEventEngineManager().load(instance2);

	test1.getEventEngineManager().clear(instance.id);
	test1.getEventEngineManager().clear(instance2.id);

}

