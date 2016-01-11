#ifndef STI_TIMINGENGINE_ENGINEEXCEPTION_H
#define STI_TIMINGENGINE_ENGINEEXCEPTION_H

#include "STI_Exception.h"

#include <string>

namespace STI
{
namespace TimingEngine
{

class EngineException : public STI::Utils::STI_Exception
{
public:

	EngineException(const std::string& message) : STI::Utils::STI_Exception(message) {}
	virtual ~EngineException() {}

};

}
}

#endif

