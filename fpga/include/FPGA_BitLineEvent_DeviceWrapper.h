
#ifndef STI_FPGA_FPGA_BITLINEEVENT_DEVICEWRAPPER_H
#define STI_FPGA_FPGA_BITLINEEVENT_DEVICEWRAPPER_H

#include "FPGATypes.h"

namespace STI
{
namespace FPGA
{

class FPGA_BitLineEvent_DeviceWrapper
{
public:

	FPGA_BitLineEvent_DeviceWrapper(FPGA_Device* device);

	const FPGA_RAM_Block_ptr& getParsingEngineRAMBlock() const;
	const EtraxBus_ptr& getRAMBus() const;

private:

	FPGA_Device* device_f;

};

}
}

#endif

