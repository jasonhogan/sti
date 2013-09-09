
#ifndef STI_FPGA_FPGA_RAM_ENGINEALLOCATOR_H
#define STI_FPGA_FPGA_RAM_ENGINEALLOCATOR_H

#include "FPGA_RAM_Block.h"
#include "EngineID.h"

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

namespace STI
{
namespace FPGA
{

class FPGA_RAM_EngineAllocator 
{
public:

	FPGA_RAM_EngineAllocator(const FPGA_RAM_Block_ptr& fullRAMBlock, const FPGALoadAccessPolicy_ptr& fpgaLoadPolicy);

	void addEngine(const EngineID& engineID, const FPGA_RAM_Block_ptr& ramBlock, bool resizeable = true);
	void removeEngine(const EngineID& engineID);

	void refresh();

	void setSubBlockSize(const EngineID& engineID);

	void markBlockAsLoaded(const EngineID& engineID);

private:

	std::vector<STI::TimingEngine::EngineID> loadRecord;
	std::map<STI::TimingEngine::EngineID, unsigned> blockNumbers;
	std::map<STI::TimingEngine::EngineID, FPGA_RAM_Block_ptr> blockNumbers;

	FPGALoadAccessPolicy_ptr loadPolicy;

	mutable boost::shared_mutex engineMutex;

};

}
}

#endif

