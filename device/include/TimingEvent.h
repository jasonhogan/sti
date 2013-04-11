#ifndef STI_TIMINGENGINE_TIMINGEVENT_H
#define STI_TIMINGENGINE_TIMINGEVENT_H

#include "TextPosition.h"
#include "MixedValue.h"

namespace STI
{
namespace TimingEngine
{

//classes to refactor:
//1. RawEvent->TimingEvent
//2. Combine MixedValue and MixedData. (elminate MixedData)

class TimingEvent
{
public:
	double time() const;
	const STI::Utils::MixedValue& value() const;
	STI::Utils::MixedValueType getValueType() const;

	template<typename T> void setValue(const T& value)
	{
		value_l.setValue( value );
	}
	STI::Utils::MixedValue value_l;

	unsigned short channelID() const;
	unsigned short channelNum() const;

	unsigned eventNum() const;

	bool operator<(const TimingEvent &other) const;


	const STI::Parse::TextPosition& position() const;
	
	bool isMeasurementEvent() const;
	DataMeasurement_ptr getMeasurement() const;
	void setMeasurement(const DataMeasurement_ptr& measurement);


	const std::string& description() const;
	const std::string& print() const;
	
	double initialTimeHoldoff() const;



	//unsigned short channelID;    // References TChannel list; TChannel contains TDevice, etc., needed by client
	//unsigned short channelNum;   //== STI::Types::TChannel.channel;	Needed by the Device to parse.
	//double         time;
	//MixedValue value;	//actually a MixedValueRemote which extends MixedValue and wraps a TValMixed_var
	//TPosition      pos;
	//boolean        isMeasurementEvent;
	//string         description;
	//double         initialTimeHoldoff;
};

//	//try to minimize reallocation of TimingEvent list
//	//Comes from line as TEventSeq.
//	//Needs to end up in map<double, RawEvent>
//	//wrapper class (that extends abc TimingEvent) that stores each TEvent_var and has accessors?  RemoteTimingEvent

//class RemoteTimingEvent : public TimingEvent
//{
//public:
//	RemoteTimingEvent(TEvent_var evt);
//
//private:
//
//	TEvent_var tEvent;
//};


}
}

#endif
