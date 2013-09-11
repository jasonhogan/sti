
#include "FPGA_BitLineEvent_DeviceWrapper.h"
#include "FPGATypes.h"
#include "FPGA_Device.h"


using STI::FPGA::FPGA_BitLineEvent_DeviceWrapper;
using STI::FPGA::FPGA_Device;
using STI::FPGA::FPGA_RAM_Block_ptr;
using STI::FPGA::EtraxBus_ptr;


FPGA_BitLineEvent_DeviceWrapper::FPGA_BitLineEvent_DeviceWrapper(FPGA_Device* device) : device_f(device)
{
}

const FPGA_RAM_Block_ptr& FPGA_BitLineEvent_DeviceWrapper::getParsingEngineRAMBlock() const
{
	return device_f->parsingEngineRAMBlock;
}

const EtraxBus_ptr& FPGA_BitLineEvent_DeviceWrapper::getRAMBus() const
{
	return device_f->ramBus;
}

