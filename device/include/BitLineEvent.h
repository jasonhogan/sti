#ifndef STI_TIMINGENGINE_BITLINEEVENT_H
#define STI_TIMINGENGINE_BITLINEEVENT_H

#include "TimingEngineTypes.h"
#include "DeviceTypes.h"
#include "SynchronousEvent.h"
#include "TimingEvent.h"

#include <bitset>

namespace STI
{
namespace TimingEngine
{

template<int N>
class BitLineEvent : public SynchronousEvent
{
public:
	BitLineEvent() : SynchronousEvent() { bits.reset(); }
	BitLineEvent(const BitLineEvent &copy) 
		: SynchronousEvent(copy) { }
	BitLineEvent(double time, STI::Device::STI_Device* device) 
		: SynchronousEvent(time, device) { bits.reset(); }
	BitLineEvent(double time, uInt32 value, STI::Device::STI_Device* device) 
		: SynchronousEvent(time, device) { setBits(value); }
	virtual ~BitLineEvent() {};

	//assign 'value' to bits LSB to MSB
	template <typename T>
	BitLineEvent<N>* setBits(T value, unsigned LSB=0, unsigned MSB=(N-1)) 
	{
		unsigned numBits = sizeof(T) * CHAR_BIT;
		unsigned i,j;
		for(i = LSB, j = 0; i <= MSB && j < numBits && i < N; i++, j++)
			bits.set(i, ((value >> j) & 0x1) == 0x1 );

		return this;
	};
	//get the value of bits 'first' to 'last'
	uInt32 getBits(unsigned first=0, unsigned last=(N-1)) const
	{
		unsigned i,j;
		uInt32 value = 0;
		for(i = first, j = 0; i <= last && j < 32 && i < N; i++, j++)
			value += ( (bits.test(i) ? 0x1 : 0x0) << j);
		return value;
	}
	uInt32 getValue() const { return getBits(); }

private:
	virtual void setupEvent() = 0;
	virtual void loadEvent() = 0;
	virtual void playEvent() = 0;
	virtual void collectMeasurementData() = 0;

protected:
	std::bitset<N> bits;
};

}
}

#endif

