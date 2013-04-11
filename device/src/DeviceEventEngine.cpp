
#include "DeviceEventEngine.h"
#include "SynchronousEvent.h"

#include "TimingEvent.h"
#include "TimingEventGroup.h"
#include "Channel.h"

#include "EngineID.h"

#include "DataMeasurement.h"
#include "MixedValue.h"
#include "utils.h"

#include "EventParsingException.h"
#include "EventConflictException.h"

#include "ParsingResultsHandler.h"

#include <algorithm>
#include <iostream>

using std::endl;

using STI::TimingEngine::DeviceEventEngine;
using STI::TimingEngine::ParsingResultsHandler;
using STI::TimingEngine::TimingEventVector;
using STI::TimingEngine::TimingEvent;
using STI::TimingEngine::EngineTimestamp;
//using STI::TimingEngine::ConstTimingEventVector;




using STI::Utils::MixedValue;
using STI::Utils::MixedValueVector;

using STI::Utils::Boolean;
using STI::Utils::Octet;
using STI::Utils::Int;
using STI::Utils::Double;
using STI::Utils::String;
using STI::Utils::File;
using STI::Utils::Vector;
using STI::Utils::Empty;


//namespace STI { namespace TimingEngine {
//class ParsingResultsHandler
//{
//};
//}}


void DeviceEventEngine::load() 
{
	for(unsigned i = 0; i < synchedEvents.size(); i++) {
		synchedEvents.at(i)->load();	//needs to call old "setup" phase first...
	}
}



//void parseEvents()
//{
//	//try to minimize reallocation of TimingEvent list
//	//Comes from line as TEventSeq.
//	//Needs to end up in map<double, RawEvent>
//	//wrapper class (that extends abc TimingEvent) that stores each TEvent_var and has accessors?  RemoteTimingEvent
//}

//void DeviceEventEngine::parseEvents(const std::vector<TimingEvent>& eventsIn, ParsingResultsHandler& results)
void DeviceEventEngine::parseEvents(const TimingEventVector& eventsIn, ParsingResultsHandler& results)
{
	unsigned i;

	bool success = true;
	std::stringstream evtTransferErr;
	evtTransferErr.str("");

	unsigned errorCount = 0;	//limit the number of errors that are reported back during a single parse attempt
	unsigned maxErrors = 10;

	//This is only zero after resetting this device's parsed events.
	unsigned initialEventNumber = 0;

	//Move the events from TDeviceEventSeq 'events' (provided by server) to
	//the raw event list 'rawEvents'.  Check for general event errors.
	for(i = 0; i < eventsIn.size(); i++)
	{
		success &= addRawEvent(eventsIn.at(i), errorCount, maxErrors);

		if(errorCount > maxErrors)
		{
			success = false;

			//Too many errors; stop parsing and tell the user that there may be more
			evtTransferErr 
				<< "****Too many errors: Parsing aborted after " << i 
				<< " of " << eventsIn.size() << " events.***" << endl;
			break;
		}
	}

	if( success )
		//All events were added successfully.  
		//Now check for device-specific conflicts and errors while parsing.
		success = parseDeviceEvents();

	std::string errorMessage = evtTransferErr.str();

//	results.returnResults(device->getDeviceID(), engineID, success, errorMessage, const std::vector<TimingEvent>& eventsOut);

}


bool DeviceEventEngine::addRawEvent(const boost::shared_ptr<TimingEvent>& rawEvent, unsigned& errorCount, unsigned maxErrors)
{
	bool success = true;
	unsigned j;

	double eventTime = rawEvent->time();

	//add event
	rawEvents[eventTime]->add( rawEvent );
	
	//check for multiple events on the same channel at the same time
	for(j = 0; j < rawEvents[eventTime]->numberOfEvents() - 1; j++)
	{
		//Has the current event's channel already being set?
		if(rawEvent->channelNum() == rawEvents[eventTime]->at(j)->channelNum())
		{
			success = false;
			errorCount++;

			// channel name?  : << "('" << rawEvent. << "')"
			//Error: Multiple events scheduled on channel #24 at time 2.56:
			evtTransferErr << "Error: Multiple events scheduled on channel #" 
				<< rawEvent->channelNum() << " at time " 
				<< STI::Utils::printTimeFormated(eventTime) << ":" << endl
				<< "       Location: " << endl
				<< "       >>> " << rawEvents[eventTime]->at(j)->position().file() << ", line " 
				<< rawEvents[eventTime]->at(j)->position().line() << "." << endl
				<< "       >>> " << rawEvent->position().file() << ", line " 
				<< rawEvent->position().line() << "." << endl
				<< "       Event trace: " << endl
				<< "       " << rawEvents[eventTime]->at(j)->print() << endl
				<< "       " << rawEvents[eventTime]->back()->print() << endl;
		}
		if(errorCount > maxErrors)
			break;
	}
	
	//look for the newest event's channel number on this device
	ChannelMap::iterator channel = 
		channels.find(rawEvents[eventTime]->back()->channelNum());

	//check that newest event's channel is defined
	if(channel == channels.end())
	{
		success = false;
		errorCount++;

		//Error: Channel #24 is not defined on this device. Event trace:
		evtTransferErr << "Error: Channel #" 
			<< rawEvents[eventTime]->back()->channelNum()
			<< " is not defined on this device. "
			<< "       Location:" << endl
			<< "       >>> " << rawEvents[eventTime]->back()->position().file() << ", line " 
			<< rawEvents[eventTime]->back()->position().line() << "." << endl
			<< "       Event trace:" << endl
			<< "       " << rawEvents[eventTime]->back()->print() << endl;
	}
	//check that the newest event is of the correct type for its channel
	else if(rawEvents[eventTime]->back()->getValueType() != channel->second.outputType)
	{
		if(rawEvents[eventTime]->back()->isMeasurementEvent() && 
			rawEvents[eventTime]->back()->getValueType() == String && channel->second.outputType == Empty)
		{
			//In this case, we assume that the measurement's value is actually its description, since a (separate) description was not parsed.
			std::string desc = "";
			if(rawEvents[eventTime]->back()->getValueType() == String) {
				desc = rawEvents[eventTime]->back()->value().getString();
			}
			rawEvents[eventTime]->back()->getMeasurement()->setDescription( desc );
			
			rawEvents[eventTime]->back()->setValue( MixedValue() );	//makes this value Empty
		}
		else
		{
			success = false;
			errorCount++;

			//Error: Incorrect type found for event on channel #5. Expected type 'Number'. Event trace:
			evtTransferErr 
				<< "Error: Incorrect type found for event on channel #"
				<< channel->first << ". Expected type '" 
				<< STI::Utils::MixedValueTypeToStr(channel->second.outputType) << "'. " << endl
				<< "       Location:" << endl
				<< "       >>> " << rawEvents[eventTime]->back()->position().file() << ", line " 
				<< rawEvents[eventTime]->back()->position().line() << "." << endl
				<< "       Event trace:" << endl
				<< "       " << rawEvents[eventTime]->back()->print() << endl;
		}
	}
	if(success && rawEvents[eventTime]->back()->isMeasurementEvent())	//measurement event
	{
		//give ownership of the measurement to the measurements ptr_vector.
		measurements[timeStamp].push_back( rawEvents[eventTime]->back()->getMeasurement() );
	}

	return success;
}


bool DeviceEventEngine::parseDeviceEvents()
{
	unsigned i;
	TimingEventGroupMap::iterator badEvent;
//	STI::Types::TMeasurement measurement;

	bool success = true;
	bool errors = true;
	evtTransferErr.str("");

	unsigned errorCount = 0;	//limit the number of errors that are reported back during a single parse attempt
	unsigned maxErrors = 10;

	synchedEvents.clear();


	//better yet, the partner.event() call should always point to the EventEngineManager, which directs events to the currently parsing engine. Partner class needs public access to a EventEngineManager hook
//	setPartnerEventEngineDestination(this);		//make partner("").event() go to this engine's list of partner events

	do {
		errors = false;	//Each time through the loop any offending events 
						//are removed before trying again. This way all events
						//can generate errors messages before returning.
		try {
			parseDeviceEvents(rawEvents, synchedEvents);	//pure virtual
//			device->parseDeviceEvents(rawEvents, synchedEvents);	//pure virtual
		}
		catch(EventConflictException& eventConflict)
		{
			errorCount++;
			success = false;
			errors = true;
			//Error: Event conflict. <Device Specific Message>
			//       Event trace:
			evtTransferErr 
				<< "Error: Event conflict. "
				<< eventConflict.printMessage() << endl
				<< "       Location:" << endl
				<< "       >>> " << eventConflict.getEvent1()->position().file() << ", line " 
				<< eventConflict.getEvent1()->position().line() << "." << endl;

			if(eventConflict.getEvent1() != eventConflict.getEvent2())
			{
				evtTransferErr
				<< "       >>> " << eventConflict.getEvent2()->position().file() << ", line " 
				<< eventConflict.getEvent2()->position().line() << "." << endl;
			}

			evtTransferErr
				<< "       Event trace:" << endl
				<< "       " << eventConflict.getEvent1()->print() << endl;
			
			if(eventConflict.getEvent1() != eventConflict.getEvent2())
			{
				evtTransferErr
				<< "       " << eventConflict.getEvent2()->print() << endl;
			}
			
			//Add to list of conflicting events; this will be sent to the client
			conflictingEvents.insert(eventConflict.getEvent1()->eventNum());
			conflictingEvents.insert(eventConflict.getEvent2()->eventNum());

			//find the latest event associated with this exception
			badEvent = rawEvents.find( eventConflict.lastTime() );

			//remove all previous events from the map
			if(badEvent != rawEvents.end())
			{
				badEvent++;		//erase removes [first, last)
				rawEvents.erase( rawEvents.begin(), badEvent );
			}
			else	//this should never happen
				return false;		//break the error loop immediately
		}
		catch(EventParsingException& eventParsing)
		{
			errorCount++;
			success = false;
			errors = true;
			//Error: Event parsing error. <Device Specific Message>
			//       Event trace:
			evtTransferErr 
				<< "Error: Event parsing error. "
				<< eventParsing.printMessage() << endl
				<< "       Location:" << endl 
				<< "       >>> " << eventParsing.getEvent()->position().file() << ", line " 
				<< eventParsing.getEvent()->position().line() << "." << endl
				<< "       Event trace:" << endl
				<< "       " << eventParsing.getEvent()->print() << endl;
			
			//Add to list of unparseable events; this will be sent to the client
			unparseableEvents.insert(eventParsing.getEvent()->eventNum());

			//find the event associated with this exception
			badEvent = rawEvents.find( eventParsing.getEvent()->time() );

			//remove all previous events from the map
			if(badEvent != rawEvents.end())
			{
				badEvent++;		//erase removes [first, last)
				rawEvents.erase( rawEvents.begin(), badEvent );	
			}
			else	//this should never happen
				return false;		//break the error loop immediately
		}
		catch(STI_Exception& exception)
		{
			errorCount++;
			success = false;
			errors = true;

			evtTransferErr 
				<< "Error: " << exception.printMessage() << endl;
				
			return false;		//break the error loop immediately
		}
		catch(...)	//generic conflict or error
		{
			errorCount++;
			success = false;
			//Error: Event error or conflict detected. Debug info not available.
			evtTransferErr 
				<< "Error: Event error or conflict detected. " << endl
				<< "       Debug info not available." << endl;

			errors = false;		//break the error loop immediately
		}
		if(errorCount > maxErrors)
		{
			success = false;

			//Too many errors; stop parsing and tell the user that there may be more
			evtTransferErr 
				<< "****Too many errors: Parsing device events aborted after " << errorCount 
				<< " errors." << endl;

			return false;		//break the error loop immediately

		}
		if(rawEvents.size() == 0)
			break;

	} while(errors);

	if( !success )
		return false;

	errorCount = 0;
	//check that all measurements are associated with a SynchronousEvent
	for(i = 0; i < measurements[timeStamp].size(); i++)
	{
		if( !measurements[timeStamp].at(i)->isScheduled() )
		{
			errorCount++;
			success = false;
			evtTransferErr << "Error: The following measurement is not associated with a SynchronousEvent." << endl;
			
			//find the original RawEvent
			for(unsigned k = 0; k < rawEvents[ measurements[timeStamp].at(i)->time() ]->numberOfEvents(); k++)
			{
				if(rawEvents[ measurements[timeStamp].at(i)->time() ]->at(k)->eventNum() == measurements[timeStamp].at(i)->eventNum())
				{
					//found the raw event
					evtTransferErr
					<<         "       Location:" << endl
					<<         "       >>> " <<  rawEvents[ measurements[timeStamp].at(i)->time() ]->at(k)->position().file() << ", line " 
					<< rawEvents[ measurements[timeStamp].at(i)->time() ]->at(k)->position().line() << "." << endl;
				}
			}
			evtTransferErr
				<<             "       Measurement trace:" << endl
				<<             "       " << measurements[timeStamp].at(i)->print() << endl;
			evtTransferErr
				<<             "       Measurement description:" << endl
				<<             "       " << measurements[timeStamp].at(i)->getDescription() << endl;
		}
		
		if(errorCount > maxErrors)
		{
			success = false;

			//Too many errors; stop parsing and tell the user that there may be more
			evtTransferErr 
				<< "****Too many errors: Parsing aborted after " << errorCount 
				<< " measurement schedule errors." << endl;

			break;		//break the error loop immediately
		}
	}

	//check that no measurement is scheduled for more than one SynchronousEvent
	unsigned numberScheduled = 0;
	for(i = 0; i < synchedEvents.size(); i++)
	{
		numberScheduled += synchedEvents.at(i)->getNumberOfMeasurements();
	}
	
	if(numberScheduled != measurements[timeStamp].size() )
	{
		success = false;
		evtTransferErr << "Error: Measurement scheduling mismatch. Each DataMeasurement must be added" << endl
					   << "       to exactly one SynchronousEvent during parseDeviceEvents(...)." << endl
					   << "       Total Number of DataMeasurements: <<" << measurements.size() << ">>" << endl
					   << "       Number added to SynchronousEvents:  <<" << numberScheduled  << ">>" << endl;
	}

	//check that all event times are positive
	//shift all event times forward if they are negative? tell server the offset? include ping offset?

	//sort in time order
	//synchedEvents.sort();
	std::sort(synchedEvents.begin(), synchedEvents.end(), compareSynchronousEventPtrs);

	int synchedEventsLength = static_cast<int>( synchedEvents.size() );
	
	//check that synchedEvents has only one entry for each time
	for(int k = 0; k < synchedEventsLength - 1; k++)
	{
		if( synchedEvents.at(k)->getTime() == synchedEvents.at(k + 1)->getTime() )
		{
			evtTransferErr 
			<< "Error: Multiple parsed events are scheduled " << endl
			<< "       to occur at the same time on device '" << getDeviceName() << "'." << endl
			<< "       Events that occur on multiple channels at the same time must be grouped" << endl
			<< "       into a single SynchonousEvent object during STI_Device::parseDeviceEvents(...)." << endl
			<< "       Only one SynchonousEvent is allowed at any time." << endl;

			success = false;
			break;
		}
	}

	//Assign event numbers
	for(i = 0; i < synchedEvents.size(); i++)
	{
		synchedEvents.at(i)->setEventNumber( i );
	}

	return success;
}



void DeviceEventEngine::preTrigger(double startTime, double endTime)
{
//		setStop(false);

	//find first event
	for(unsigned j = 0; j < synchedEvents.size(); j++) {
		if( synchedEvents.at(j)->getTime() > startTime ) {
			firstEventToPlay = j;
			break;
		}
	}

	//find last event, (start looking from the end)
	unsigned lastIndex = synchedEvents.size() - 1;
	lastEventToPlay = lastIndex;
	for(unsigned j = 0; j < synchedEvents.size(); j++) {
		if( synchedEvents.at(lastIndex - j)->getTime() < endTime ) {
			lastEventToPlay = lastIndex - j;
			break;
		}
	}
}


void DeviceEventEngine::play(double startTime, double endTime) 
{
	time.preset(startTime);

	for(unsigned i = firstEventToPlay; i < lastEventToPlay; i++) {
		
		waitUntil( synchedEvents.at(i)->getTime() );	//returns at time event should play

		//if( checkStop() ) {
		//	time.pause();
		//	throw EngineStopException();
		//}
		//if( checkPause() ) {
		//	time.pause();
		//	throw EnginePauseException();
		//}

		if( !inState(Playing) ) {
//			throw EngineStopException( synchedEvents.at(i).getTime() );
		}

		synchedEvents.at(i)->play();

		//data is ready
		synchedEvents.at(i)->collectData();
//		collectionCondition->notify_one();	//wake up the collection thread?

		//reload
		synchedEvents.at(i)->load();

	}
	
	//this may be unneccessary -- in repeat mode at least we really prefer the devices wait for the trigger
//	if(repeats == 0)
	waitUntil(endTime);
}


void DeviceEventEngine::waitUntil(double time)
{
	boost::shared_lock< boost::shared_mutex > lock(playMutex);

	boost::system_time wakeTime = 
		boost::get_system_time() 
		+ boost::posix_time::milliseconds( static_cast<long>(time/1000000) );

	//wrap sleep in while loop to catch early wakeups (glitches)
	while( inState(Playing) && wakeTime > boost::get_system_time() ) {
		playCondition.timed_wait(lock, wakeTime);
	}
}
	
void DeviceEventEngine::publishData(const EngineTimestamp& timestamp, MixedValueVector& data)
{
	data.clear();
	DataMeasurementVector& storedMeasurements = measurements[timestamp];
	
	for(unsigned i = 0; i < storedMeasurements.size(); i++) {
		data.push_back( storedMeasurements.at(i) );
	}
	
	measurements.erase(timestamp);
}


