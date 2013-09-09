#ifndef STI_TIMINGENGINE_TIMINGEVENT_H
#define STI_TIMINGENGINE_TIMINGEVENT_H

#include "TimingEngineTypes.h"
#include "EventTime.h"
#include "MixedValue.h"

namespace STI
{
namespace TimingEngine
{

//classes to refactor:
//1. RawEvent->TimingEvent

class TimingEvent
{
public:

	virtual ~TimingEvent() {}

	virtual bool operator==(const TimingEvent& rhs) const = 0;
	virtual bool operator!=(const TimingEvent& rhs) const = 0;
	virtual bool operator<(const TimingEvent& rhs) const = 0;
	virtual bool operator>(const TimingEvent& rhs) const = 0;

	virtual const EventTime& time() const = 0;
//	virtual unsigned short channelNum() const = 0;

	virtual const Channel& channel() const = 0;

	//A TaggedValue concept could give the parseDeviceEvents() function a chance 
	//to optimize patched parsing by directly registering the TaggedValue with
	//a SynchronousEvent.
	//To take advantage, an optimized device would extend TaggedSynchronousEvent
	//which would take a reference to the TaggedValue and connect them somehow.
	//It might want an interface with a setValue() function hook that parsePatch
	//could call. Only would be used if the event times are the same.
	virtual const STI::Utils::MixedValue& value() const = 0;

	virtual bool getDynamicValue(STI::TimingEngine::DynamicValue_ptr& dynamicValue) const = 0;

	virtual const std::string& description() const = 0;

	virtual unsigned eventNum() const = 0;
	virtual bool isMeasurementEvent() const = 0;

	virtual const TextPosition& position() const = 0;

//	virtual unsigned short channelID() const = 0;
//	virtual double initialTimeHoldoff() const = 0;

	bool getMeasurement(ScheduledMeasurement_ptr& measurement) const;
	void setMeasurement(ScheduledMeasurement_ptr& measurement);

private:

	mutable ScheduledMeasurement_weak_ptr measurement_l;
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
