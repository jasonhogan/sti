#ifndef STI_TIMINGENGINE_SYNCHRONOUSEVENT_H
#define STI_TIMINGENGINE_SYNCHRONOUSEVENT_H

#include "TimingEngineTypes.h"
#include "EventTime.h"

namespace STI
{
namespace TimingEngine
{

//bool compareSynchronousEventPtrs(SynchronousEvent_ptr l,SynchronousEvent_ptr r);

//class TaggedSynchronousEvent : public SynchronousEvent
//{
//	TaggedSynchronousEvent(EventTime time, const TaggedValue_ptr& value)
//	{
//		//value->attachEvent(shared_ptr_to_this); // (no, make the user do this in parseDeviceEvent; see below
//	}
//
//	virtual void setValue(const TaggedValue& value) = 0;
//
//};

////and maybe:
//
//class TaggedValue
//{
//Makes more sense for these functions to be members of TimingEvent, and have TaggedValue be a private member of TimingEvent rather than expose two value types
//	void attachTaggedEvent(const TaggedSynchronousEvent_ptr& evt, EventTime minTimeBefore, EventTime, minTimeAfter);
//	void attachTaggedEvent(const TaggedSynchronousEvent_ptr& evt);
//}

class SynchronousEvent
{
public:

//	SynchronousEvent() {}
	SynchronousEvent(const STI::TimingEngine::EventTime& time);
//	SynchronousEvent(EventTime time, STI::Device::STI_Device* device);
	virtual ~SynchronousEvent() {}

	bool operator< (const SynchronousEvent &rhs) const { return (time_ < rhs.time_); }
	bool operator> (const SynchronousEvent &rhs) const { return (time_ > rhs.time_); }
	bool operator==(const SynchronousEvent &rhs) const { return (time_ == rhs.time_); }
	bool operator!=(const SynchronousEvent &rhs) const { return !((*this) == rhs); }

	void load();
	void play();
	void collectData(const TimingMeasurementGroup_ptr& measurements);
	void publishData(const TimingMeasurementGroup_ptr& measurements, DocumentationOptions_ptr documentation);
	void reload();

	void addMeasurement(const TimingEvent_ptr& measurementEvent);
	void setTime(const EventTime& time) { time_ = time; }
	void setEventNumber(unsigned eventNumber) { eventNumber_ = eventNumber; }

	EventTime getTime() const { return time_; }
	unsigned getEventNumber() const { return eventNumber_; }
	unsigned getNumberOfMeasurements() const { return scheduledMeasurements.size(); }
	
//	template<typename T> void setTime(T time) { time_ = static_cast<double>(time); }
//	template<typename T> void setData(T data) { time_ = static_cast<uInt64>(time); }


private:
	virtual void setupEvent() = 0;
	virtual void loadEvent() = 0;
	virtual void playEvent() = 0;
	virtual void collectMeasurements(TimingMeasurementVector& measurementsOut) = 0;
	virtual void publishMeasurements(const TimingMeasurementVector& measurements) = 0;
	virtual void reloadEvent() = 0;
//	virtual void collectMeasurementData() = 0;

protected:

	ScheduledMeasurementVector scheduledMeasurements;
//	STI_Device* device_;

private:

	EventTime time_;
	unsigned eventNumber_;
};

}
}

#endif

