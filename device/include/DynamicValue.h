
#ifndef STI_TIMINGENGINE_DYNAMICVALUE_H
#define STI_TIMINGENGINE_DYNAMICVALUE_H

#include "MixedValue.h"

#include <boost/shared_ptr.hpp>

#include <set>

class DynamicValue;
class DynamicValueListener;
class DynamicValueEvent;

typedef boost::shared_ptr<DynamicValue> DynamicValue_ptr;


class DynamicValue
{
public:

	DynamicValue();
	
	template<typename T> 
	DynamicValue(const T& value)
	{
		setValue(value);
	}	

	template<typename T>
	void setValue(const T& val)
	{
		setValue(MixedValue(val));
	}

	virtual void setValue(const MixedValue& val);
	const MixedValue& getValue() const;

	void addLink(DynamicValueListener* listener);
	void unLink(DynamicValueListener* listener);

private:

	void pushRefreshEvent(const DynamicValueEvent& evt);

	std::set<DynamicValueListener*> listeners;
	MixedValue value_l;
};

class DynamicValueEvent
{
public:
	DynamicValueEvent(const MixedValue& value) : value_l(value) {}
	const MixedValue& getValue() const { return value_l;}

private:
	const MixedValue& value_l;
};

class DynamicValueListener
{
public:
	virtual void refresh(const DynamicValueEvent& evt) = 0;
};



#endif

