
#ifndef STI_TIMINGENGINE_DYNAMICVALUELISTENER_H
#define STI_TIMINGENGINE_DYNAMICVALUELISTENER_H

#include "TimingEngineTypes.h"

namespace STI
{
namespace TimingEngine
{

class DynamicValueListener
{
public:
	virtual void refresh(const DynamicValueEvent& evt) = 0;
};

}
}


#endif
