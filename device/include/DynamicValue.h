
#ifndef STI_TIMINGENGINE_DYNAMICVALUE_H
#define STI_TIMINGENGINE_DYNAMICVALUE_H

#include "MixedValue.h"

#include "TimingEngineTypes.h"

#include <set>


namespace STI
{
namespace TimingEngine
{


class DynamicValue
{
public:

	DynamicValue();
	virtual ~DynamicValue() {}
	
	template<typename T> 
	DynamicValue(const T& value)
	{
		setValue(value);
	}	

	template<typename T>
	void setValue(const T& val)
	{
		setValue(STI::Utils::MixedValue(val));
	}

	virtual void setValue(const STI::Utils::MixedValue& val);
	const STI::Utils::MixedValue& getValue() const;

	void addLink(DynamicValueListener* listener);
	void unLink(DynamicValueListener* listener);

private:

	void pushRefreshEvent(const DynamicValueEvent& evt);

	std::set<DynamicValueListener*> listeners;
	STI::Utils::MixedValue value_l;
};

class DynamicValueEvent
{
public:
	DynamicValueEvent(const STI::Utils::MixedValue& value) : value_l(value) {}
	const STI::Utils::MixedValue& getValue() const { return value_l;}

private:
	const STI::Utils::MixedValue& value_l;
};


}
}


#endif

