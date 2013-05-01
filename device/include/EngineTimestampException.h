#ifndef STI_TIMINGENGINE_ENGINETIMESTAMPEXCEPTION_H
#define STI_TIMINGENGINE_ENGINETIMESTAMPEXCEPTION_H

#include "EngineException.h"
#include "EngineID.h"

namespace STI
{
namespace TimingEngine
{

class EngineTimestampException : public EngineException
{
public:

	EngineTimestampException(const std::string& message, 
		const EngineTimestamp& current, const EngineTimestamp& last) 
		: EngineException(message), currentTimestamp(current), lastTimestamp(last) {}
	~EngineTimestampException() throw() {}

	const EngineTimestamp currentTimestamp;
	const EngineTimestamp lastTimestamp;

};

}
}

#endif

