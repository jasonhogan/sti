
#include "engineUtils.h"
#include "utils.h"
#include "TimingEvent.h"
#include "TimingMeasurement.h"

#include <sstream>

		
std::string STI::Utils::print(const STI::TimingEngine::TimingEvent_ptr ptrEvent)
{
	if(ptrEvent)
		return STI::Utils::print(*ptrEvent);
	else
		return "<Null Pointer>";
}



std::string STI::Utils::print(const STI::TimingEngine::TimingEvent& tEvent)
{
	std::stringstream evt;

	//<Time=2.1, Channel=4, Type=Number, Value=3.4>
	evt << "<Time=" << STI::Utils::printTimeFormated(tEvent.time());
	evt << ", Channel=" << tEvent.channelNum();
	evt << ", Type=" << STI::Utils::print(tEvent.value().getType());
	evt << ", Value=" << tEvent.value().print() << ">";
	
	return evt.str();
}

std::string STI::Utils::print(const STI::TimingEngine::TimingMeasurement_ptr ptrMeas)
{
	if(ptrMeas)
		return STI::Utils::print(*ptrMeas);
	else
		return "<Null Pointer>";
}

std::string STI::Utils::print(const STI::TimingEngine::TimingMeasurement& tMeas)
{
	std::stringstream meas;

//	<Time=2.1, Channel=4, Type=Double, Value=3.4>
	meas << "<Time=" << STI::Utils::printTimeFormated(tMeas.time());
	meas << ", Channel=" << tMeas.channel();
	meas << ", Type=" << STI::Utils::print(tMeas.measuredValue().getType());
	meas << ", Data=" << tMeas.measuredValue().print();
	meas << ">";	
	
	return meas.str();
}