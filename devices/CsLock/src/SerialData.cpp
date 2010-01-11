
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "SerialData.h"



SerialData::SerialData()
{
}

SerialData::SerialData(bool ADDR, bool CLK, bool DIN, bool EN)
{
	bits.ADDR = ADDR;
	bits.CLK = CLK;
	bits.DIN = DIN;
	bits.EN = EN;

}

SerialData::~SerialData()
{
}


uInt32 SerialData::getValue()
{
	return bits.ADDR + 2*bits.CLK + 4*bits.DIN + 8*bits.EN;	//using GradEmbed convention
//	return bits.EN + 2*bits.CLK + 4*bits.DIN + 8*bits.ADDR;	//using Ted's convention
}

uInt32 SerialData::getValue_ECDA()
{
	return bits.EN + 2*bits.CLK + 4*bits.DIN + 8*bits.ADDR;	//using Ted's convention
}

uInt32 SerialData::getValue_EDCA()
{
	return bits.EN + 2*bits.DIN + 4*bits.CLK + 8*bits.ADDR;	//using Dave's convention
}
