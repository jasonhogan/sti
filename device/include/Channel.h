#ifndef STI_TIMINGENGINE_CHANNEL_H
#define STI_TIMINGENGINE_CHANNEL_H

#include "DeviceID.h"
#include "MixedValue.h"

#include <string>

namespace STI
{
namespace TimingEngine
{

enum ChannelType {Output, Input, BiDirectional, ChannelTypeUnknown};

//enum MixedValueType {Boolean, Octet, Int, Double, String, File, Vector, Empty};

class Channel 
{
public:
	
	Channel(
		const STI::Device::DeviceID& device, 
		unsigned short channel,
		ChannelType ioType,	
		const std::string& channelName,	
		STI::Utils::MixedValueType inputType,
		STI::Utils::MixedValueType outputType) 
		: device_l(device), channel_l(channel), ioType_l(ioType), 
		channelName_l(channelName), inputType_l(inputType), outputType_l(outputType)
	{}
	
	bool operator<(const Channel &rhs) const { return (device_l < rhs.device_l) || (channel_l < rhs.channel_l); }
	
	const STI::Device::DeviceID& deviceID() const { return device_l; }
	unsigned short channelNum() const { return channel_l; }
	ChannelType IOType() const { return ioType_l; }
	const std::string& channelName() const { return channelName_l; }
	
	STI::Utils::MixedValueType inputType() const { return inputType_l; }
	STI::Utils::MixedValueType outputType() const { return outputType_l; }

private:

	const STI::Device::DeviceID& device_l;
	unsigned short channel_l;
	ChannelType ioType_l;
	std::string channelName_l;
	
	// Types that this channel expects
	STI::Utils::MixedValueType inputType_l;
	STI::Utils::MixedValueType outputType_l;
};


}
}

#endif
