#ifndef STI_FPGA_FPGALOADACCESSPOLICY_H
#define STI_FPGA_FPGALOADACCESSPOLICY_H

#include "FPGATypes.h"

#include "LoadAccessPolicy.h"
#include "EngineID.h"
#include <types.h>

#include <map>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

namespace STI
{
namespace FPGA
{

class FPGALoadAccessPolicy : public STI::TimingEngine::LoadAccessPolicy
{
public:

	FPGALoadAccessPolicy(const FPGA_RAM_Block_ptr& fullModuleRAMBlock);
	
	bool loadWhilePlayingAllowed(const STI::TimingEngine::EngineID& loader, const STI::TimingEngine::EngineID& player) const;
	bool loadWhileLoadedAllowed(const STI::TimingEngine::EngineID& loader, const STI::TimingEngine::EngineID& loaded) const;

	void addEngine(const STI::TimingEngine::EngineID& engineID, const FPGA_RAM_Block_ptr& ramBlock, bool resizable = true, bool overlappable = true);
	void removeEngine(const STI::TimingEngine::EngineID& engineID);

	void reallocateSubBlocks();

	void setSubBlockSize(const STI::TimingEngine::EngineID& engineID, uInt32 targetBlockSizeInWords);

	void markBlockAsLoaded(const STI::TimingEngine::EngineID& engineID);

private:

	void reindexSubBlockNumbers();

	void removeEngineFromLoadRecord(const STI::TimingEngine::EngineID& engineID);
	bool isLessRecentlyLoaded(const STI::TimingEngine::EngineID& lessRecent, const STI::TimingEngine::EngineID& moreRecent);
	
	bool blocksOverlap(const FPGA_RAM_Block_ptr& block1, const FPGA_RAM_Block_ptr& block2) const;

	class RAMSubBlock
	{
	public:
		bool operator< (const RAMSubBlock &rhs) const 
		{
			//Ordered by blockNumber.  Resizable sub blocks go after fixed sub blocks.
			return (blockNumber < rhs.blockNumber) || ( (blockNumber == rhs.blockNumber) && (!resizable) );
		}
		int blockNumber;
		FPGA_RAM_Block_ptr ramBlock;
		bool resizable;
		bool overlappable;
		uInt32 defaultStartWord;
	};

	typedef std::map<STI::TimingEngine::EngineID, RAMSubBlock> RAMSubBlockMap;
	typedef std::pair<STI::TimingEngine::EngineID, RAMSubBlock> RAMSubBlockPair;

	RAMSubBlockMap ramSubBlocks;
	FPGA_RAM_Block_ptr fullRAMBlock;
	
	std::vector<STI::TimingEngine::EngineID> loadRecord;

	bool getNextSubBlock(RAMSubBlockMap::iterator thisSubBlock, RAMSubBlockMap::iterator& nextSubBlock);
	bool getPreviousSubBlock(RAMSubBlockMap::iterator thisSubBlock, RAMSubBlockMap::iterator& previousSubBlock);
	bool findTargetBlockNumber(int targetBlockNumber, RAMSubBlockMap::iterator& targetSubBlock);

	bool getNextNonOverlappingSubBlock(const RAMSubBlockMap::iterator& initialSubBlock, RAMSubBlockMap::iterator& nextSubBlock);
	bool getPreviousNonOverlappingSubBlock(const RAMSubBlockMap::iterator& initialSubBlock, RAMSubBlockMap::iterator& previousSubBlock);

	mutable boost::shared_mutex engineMutex;

};


}
}

#endif
