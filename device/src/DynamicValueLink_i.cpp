

#include "DynamicValueLink_i.h"

DynamicValueLink_i::DynamicValueLink_i(const DynamicValue_ptr& value) 
: linked(false), dynamicValue(value)
{
	//Add the DynamicValueLink as a listener to the DynamicValue; this way changes to the DynamicValue will trigger 
	//calls to refresh on the DynamicValueLink.
	dynamicValue->addLink(this);
}

DynamicValueLink_i::~DynamicValueLink_i()
{
	dynamicValue->unLink(this);
}

void DynamicValueLink_i::addLink(STI::Server_Device::DynamicValueLink_ptr link)
{
	if(!linked) {
		dynamicValueLink = link;
		linked = true;
	}
}

void DynamicValueLink_i::unLink()
{
	linked = false;
}

//This function gets called by remote instances of the DynamicValueLink
//which intend to update the DynamicValue that is linked.
void DynamicValueLink_i::refreshLinkedValue(const STI::Server_Device::TNetworkDynamicValueEvent& evt)
{
	dynamicValue->setValue(evt.value);
}

//This function gets called by local DynamicValue instances that this
//DynamicValueLink_i is listening to. That is, any DynamicValue that has 
//DynamicValue::addLink(<pointer to this DynamicValueLink_i>).
//In general, this always contains the localed stored DynamicValue (called dynamicValue).
//Note: if two DynamicValueLink_i are liked to each other BOTH WAYS,
//the system will oscillate without end.  This should be avoided...
void DynamicValueLink_i::refresh(const DynamicValueEvent& evt)
{
	using STI::Server_Device::TNetworkDynamicValueEvent;
	using STI::Server_Device::TNetworkDynamicValueEvent_var;

	if(linked) {
		TNetworkDynamicValueEvent_var networkEvent(new TNetworkDynamicValueEvent());
		networkEvent->value = evt.getValue().getTValMixed();

		dynamicValueLink->refreshLinkedValue(networkEvent);
	}
//	forwardRefreshToLink(convert(evt));
}

