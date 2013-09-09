

#include "FPGA_RAM_EngineAllocator.h"


using STI::FPGA::FPGA_RAM_EngineAllocator;


FPGA_RAM_EngineAllocator::FPGA_RAM_EngineAllocator(const FPGA_RAM_Block_ptr& fullRAMBlock)
{
}


void FPGA_RAM_EngineAllocator::addEngine(const EngineID& engineID)
{
	boost::unique_lock< boost::shared_mutex > writeLock(engineMutex);
}

void FPGA_RAM_EngineAllocator::removeEngine(const EngineID& engineID)
{
	boost::unique_lock< boost::shared_mutex > writeLock(engineMutex);
}

void FPGA_RAM_EngineAllocator::refresh()
{
	boost::shared_lock< boost::shared_mutex > readLock(engineMutex);
}

void FPGA_RAM_EngineAllocator::setSubBlockSize(const EngineID& engineID)
{
	boost::shared_lock< boost::shared_mutex > readLock(engineMutex);

	//explore expanding up

	//explore expanding down

	//avoid the direction with the more recently loaded block
}

void FPGA_RAM_EngineAllocator::markBlockAsLoaded(const EngineID& engineID)
{
	boost::unique_lock< boost::shared_mutex > writeLock(engineMutex);

	using STI::TimingEngine::EngineID;

	//If this engine is already in the loadRecord, find it and remove it (so it can be put at the top of the list).
	for(std::vector<EngineID>::iterator it = loadRecord.begin(); it != loadRecord.end(); ++it) {
		if(*it == engineID) {
			loadRecord.erase(it);
			break;
		}
	}
	
	//By convention, the last element in the list is the most recently loaded.
	loadRecord.push_back(engineID);	//this engine is now at the top of the list (most recently loaded)
}
