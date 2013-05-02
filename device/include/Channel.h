#ifndef STI_TIMINGENGINE_CHANNEL_H
#define STI_TIMINGENGINE_CHANNEL_H

#include "DeviceID.h"

#include <string>

namespace STI
{
namespace TimingEngine
{

enum ChannelType {Output, Input, BiDirectional, Unknown};

//enum MixedValueType {Boolean, Octet, Int, Double, String, File, Vector, Empty};

class Channel 
{
public:
	bool operator<(const Channel &rhs) const { return (device < rhs.device) || (channel < rhs.channel); }
	
	const STI::Device::DeviceID& device;
	unsigned short channel;
	ChannelType IOType;
	std::string channelName;
	
	// Types that this channel expects
	STI::Utils::MixedValueType inputType;
	STI::Utils::MixedValueType outputType;
};


}
}

#endif
