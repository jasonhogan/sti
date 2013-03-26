
#include "LocalEventEngine.h"

void parseEvents()
{
	//try to minimize reallocation of TimingEvent list
	//Comes from line as TEventSeq.
	//Needs to end up in map<double, RawEvent>
	//wrapper class (that extends abc TimingEvent) that stores each TEvent_var and has accessors?  RemoteTimingEvent
}


//classes to refactor:
//1. RawEvent->TimingEvent
//2. Combine MixedValue and MixedData. (elminate MixedData)

class TimingEvent
{
	unsigned short channelID;    // References TChannel list; TChannel contains TDevice, etc., needed by client
	unsigned short channelNum;   //== STI::Types::TChannel.channel;	Needed by the Device to parse.
	double         time;
	MixedValue value;	//actually a MixedValueRemote which extends MixedValue and wraps a TValMixed_var
	TPosition      pos;
	boolean        isMeasurementEvent;
	string         description;
	double         initialTimeHoldoff;
};



void DeviceEventEngine::parseEvents(const std::vector<TimingEvent>& eventsIn, ParsingResultsHandler& results)
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
				<< " of " << events.length() << " events.***" << endl;
			break;
		}
	}

	if( success )
		//All events were added successfully.  
		//Now check for device-specific conflicts and errors while parsing.
		success = parseDeviceEvents(evtTransferErr);

	std::string errorMessage = evtTransferErr.str();

	results.returnResults(device->getDeviceID(), engineID, success, errorMessage, const std::vector<TimingEvent>& eventsOut);

}


bool DeviceEventEngine::addRawEvent(const TimingEvent& rawEvent, unsigned& errorCount, unsigned maxErrors)
{
	bool success = true;
	unsigned j;

	double eventTime = rawEvent.time();

	//add event
	rawEvents[eventTime].push_back( rawEvent );
	
	//check for multiple events on the same channel at the same time
	for(j = 0; j < rawEvents[eventTime].size() - 1; j++)
	{
		//Has the current event's channel already being set?
		if(rawEvent.channel() == rawEvents[eventTime].at(j).channel())
		{
			success = false;
			errorCount++;

			// channel name?  : << "('" << rawEvent. << "')"
			//Error: Multiple events scheduled on channel #24 at time 2.56:
			evtTransferErr << "Error: Multiple events scheduled on channel #" 
				<< rawEvent.channel() << " at time " 
				<< STI::Utils::printTimeFormated(eventTime) << ":" << endl
				<< "       Location: " << endl
				<< "       >>> " << rawEvents[eventTime][j].file() << ", line " 
				<< rawEvents[eventTime][j].line() << "." << endl
				<< "       >>> " << rawEvent.file() << ", line " 
				<< rawEvent.line() << "." << endl
				<< "       Event trace: " << endl
				<< "       " << rawEvents[eventTime][j].print() << endl
				<< "       " << rawEvents[eventTime].back().print() << endl;
		}
		if(errorCount > maxErrors)
			break;
	}
	
	//look for the newest event's channel number on this device
	ChannelMap::iterator channel = 
		channels.find(rawEvents[eventTime].back().channel());
	
	//check that newest event's channel is defined
	if(channel == channels.end())
	{
		success = false;
		errorCount++;

		//Error: Channel #24 is not defined on this device. Event trace:
		evtTransferErr << "Error: Channel #" 
			<< rawEvents[eventTime].back().channel()
			<< " is not defined on this device. "
			<< "       Location:" << endl
			<< "       >>> " << rawEvents[eventTime].back().file() << ", line " 
			<< rawEvents[eventTime].back().line() << "." << endl
			<< "       Event trace:" << endl
			<< "       " << rawEvents[eventTime].back().print() << endl;
	}
	//check that the newest event is of the correct type for its channel
	else if(rawEvents[eventTime].back().getSTItype() != channel->second.outputType)
	{
		if(rawEvents[eventTime].back().isMeasurementEvent() && 
			rawEvents[eventTime].back().getSTItype() == ValueString && channel->second.outputType == ValueNone)
		{
			//In this case, we assume that the measurement's value is actually its description, since a (separate) description was not parsed.
			rawEvents[eventTime].back().getMeasurement()->setDescription(
				rawEvents[eventTime].back().stringValue() );
			
			rawEvents[eventTime].back().setValue( MixedValue() );	//makes this value Empty
		}
		else
		{
			success = false;
			errorCount++;

			//Error: Incorrect type found for event on channel #5. Expected type 'Number'. Event trace:
			evtTransferErr 
				<< "Error: Incorrect type found for event on channel #"
				<< channel->first << ". Expected type '" 
				<< TValueToStr(channel->second.outputType) << "'. " << endl
				<< "       Location:" << endl
				<< "       >>> " << raw_events[eventTime].back().file() << ", line " 
				<< rawEvents[eventTime].back().line() << "." << endl
				<< "       Event trace:" << endl
				<< "       " << rawEvents[eventTime].back().print() << endl;
		}
	}
	if(success && rawEvents[eventTime].back().isMeasurementEvent())	//measurement event
	{
		//give ownership of the measurement to the measurements ptr_vector.
		measurements.push_back( rawEvents[eventTime].back().getMeasurement() );
	}

	return success;
}


bool DeviceEventEngine::parseDeviceEvents(std::stringstream& evtTransferErr)
{
	unsigned i;
	RawEventMap::iterator badEvent;
	STI::Types::TMeasurement measurement;

	bool success = true;
	bool errors = true;
	evtTransferErr.str("");

	unsigned errorCount = 0;	//limit the number of errors that are reported back during a single parse attempt
	unsigned maxErrors = 10;

	synchedEvents.clear();


	//better yet, the partner.event() call should always point to the EventEngineManager, which directs events to the currently parsing engine. Partner class needs public access to a EventEngineManager hook
	setPartnerEventEngineDestination(this);		//make partner("").event() go to this engine's list of partner events

	do {
		errors = false;	//Each time through the loop any offending events 
						//are removed before trying again. This way all events
						//can generate errors messages before returning.
		try {
			device->parseDeviceEvents(rawEvents, synchedEvents);	//pure virtual
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
				<< "       >>> " << eventConflict.getEvent1().file() << ", line " 
				<< eventConflict.getEvent1().line() << "." << endl;

			if(eventConflict.getEvent1() != eventConflict.getEvent2())
			{
				evtTransferErr
				<< "       >>> " << eventConflict.getEvent2().file() << ", line " 
				<< eventConflict.getEvent2().line() << "." << endl;
			}

			evtTransferErr
				<< "       Event trace:" << endl
				<< "       " << eventConflict.getEvent1().print() << endl;
			
			if(eventConflict.getEvent1() != eventConflict.getEvent2())
			{
				evtTransferErr
				<< "       " << eventConflict.getEvent2().print() << endl;
			}
			
			//Add to list of conflicting events; this will be sent to the client
			conflictingEvents.insert(eventConflict.getEvent1().eventNum());
			conflictingEvents.insert(eventConflict.getEvent2().eventNum());

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
				<< "       >>> " << eventParsing.getEvent().file() << ", line " 
				<< eventParsing.getEvent().line() << "." << endl
				<< "       Event trace:" << endl
				<< "       " << eventParsing.getEvent().print() << endl;
			
			//Add to list of unparseable events; this will be sent to the client
			unparseableEvents.insert(eventParsing.getEvent().eventNum());

			//find the event associated with this exception
			badEvent = rawEvents.find( eventParsing.getEvent().time() );

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
	for(i = 0; i < measurements.size(); i++)
	{
		if( !measurements.at(i).isScheduled() )
		{
			errorCount++;
			success = false;
			evtTransferErr << "Error: The following measurement is not associated with a SynchronousEvent." << endl;
			
			//find the original RawEvent
			for(unsigned k = 0; k < rawEvents[ measurements.at(i).time() ].size(); k++)
			{
				if(rawEvents[ measurements.at(i).time() ].at(k).eventNum() == measurements.at(i).eventNum())
				{
					//found the raw event
					evtTransferErr
					<<         "       Location:" << endl
					<<         "       >>> " <<  rawEvents[ measurements.at(i).time() ].at(k).file() << ", line " 
					<< rawEvents[ measurements.at(i).time() ].at(k).line() << "." << endl;
				}
			}
			evtTransferErr
				<<             "       Measurement trace:" << endl
				<<             "       " << measurements.at(i).print() << endl;
			evtTransferErr
				<<             "       Measurement description:" << endl
				<<             "       " << measurements.at(i).getDescription() << endl;
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
		numberScheduled += synchedEvents.at(i).getNumberOfMeasurements();
	}
	
	if(numberScheduled != measurements.size() )
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
	synchedEvents.sort();

	int synchedEventsLength = static_cast<int>( synchedEvents.size() );
	
	//check that synchedEvents has only one entry for each time
	for(int k = 0; k < synchedEventsLength - 1; k++)
	{
		if( synchedEvents.at(k).getTime() == synchedEvents.at(k + 1).getTime() )
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
		synchedEvents.at(i).setEventNumber( i );
	}

	return success;
}
