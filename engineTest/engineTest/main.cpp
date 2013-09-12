
#include "testDevice.h"

#include "TextPosition.h"
#include "LocalTimingEvent.h"
#include "QueuedEventEngineManager.h"

#include "DocumentationOptions.h"
#include "PlayOptions.h"

#include "ParsingResultsHandler.h"
#include "ParsingResultsTarget.h"
#include "EngineCallbackHandler.h"
#include "EngineCallbackTarget.h"
#include "LocalMeasurementResultsHandler.h"
#include "MeasurementResultsTarget.h"

#include "NullEngineCallbackTarget.h"
#include "NullEngineCallbackHandler.h"

#include "DeviceID.h"
#include "Distributer.h"
#include "LocalCollector.h"
#include "DeviceInterface.h"

#include "STI_Server.h"

//#include <boost/thread.hpp>

#include <iostream>
using namespace std;


typedef boost::shared_ptr<testDevice> testDevice_ptr;

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

//class NullCallbackTarget : public STI::TimingEngine::ParsingResultsTarget, public STI::TimingEngine::EngineCallbackTarget
//{
//	void handleParsingResults(
//		const STI::Device::DeviceID& deviceID, 
//		const STI::TimingEngine::EngineInstance& engineInstance,
//		bool success, const std::string& errors, 
//		const STI::TimingEngine::TimingEventVector_ptr& eventsOut) {}
//
//	void handleCallback(const STI::Device::DeviceID& deviceID, 
//		const STI::TimingEngine::EngineInstance& engineInstance,
//		const STI::TimingEngine::EventEngineState& state) {}
//};


class TestParsingResultsTarget : public STI::TimingEngine::ParsingResultsTarget
{
public:
	void reset()
	{
		boost::unique_lock<boost::mutex> resourceLock(callbackMutex);
		callbackReceived = false;
	}

	void waitForCallback()
	{
		boost::unique_lock<boost::mutex> resourceLock(callbackMutex);

		boost::system_time wakeTime;

		while( !callbackReceived )
		{
			wakeTime = boost::get_system_time() 
				+ boost::posix_time::milliseconds( static_cast<long>(1000) );
			callbackCondition.timed_wait(callbackMutex, wakeTime);
		}

	}

	void handleParsingResults(
		const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		bool success, const std::string& errors, 
		const STI::TimingEngine::TimingEventVector_ptr& eventsOut)
	{
		boost::unique_lock<boost::mutex> resourceLock(callbackMutex);
		callbackReceived = true;

		if(!success) {
			cout << "Parsing Errors: " << endl << errors << endl;
		}
	}

private:
	bool callbackReceived;
	mutable boost::mutex callbackMutex;
	mutable boost::condition_variable_any callbackCondition;
};



class TestEngineCallbackTarget : public STI::TimingEngine::EngineCallbackTarget
{
public:

	void reset()
	{
		boost::unique_lock<boost::mutex> resourceLock(callbackMutex);
		callbackReceived = false;
	}

	void waitForCallback()
	{
		boost::unique_lock<boost::mutex> resourceLock(callbackMutex);

		boost::system_time wakeTime;

		while( !callbackReceived )
		{
			wakeTime = boost::get_system_time() 
				+ boost::posix_time::milliseconds( static_cast<long>(1000) );
			callbackCondition.timed_wait(callbackMutex, wakeTime);
		}

	}

	void handleCallback(const STI::Device::DeviceID& deviceID, 
		const STI::TimingEngine::EngineInstance& engineInstance,
		const STI::TimingEngine::EventEngineState& state)
	{
		boost::unique_lock<boost::mutex> resourceLock(callbackMutex);
		callbackReceived = true;
		//callbackReceived = true;
		callbackCondition.notify_one();
		cout << "handleCallback: " << deviceID.getID() << " : " <<  STI::Utils::print(state) << endl;
		//if(!success) {
		//	cout << "Parsing Errors: " << endl << errors << endl;
		//}
	}
private:
	bool callbackReceived;
	mutable boost::mutex callbackMutex;
	mutable boost::condition_variable_any callbackCondition;
};




typedef boost::shared_ptr<TestParsingResultsTarget> TestParsingResultsTarget_ptr;
typedef boost::shared_ptr<TestEngineCallbackTarget> TestEngineCallbackTarget_ptr;


void memtest();
void distributeTest();
void playtest();
void serverTest();
void singleLineTimingTest();


namespace STI { 
namespace Device {

class LocalDevice2 :  public STI::Device::DeviceInterface
{
public:
	LocalDevice2() : partnerCollector(new STI::Device::LocalDeviceCollector()), id("", "", 0)  {}	//
//	DeviceCollector_ptr getPartnerCollector() { return partnerCollector; }

	STI::Device::DeviceCollector_ptr partnerCollector;
	STI::Device::DeviceID id;

	const STI::Device::DeviceID& getDeviceID() const {return id;}
	bool getEventEngineManager(STI::TimingEngine::EventEngineManager_ptr& manager) {return false;}
	bool getPartnerCollector(STI::Device::DeviceCollector_ptr& deviceCollector) {deviceCollector = partnerCollector; return true;}
	bool getEventPartners(STI::Device::DeviceIDSet_ptr& eventPartners) { return false; }
//	bool addEventEngine(const STI::TimingEngine::EngineID& engineID) { return false; }
	bool read(unsigned short channel, 
		const STI::Utils::MixedValue& commandIn, STI::Utils::MixedValue& measurementOut)  { return false; }
	bool write(unsigned short channel, const STI::Utils::MixedValue& commandIn) { return false; }

	void connect(const STI::Server::ServerInterface& serverRef) {}

};
typedef boost::shared_ptr<LocalDevice2> LocalDevice2_ptr;

}
}


TestEngineCallbackTarget_ptr engineCallbackTarget(new TestEngineCallbackTarget());


int main(int argc, char **argv)
{
//	STI::Device::DeviceID serverID("Server1", "192.168.10.101", 0);

//	STI::Server::STI_Server server(serverID);

	int x;
	cout << "Test mode: ";
	cin >> x;


	if(x == 1) memtest();
	if(x == 2) playtest();
	if(x == 3) distributeTest();
	

	//while(true) {
	//	STI::Device::DeviceID serverID("Server", "localhost", 0);
	//	STI::Server::STI_Server server(serverID);
	//	testDevice_ptr test1(new testDevice("Test", "192.168.0.1", 0));
	//}

	if(x == 4) { while(true) {serverTest();} }
	if(x == 5) { while(true) {singleLineTimingTest();} }


//	while(true) {
////		serverTest();
//		singleLineTimingTest();
//	}
	return 0;
}


void distributeTest()
{
	using STI::Device::DeviceInterface;
	//using STI::Device::LocalDevice;
	//using STI::Device::LocalDevice_ptr;
	using STI::Device::DeviceInterface_ptr;
	using STI::Device::DeviceID;
	using STI::Device::LocalDevice2;
	using STI::Device::LocalDevice2_ptr;

	STI::Utils::Distributer<STI::Device::DeviceID, STI::Device::DeviceInterface> dist;
	
	//DeviceID id1("Test1", "192.168.10.101", 1);
	//LocalDevice_ptr test1(new LocalDevice());
	DeviceID id2("Test2", "192.168.10.101", 2);
	DeviceInterface_ptr test2(new LocalDevice2());


	//dist.addNode(id1, test1);
	//dist.addCollector(id1, test1);

	dist.addNode(id2, test2);
//	dist.addCollector(id2, test2->getPartnerCollector());

	//	dist.addNode(id1, 4);
}



void playtest()
{

	testDevice test1("Test", "192.168.0.1", 0);
//	test1.start();

	TestParsingResultsTarget_ptr parsingTarget(new TestParsingResultsTarget());

	STI::TimingEngine::EventEngineManager_ptr manager;

	std::set<STI::TimingEngine::EngineID> ids;
	test1.getEventEngineManager(manager);
	manager->getEngineIDs(ids);
	std::set<STI::TimingEngine::EngineID>::iterator id = ids.begin();
	
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
	
	STI::TimingEngine::EngineCallbackHandler_ptr engineCallback(
		new STI::TimingEngine::EngineCallbackHandler(test1.getDeviceID(), instance, engineCallbackTarget));
	
	STI::TimingEngine::MeasurementResultsTarget_ptr measurementResultsTarget(
		new STI::TimingEngine::NullMeasurementResultsTarget());
	STI::TimingEngine::MeasurementResultsHandler_ptr measurementResultsHandler(
		new STI::TimingEngine::LocalMeasurementResultsHandler(measurementResultsTarget, 0));
	
	//Make an event list and parse it
	{
		STI::TimingEngine::TimingEventVector_ptr events = 
			STI::TimingEngine::TimingEventVector_ptr(new STI::TimingEngine::TimingEventVector());
		STI::TimingEngine::TextPosition pos("main", 1);
		STI::TimingEngine::TimingEvent_ptr evt;

		STI::TimingEngine::Channel chan2(test1.getDeviceID(), 2, STI::TimingEngine::Output, "laser power", STI::Utils::Double, STI::Utils::Double);

		unsigned stop = 100;
		for(unsigned i = 0; i < stop; i++) {
			evt = STI::TimingEngine::TimingEvent_ptr( 
				new STI::TimingEngine::LocalTimingEvent(100 + i, chan2, "Hello " + STI::Utils::valueToString(i), 0, pos) );
			events->push_back( evt );
		}
		test1.getEventEngineManager(manager);
		manager->parse(instance, events, parsingHandler);
	}

	test1.getEventEngineManager(manager);
	manager->load(instance, engineCallback);
	
//	{
//		STI::TimingEngine::QueuedEventEngineManager queuedManager(test1.getEventEngineManager(), 2);
//	}
	
	STI::TimingEngine::DocumentationOptions_ptr docOptions( 
		new STI::TimingEngine::LocalDocumentationOptions(true, "") );
	test1.getEventEngineManager(manager);
	STI::TimingEngine::QueuedEventEngineManager queuedManager(manager, 2);

	STI::TimingEngine::PlayOptions_ptr playOptions( 
		new STI::TimingEngine::PlayOptions(0, 1000, 0) );


	queuedManager.play(instance, playOptions, docOptions, measurementResultsHandler, engineCallback);

	while( !queuedManager.inState(instance.id, STI::TimingEngine::WaitingForTrigger) )
	{
		cout << "Preparing..." << endl;
	}
	queuedManager.trigger(instance);
	

	int x;
	cin >> x;

	test1.getEventEngineManager(manager);
	manager->clear(instance.id, engineCallback);
}


void singleLineTimingTest()
{
	testDevice_ptr test1(new testDevice("Test", "192.168.0.1", 0));

	STI::Device::DeviceID serverID("Server", "localhost", 0);
	STI::Server::STI_Server server(serverID);
	server.addDevice(test1);

	while(true) {
//		test1->writeChannelDefault(2, "writeChannelDefault!");
		STI::Utils::MixedValue val;
		test1->readChannelDefault(1, STI::Utils::MixedValue(), val);
		cout << "Value = " << val.print() << endl;
	}

	while(true) {

	STI::TimingEngine::TextPosition pos("main", 1);
	STI::TimingEngine::Channel chan2(test1->getDeviceID(), 2, STI::TimingEngine::Output, "laser power", STI::Utils::String, STI::Utils::String);		
	STI::TimingEngine::TimingEvent_ptr evt = STI::TimingEngine::TimingEvent_ptr( 
				new STI::TimingEngine::LocalTimingEvent(100, chan2, "Single line timing file", 0, pos) );

	STI::TimingEngine::TimingMeasurement_ptr measurement;

	test1->playSingleEventDefault(evt, measurement);
	} //while
}

void serverTest()
{
	testDevice_ptr test1(new testDevice("Test", "192.168.0.1", 0));
	testDevice_ptr test2(new testDevice("dev2","192.168.0.1", 1));
//	test1->start();
//	test2->start();


	STI::Device::DeviceID serverID("Server", "localhost", 0);
	STI::Server::STI_Server server(serverID);

	server.addDevice(test1);
	server.addDevice(test2);

	//server.deviceDistributer.addNode(test1->getDeviceID(), test1);
	//server.deviceDistributer.addNode(test2->getDeviceID(), test2);
	//
	//STI::Device::DeviceCollector_ptr collector;
	//test1->getPartnerCollector(collector);

	//server.deviceDistributer.addCollector(test1->getDeviceID(), collector);
//	server.deviceDistributer.addCollector(test2->getDeviceID(), test2->getPartnerCollector());

//	STI::TimingEngine::EngineID eID(3, "TestEngine");

	server.initEngines();


	std::set<STI::TimingEngine::EngineID> ids;
	server.queuedEngineManager->getEngineIDs(ids);
	std::set<STI::TimingEngine::EngineID>::iterator id = ids.begin();
	
	STI::TimingEngine::EngineInstance instance(*id);
	instance.parseTimestamp.timestamp = 0;
	instance.playTimestamp.timestamp = 2;
	
	TestParsingResultsTarget_ptr parsingTarget(new TestParsingResultsTarget());
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler(
		new STI::TimingEngine::ParsingResultsHandler(serverID, instance, parsingTarget));
	
	STI::TimingEngine::EngineCallbackHandler_ptr engineCallback(
		new STI::TimingEngine::EngineCallbackHandler(serverID, instance, engineCallbackTarget));

	//STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler(
	//	new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance, parsingTarget));
	//STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler2(
	//	new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance2, parsingTarget));
	
//	int x;

	while(true) {

	//engineCallbackTarget->reset();
	server.localEngineManager->clear(instance.id, engineCallback);
	//engineCallbackTarget->waitForCallback();



	//Make an event list and parse it
	{
		STI::TimingEngine::TimingEventVector_ptr events = 
			STI::TimingEngine::TimingEventVector_ptr(new STI::TimingEngine::TimingEventVector());
		STI::TimingEngine::TextPosition pos("main", 1);
		STI::TimingEngine::TimingEvent_ptr evt;

		STI::TimingEngine::Channel chan2(test1->getDeviceID(), 2, STI::TimingEngine::Output, "laser power", STI::Utils::String, STI::Utils::String);

		unsigned stop = 27;
		for(unsigned i = 0; i < stop; i++) {
			evt = STI::TimingEngine::TimingEvent_ptr( 
				new STI::TimingEngine::LocalTimingEvent(100.0 + 0.5*1000000000*i, chan2, "Hello " + STI::Utils::valueToString(i), 0, pos) );
			events->push_back( evt );
		}


		STI::TimingEngine::Channel chan5b(test2->getDeviceID(), 5, STI::TimingEngine::Output, "Something else", STI::Utils::String, STI::Utils::String);

		stop = 10;
		for(unsigned i = 0; i < stop; i++) {
			evt = STI::TimingEngine::TimingEvent_ptr( 
				new STI::TimingEngine::LocalTimingEvent(200.0 + 1.0*1000000000*i, chan5b, "Bye " + STI::Utils::valueToString(i), 0, pos) );
			events->push_back( evt );
		}



//		test1.getEventEngineManager(manager);
//		manager->parse(instance, events, parsingHandler);
//		test1.getEventEngineManager().parse(instance2, events, parsingHandler2);
//		server.queuedEngineManager->parse(instance, events, parsingHandler);
		parsingTarget->reset();
		server.localEngineManager->parse(instance, events, parsingHandler);
		parsingTarget->waitForCallback();
	}


	while(!server.localEngineManager->inState(instance.id, STI::TimingEngine::Parsed)) {}

//	cin >> x;

//	server.queuedEngineManager->load(instance, engineCallback);
//	engineCallbackTarget->reset();
	server.localEngineManager->load(instance, engineCallback);
//	engineCallbackTarget->waitForCallback();

//	cin >> x;


	while(!server.localEngineManager->inState(instance.id, STI::TimingEngine::Loaded)) {}

		
	STI::TimingEngine::DocumentationOptions_ptr docOptions( 
		new STI::TimingEngine::LocalDocumentationOptions(true, "") );
	
	STI::TimingEngine::MeasurementResultsTarget_ptr measurementResultsTarget(
		new STI::TimingEngine::NullMeasurementResultsTarget());
	STI::TimingEngine::MeasurementResultsHandler_ptr measurementResultsHandler(
		new STI::TimingEngine::LocalMeasurementResultsHandler(measurementResultsTarget, 0));


	STI::TimingEngine::PlayOptions_ptr playOptions( 
		new STI::TimingEngine::PlayOptions(0, 42000000000.0, 0) );


//	server.queuedEngineManager->play(instance, playOptions, docOptions, engineCallback);

//	engineCallbackTarget->reset();
	server.localEngineManager->play(instance, playOptions, docOptions, measurementResultsHandler, engineCallback);
//	engineCallbackTarget->waitForCallback();

	while(server.localEngineManager->inState(instance.id, STI::TimingEngine::Playing)) {}


	//cin >> x;

	}//while

//	server.queuedEngineManager->trigger(instance);
//	cin >> x;

}


void memtest()
{
	testDevice test1("Test", "192.168.0.1", 0);
	test1.start();
//	test1.connect();

	test1.write(2, 123);

	STI::TimingEngine::EventEngineManager_ptr manager;
	test1.getEventEngineManager(manager);

	TestParsingResultsTarget_ptr parsingTarget(new TestParsingResultsTarget());

	std::set<STI::TimingEngine::EngineID> ids;
	manager->getEngineIDs(ids);
	std::set<STI::TimingEngine::EngineID>::iterator id = ids.begin();
	
	STI::TimingEngine::EngineInstance instance(*id);
	instance.parseTimestamp.timestamp = 0;
	id++;
	STI::TimingEngine::EngineInstance instance2(*id);
	instance.parseTimestamp.timestamp = 22;
	
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler(
		new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance, parsingTarget));
	STI::TimingEngine::ParsingResultsHandler_ptr parsingHandler2(
		new STI::TimingEngine::ParsingResultsHandler(test1.getDeviceID(), instance2, parsingTarget));
		
	STI::TimingEngine::EngineCallbackHandler_ptr engineCallback(
		new STI::TimingEngine::EngineCallbackHandler(test1.getDeviceID(), instance, engineCallbackTarget));

	//Make an event list and parse it
	int run = true;
	unsigned count = 0;

//	STI::TimingEngine::Channel chan2(test1.getDeviceID(), 2, STI::TimingEngine::Output, "laser power", STI::Utils::Double, STI::Utils::Double);
	STI::TimingEngine::Channel chan2(test1.getDeviceID(), 2, STI::TimingEngine::Output, "laser power", STI::Utils::Empty, STI::Utils::String);

	while(true)
	{
//		engineCallbackTarget->reset();
		manager->clear(instance.id, engineCallback);
//		engineCallbackTarget->waitForCallback();

//		manager->inState(instance.id, 
//		while(!server.localEngineManager->inState(instance.id, STI::TimingEngine::Loaded)) {}
	}

	while (run > 0)
	{
		count++;
		if(false) {
			STI::TimingEngine::TimingEventVector_ptr events = 
				STI::TimingEngine::TimingEventVector_ptr(new STI::TimingEngine::TimingEventVector());
			STI::TimingEngine::TextPosition pos("main", 1);
			STI::TimingEngine::TimingEvent_ptr evt;
			
			unsigned stop = 0;
			unsigned j = 0;
			for(unsigned i = 0; i < stop; i++) {
				evt = STI::TimingEngine::TimingEvent_ptr( 
					new STI::TimingEngine::LocalTimingEvent(100 + i *(j+1), chan2, "Hello " + i, 0, pos) );
				events->push_back( evt );

				//if(i==stop-1) {
				//	cin >> i;
				//	j++;
				//}
			}
			manager->parse(instance, events, parsingHandler);
		}

//		test1.getEventEngineManager().parse(instance2, events, parsingHandler2);
		manager->clear(instance.id, engineCallback);
		
	//	cin >> run;
	}

	manager->load(instance, engineCallback);
	manager->load(instance2, engineCallback);

	manager->clear(instance.id, engineCallback);
	manager->clear(instance2.id, engineCallback);

}

