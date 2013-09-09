#ifndef STI_FPGA_FPGATYPES_H
#define STI_FPGA_FPGATYPES_H

#include <boost/shared_ptr.hpp>

namespace STI
{	
	namespace FPGA
	{
		class FPGA_Device;
		class FPGADeviceEventEngine;

		class FPGA_RAM_Block;
		typedef boost::shared_ptr<FPGA_RAM_Block> FPGA_RAM_Block_ptr;

		class EtraxBus;
		typedef boost::shared_ptr<EtraxBus> EtraxBus_ptr;

		class FPGALoadAccessPolicy;
		typedef boost::shared_ptr<FPGALoadAccessPolicy> FPGALoadAccessPolicy_ptr;
	}

}

#endif
