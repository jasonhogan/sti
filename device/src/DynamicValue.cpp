
#include "DynamicValue.h"

DynamicValue::DynamicValue()
{
	setValue(MixedValue());
}


void DynamicValue::setValue(const MixedValue& val)
{
	value_l = val;

	//update all linked listeners
	DynamicValueEvent evt(value_l);
	pushRefreshEvent(evt);
}

void DynamicValue::pushRefreshEvent(const DynamicValueEvent& evt)
{
	//DynamicValueListener* listener;
	//for(unsigned i = 0; i < listeners.size(); i++) {
	//	listener = listeners.at(i);
	//	if(listener != 0) {
	//		listener->refresh(evt);
	//	}
	//}

	std::set<DynamicValueListener*>::iterator listener;
	for(listener = listeners.begin(); listener != listeners.end(); listener++) {
		if((*listener) != 0) {
			(*listener)->refresh(evt);
		}
	}
}

const MixedValue& DynamicValue::getValue() const
{
	return value_l;
}

void DynamicValue::addLink(DynamicValueListener* listener)
{
	if(listener != 0) {
		listeners.insert(listener);
//		listeners.push_back(listener);
	}
}

void DynamicValue::unLink(DynamicValueListener* listener)
{
	//Try to find and remove this listener pointer
	std::set<DynamicValueListener*>::iterator it = listeners.find(listener);

	if(it != listeners.end()) {
		listeners.erase(it);
	}

//	std::set<DynamicValueListener*> temp;
//	std::set<DynamicValueListener*>::iterator it = temp;
//	listeners.clear();
}

