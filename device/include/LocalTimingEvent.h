#ifndef STI_TIMINGENGINE_LOCALTIMINGEVENT_H
#define STI_TIMINGENGINE_LOCALTIMINGEVENT_H

#include "TimingEngineTypes.h"
#include "TimingEvent.h"
#include "TextPosition.h"
#include "ScheduledMeasurement.h"
#include "Channel.h"

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
	LocalTimingEvent(double time, const STI::TimingEngine::Channel& channel, const T& value, 
		unsigned eventNumber, const TextPosition& position, const std::string& description = "", bool isMeasurementEvent=false) 
		: time_l(time), channel_l(channel), value_l(value), 
		eventNumber_l(eventNumber), position_l(position), description_l(description), 
		isMeasurement_l(isMeasurementEvent), hasDynamicValue(false)
	{
		//if(isMeasurement_l) {
		//	makeScheduledMeasurement(channel, eventNumber);
		//}
	}
	bool operator==(const TimingEvent& rhs) const;
	bool operator!=(const TimingEvent& rhs) const;
	bool operator<(const TimingEvent& rhs) const;
	bool operator>(const TimingEvent& rhs) const;

	const EventTime& time() const;
//	unsigned short channelNum() const;
	const Channel& channel() const;
	const STI::Utils::MixedValue& value() const;

	bool getDynamicValue(STI::TimingEngine::DynamicValue_ptr& dynamicValue) const;

	unsigned eventNum() const;
	bool isMeasurementEvent() const;

//	unsigned short channelID() const;
	
	const TextPosition& position() const;
	const std::string& description() const;

//	double initialTimeHoldoff() const;

//	bool getMeasurement(TimingMeasurement_ptr& measurement) const;

private:

//	void makeScheduledMeasurement(const STI::TimingEngine::Channel& channel, unsigned eventNumber);

//	TimingMeasurement_ptr measurement_l;
	
	EventTime         time_l;
//	unsigned short channelNum_l;   //== STI::Types::TChannel.channel
//	unsigned short channelID_l;
	Channel channel_l;
	STI::Utils::MixedValue value_l;
	unsigned eventNumber_l;
	STI::TimingEngine::TextPosition position_l;
	bool isMeasurement_l;

	bool hasDynamicValue;
	DynamicValue_ptr dynamicValue_l;

	std::string description_l;
//	DynamicValueLink_i_ptr dynamicValueLink;

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
