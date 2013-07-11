

#include "DynamicValueLink_i.h"

DynamicValueLink_i::DynamicValueLink_i(const DynamicValue_ptr& value) 
: hasLinkTarget(false), isLinkedToRemoteSource(false), dynamicValue(value)
{
	//Add the DynamicValueLink as a listener to the DynamicValue; this way changes to the DynamicValue will trigger 
	//calls to refresh on the DynamicValueLink.
	dynamicValue->addLink(this);
}
DynamicValueLink_i::DynamicValueLink_i(const DynamicValue_ptr& value, const STI::Server_Device::DynamicValueLink_var& dynamicValueLinkRef) 
: hasLinkTarget(false), isLinkedToRemoteSource(false), dynamicValueLink(dynamicValueLinkRef), dynamicValue(value)
{
	//Add the DynamicValueLink as a listener to the DynamicValue; this way changes to the DynamicValue will trigger 
	//calls to refresh on the DynamicValueLink.
	dynamicValue->addLink(this);

	//Give the remote instance of the DynamicValueLink a reference to the local instance.
	//This lets the remote instance trigger refresh events on the local DynamicValue.
	try {
		dynamicValueLink->addLink(_this());
		isLinkedToRemoteSource = true;
	} catch(...) {
	}

}
DynamicValueLink_i::~DynamicValueLink_i()
{
	dynamicValue->unLink(this);
	
	if(isLinkedToRemoteSource && dynamicValueLink != 0) {
		try {
			//really need to come up with a robust way to disconnect before either end gets deleted...
//			dynamicValueLink->unLink();
		} catch(...) {
		}
	}
}

void DynamicValueLink_i::addLink(STI::Server_Device::DynamicValueLink_ptr link)
{
	if(!hasLinkTarget && !isLinkedToRemoteSource) {		//no two way links allowed!
		dynamicValueLink = STI::Server_Device::DynamicValueLink::_duplicate(link);
		hasLinkTarget = true;
	}
}

void DynamicValueLink_i::unLink()
{
	hasLinkTarget = false;
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

	if(hasLinkTarget) {
		TNetworkDynamicValueEvent_var networkEvent(new TNetworkDynamicValueEvent());
		networkEvent->value = evt.getValue().getTValMixed();

		try {
			dynamicValueLink->refreshLinkedValue(networkEvent);
		} catch(...) {
		}
	}
//	forwardRefreshToLink(convert(evt));
}

