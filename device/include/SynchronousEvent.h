#ifndef STI_TIMINGENGINE_SYNCHRONOUSEVENT_H
#define STI_TIMINGENGINE_SYNCHRONOUSEVENT_H

namespace STI
{
namespace TimingEngine
{

class SynchronousEvent
{
public:

	SynchronousEvent() {}
	//SynchronousEvent(const SynchronousEvent &copy)  { device_ = copy.device_; time_ = copy.time_; }
	//SynchronousEvent(double time, STI_Device* device) : device_(device) 
	//{
	//	setTime(time);
	//	statusMutex = new omni_mutex();
	//	loadCondition = new omni_condition(statusMutex);
	//	playCondition = new omni_condition(statusMutex);
	//	collectionCondition = new omni_condition(statusMutex);
	//	setupDone = false;
	//	loaded = false;
	//	played = false;
	//}
	virtual ~SynchronousEvent() {}

	bool operator< (const SynchronousEvent &rhs) const { return (time_ < rhs.time_); }
	bool operator> (const SynchronousEvent &rhs) const { return (time_ > rhs.time_); }
	bool operator==(const SynchronousEvent &rhs) const { return (time_ == rhs.time_); }
	bool operator!=(const SynchronousEvent &rhs) const { return !((*this) == rhs); }

	void setup();
	void load();
	void play();
	void collectData();
	void reload();

	virtual void stop();
	virtual void reset();

	virtual void waitBeforeLoad();
	virtual void waitBeforePlay();
	virtual void waitBeforeCollectData();

	double getTime() { return time_; }
//	uInt64 getTime() { return time_; }
//	unsigned getEventNumber() { return eventNumber_; }
	unsigned getNumberOfMeasurements() { return eventMeasurements.size(); }
	
//	template<typename T> void setTime(T time) { time_ = static_cast<uInt64>(time); }
//	template<typename T> void setTime(T time) { time_ = static_cast<double>(time); }
//	template<typename T> void setData(T data) { time_ = static_cast<uInt64>(time); }

	void setEventNumber(unsigned eventNumber) { eventNumber_ = eventNumber; }
//	void addMeasurement(const RawEvent& measurementEvent);

private:
	virtual void setupEvent() = 0;
	virtual void loadEvent() = 0;
	virtual void playEvent() = 0;
	virtual void collectMeasurementData() = 0;

protected:
//	STI_Device* device_;
	DataMeasurementVector eventMeasurements;
///	std::vector<DataMeasurement*> eventMeasurements;
//
//	omni_mutex* statusMutex;
//	omni_condition* loadCondition;
//	omni_condition* playCondition;
//	omni_condition* collectionCondition;
//
//	bool played;
//	bool loaded;
//	bool setupDone;
//
//private:
//	uInt64 time_;
	double time_;
	unsigned eventNumber_;
//

};

}
}

#endif