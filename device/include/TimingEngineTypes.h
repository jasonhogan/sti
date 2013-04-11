#ifndef STI_TIMINGENGINE_TIMINGENGINETYPES_H
#define STI_TIMINGENGINE_TIMINGENGINETYPES_H

#include <boost/shared_ptr.hpp>

#include <vector>
#include <map>

namespace STI
{
	namespace Utils
	{
		class MixedValue;
	}
	
	namespace TimingEngine
	{
		class EngineTimestamp;
		
		class ParsingResultsHandler;

		class DataMeasurement;

		class Channel;

		class TimingEvent;
		class TimingEventGroup;
		class SynchronousEvent;

		typedef std::vector< boost::shared_ptr<STI::Utils::MixedValue> > MixedValueVector;
		
	//	typedef const std::vector< boost::shared_ptr<TimingEvent> > ConstTimingEventVector;
		typedef boost::shared_ptr<TimingEvent> TimingEvent_ptr;
		typedef std::vector< TimingEvent_ptr > TimingEventVector;
		typedef std::map<double, boost::shared_ptr<TimingEventGroup> > TimingEventGroupMap;	//map time to TimingEventGroup

		typedef boost::shared_ptr<SynchronousEvent> SynchronousEvent_ptr;
		typedef std::vector< SynchronousEvent_ptr > SynchronousEventVector;
	
		typedef boost::shared_ptr<DataMeasurement> DataMeasurement_ptr;
		typedef std::vector< DataMeasurement_ptr > DataMeasurementVector;
		typedef std::map<EngineTimestamp, DataMeasurementVector> DataMeasurementMap;

		typedef std::map<unsigned short, Channel> ChannelMap;	//map channel number to Channel

	}
}



#endif
