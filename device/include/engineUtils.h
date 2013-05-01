
#ifndef STI_UTILS_ENGINEUTILS_H
#define STI_UTILS_ENGINEUTILS_H

#include <string>

#include "TimingEngineTypes.h"

namespace STI
{
	namespace Utils
	{
		std::string print(const STI::TimingEngine::TimingEvent_ptr ptrEvent);
		std::string print(const STI::TimingEngine::TimingEvent& tEvent);
		std::string print(const STI::TimingEngine::TimingMeasurement_ptr ptrEvent);
		std::string print(const STI::TimingEngine::TimingMeasurement& tMeas);
	}
}

#endif

