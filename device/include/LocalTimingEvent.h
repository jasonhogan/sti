#ifndef STI_TIMINGENGINE_LOCALTIMINGEVENT_H
#define STI_TIMINGENGINE_LOCALTIMINGEVENT_H

#include "TimingEngineTypes.h"
#include "TimingEvent.h"
#include "TextPosition.h"
#include "ScheduledMeasurement.h"

namespace STI
{
namespace TimingEngine
{

//classes to refactor:
//1. RawEvent->TimingEvent

	class LocalTimingEvent : public TimingEvent
{
public:

//	LocalTimingEvent(double time, unsigned short channel, unsigned eventNumber, bool isMeasurementEvent=false);
	
	template<typename T> 
	LocalTimingEvent(double time, unsigned short channel, const T& value, 
		unsigned eventNumber, const TextPosition& position, bool isMeasurementEvent=false) 
		: time_l(time), channelNum_l(channel), value_l(value), 
		eventNumber_l(eventNumber), position_l(position), isMeasurement_l(isMeasurementEvent)
	{
		if(isMeasurement_l) {
			measurement_l = ScheduledMeasurement_ptr( new ScheduledMeasurement(channel, eventNumber) );
		}
	}

	bool operator==(const TimingEvent& rhs) const;
	bool operator!=(const TimingEvent& rhs) const;
	bool operator<(const TimingEvent& rhs) const;
	bool operator>(const TimingEvent& rhs) const;

	const EventTime& time() const;
	unsigned short channelNum() const;
	const STI::Utils::MixedValue& value() const;

	unsigned eventNum() const;
	bool isMeasurementEvent() const;

//	unsigned short channelID() const;
	
	const TextPosition& position() const;

//	double initialTimeHoldoff() const;

	bool getMeasurement(ScheduledMeasurement_ptr& measurement) const;

private:
	ScheduledMeasurement_ptr measurement_l;
	
	EventTime         time_l;
	unsigned short channelNum_l;   //== STI::Types::TChannel.channel
//	unsigned short channelID_l;
	STI::Utils::MixedValue value_l;
	unsigned eventNumber_l;
	STI::TimingEngine::TextPosition position_l;
	bool isMeasurement_l;
};

//class TimingEvent
//{
//public:
//	TimingEvent();
//
//	bool operator==(const TimingEvent& rhs) const;
//	bool operator!=(const TimingEvent& rhs) const;
//	bool operator<(const TimingEvent& rhs) const;
//	bool operator>(const TimingEvent& rhs) const;
//	
//	double time() const;
//	const STI::Utils::MixedValue& value() const;
//
//	STI::Utils::MixedValueType getValueType() const;
//	template<typename T> void setValue(const T& value)
//	{
//		value_l.setValue( value );
//	}
//
//	unsigned short channelID() const;
//	unsigned short channelNum() const;
//
//	unsigned eventNum() const;
//	bool isMeasurementEvent() const;
//
//	double initialTimeHoldoff() const;
//	const STI::Parse::TextPosition& position() const;
//
//	DataMeasurement_ptr getMeasurement() const;
//	void setMeasurement(const DataMeasurement_ptr& measurement);
//
//	const std::string& description() const;
//	const std::string& print() const;
//	
//private:
//
//	double         time_l;
//	unsigned short channelNum_l;   //== STI::Types::TChannel.channel
//	unsigned short channelID_l;
//	STI::Utils::MixedValue value_l;
//
//	unsigned eventNumber_l;
//
//	bool isMeasurement;
//	DataMeasurement_ptr measurement_;
//
//	TextPosition textPosition;
//
//	//unsigned short channelID;    // References TChannel list; TChannel contains TDevice, etc., needed by client
//	//unsigned short channelNum;   //== STI::Types::TChannel.channel;	Needed by the Device to parse.
//	//double         time;
//	//MixedValue value;	//actually a MixedValueRemote which extends MixedValue and wraps a TValMixed_var
//	//TPosition      pos;
//	//boolean        isMeasurementEvent;
//	//string         description;
//	//double         initialTimeHoldoff;
//};
//
////	//try to minimize reallocation of TimingEvent list
////	//Comes from line as TEventSeq.
////	//Needs to end up in map<double, RawEvent>
////	//wrapper class (that extends abc TimingEvent) that stores each TEvent_var and has accessors?  RemoteTimingEvent
//
////class RemoteTimingEvent : public TimingEvent
////{
////public:
////	RemoteTimingEvent(TEvent_var evt);
////
////private:
////
////	TEvent_var tEvent;
////};
//

}
}

#endif
