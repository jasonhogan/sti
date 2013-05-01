#ifndef STI_TIMINGENGINE_TIMINGENGINETYPES_H
#define STI_TIMINGENGINE_TIMINGENGINETYPES_H

#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>

#include "DeviceTypes.h"
#include "SynchronizedMap.h"

namespace STI
{
	//namespace Utils
	//{
	//	class MixedValue;
	//	typedef std::vector< boost::shared_ptr<STI::Utils::MixedValue> > MixedValueVector;
	//}
	
	namespace TimingEngine
	{
		class EngineTimestamp;
		class EngineInstance;
		class EngineID;
		
		class DocumentationOptions;
		class ParsingResultsHandler;
		typedef boost::shared_ptr<DocumentationOptions> DocumentationOptions_ptr;
		typedef boost::shared_ptr<ParsingResultsHandler> ParsingResultsHandler_ptr;

		class LoadAccessPolicy;
		typedef boost::shared_ptr<LoadAccessPolicy> LoadAccessPolicy_ptr;

		class Trigger;
		typedef boost::shared_ptr<Trigger> Trigger_ptr;

		class EventEngine;
		typedef boost::shared_ptr<STI::TimingEngine::EventEngine> EventEngine_ptr;
		class DeviceEventEngine;
		typedef boost::shared_ptr<STI::TimingEngine::DeviceEventEngine> DeviceEventEngine_ptr;


//		class EventTime;

		class Channel;

		class TextPosition;

		class TimingEvent;
		class TimingEventGroup;

		class TimingMeasurement;
		class ScheduledMeasurement;
		class TimingMeasurementResult;
		class TimingMeasurementGroup;

		class SynchronousEvent;

		class MasterTrigger;
		typedef boost::shared_ptr<MasterTrigger> MasterTrigger_ptr;
		
	//	typedef const std::vector< boost::shared_ptr<TimingEvent> > ConstTimingEventVector;
		typedef boost::shared_ptr<TimingEvent> TimingEvent_ptr;
		typedef std::vector< TimingEvent_ptr > TimingEventVector;
		typedef boost::shared_ptr<TimingEventGroup> TimingEventGroup_ptr;
		typedef std::map<double, TimingEventGroup_ptr > TimingEventGroupMap;	//map time to TimingEventGroup

		typedef std::map<STI::Device::DeviceIDString, TimingEventVector > DeviceTimingEventsMap;	//map time to TimingEventVectors, sorted by device target

		typedef boost::shared_ptr<SynchronousEvent> SynchronousEvent_ptr;
		typedef std::vector< SynchronousEvent_ptr > SynchronousEventVector;

		typedef boost::shared_ptr<TimingMeasurement> TimingMeasurement_ptr;
		typedef std::vector< TimingMeasurement_ptr > TimingMeasurementVector;
		
		
//		typedef std::map<EngineTimestamp, TimingMeasurementVector> TimingMeasurementMap;
		typedef boost::shared_ptr<TimingMeasurementGroup> TimingMeasurementGroup_ptr;
		typedef std::map<EngineTimestamp, TimingMeasurementGroup_ptr> TimingMeasurementGroupMap;
//		typedef STI::Utils::SynchronizedMap<EngineTimestamp, TimingMeasurementGroup_ptr> TimingMeasurementGroupMap;



		typedef boost::shared_ptr<TimingMeasurementResult> TimingMeasurementResult_ptr;
		typedef std::vector< TimingMeasurementResult_ptr > TimingMeasurementResultVector;

		typedef boost::shared_ptr<ScheduledMeasurement> ScheduledMeasurement_ptr;
		typedef std::vector< ScheduledMeasurement_ptr > ScheduledMeasurementVector;

		typedef std::map<unsigned short, Channel> ChannelMap;	//map channel number to Channel


	}
}



#endif
