

#include "FPGALoadAccessPolicy.h"
#include "FPGA_RAM_Block.h"
#include <math.h>

using STI::FPGA::FPGALoadAccessPolicy;
using STI::TimingEngine::EngineID;
using STI::FPGA::FPGA_RAM_Block_ptr;

FPGALoadAccessPolicy::FPGALoadAccessPolicy(const FPGA_RAM_Block_ptr& fullModuleRAMBlock)
: fullRAMBlock(fullModuleRAMBlock)
{
}

bool FPGALoadAccessPolicy::loadWhilePlayingAllowed(const EngineID& loader, const EngineID& player) const
{
	return loadWhileLoadedAllowed(loader, player);
}

bool FPGALoadAccessPolicy::loadWhileLoadedAllowed(const EngineID& loader, const EngineID& loaded) const
{
	boost::shared_lock< boost::shared_mutex > readLock(engineMutex);

	RAMSubBlockMap::const_iterator subBlock1 = ramSubBlocks.find(loader);
	RAMSubBlockMap::const_iterator subBlock2 = ramSubBlocks.find(loaded);

	if(subBlock1 == ramSubBlocks.end() || subBlock2 == ramSubBlocks.end())
		return false;	//Be conservative and prevent simultaneous loading in case where a subblock is not found.

	//As long as the RAM block do not overlap, they can be loaded at the same time.
	return !blocksOverlap(subBlock1->second.ramBlock, subBlock2->second.ramBlock);
}

bool FPGALoadAccessPolicy::blocksOverlap(const FPGA_RAM_Block_ptr& block1, const FPGA_RAM_Block_ptr& block2) const
{
	uInt32 low1 = block1->getStartAddress();
	uInt32 high1 = block1->getEndAddress();
	uInt32 low2 = block2->getStartAddress();
	uInt32 high2 = block2->getEndAddress();

	if(low1 < low2 && high1 < low2) {
		//endpoint ordering pattern: 1122
		return false;	//blocks do not overlap
	}
	if(low2 < low1 && high2 < low1) {
		//endpoint ordering pattern: 2211
		return false;	//blocks do not overlap
	}

	return true;	//All other ordering patterns indicate overlap.
}


void FPGALoadAccessPolicy::addEngine(const EngineID& engineID, const FPGA_RAM_Block_ptr& ramBlock, bool resizable, bool overlappable)
{
	boost::unique_lock< boost::shared_mutex > writeLock(engineMutex);

	RAMSubBlock ramSubBlock;
	ramSubBlock.ramBlock = ramBlock;
	ramSubBlock.resizable = resizable;
	ramSubBlock.overlappable = overlappable;

	ramSubBlock.blockNumber = ramSubBlocks.size();	//Assign engine to a new block at the end of the list.

	if(!resizable) {
		//Put fixed blocks at the beginning of the list.
		ramSubBlock.blockNumber = 0;
	}

	ramSubBlocks.insert(RAMSubBlockPair(engineID, ramSubBlock));
	
	if(!resizable) {
		reindexSubBlockNumbers();
	}

	reallocateSubBlocks();
}

void FPGALoadAccessPolicy::removeEngine(const EngineID& engineID)
{
	boost::unique_lock< boost::shared_mutex > writeLock(engineMutex);

	removeEngineFromLoadRecord(engineID);

	RAMSubBlockMap::iterator it = ramSubBlocks.find(engineID);

	if(it != ramSubBlocks.end()) {
		//found; delete it and reindex RAM block numbers.

	//	unsigned erasedBlockNumber = it->second.blockNumber;

		ramSubBlocks.erase(it);

		//Now reindex remaining RAM block numbers.
		reindexSubBlockNumbers();

		reallocateSubBlocks();
	}
}




void FPGALoadAccessPolicy::reindexSubBlockNumbers()
{
	//Put the non-overlappable blocks at the beginning for more efficient resizing later.
	int nonOverlappableNum = 0;
	
	for(RAMSubBlockMap::iterator it = ramSubBlocks.begin(); it != ramSubBlocks.end(); ++it) {
		//number the non-overlappable sub blocks first
		if(!it->second.overlappable) {
			it->second.blockNumber = nonOverlappableNum;
			nonOverlappableNum++;
		}
		else {
			it->second.blockNumber = -1;	//tagged as overlappable
		}
	}
	
	int i = nonOverlappableNum;
	
	for(RAMSubBlockMap::iterator it = ramSubBlocks.begin(); it != ramSubBlocks.end(); ++it) {
		//now do the overlappable sub blocks (all remaining tagged blocks)
		if(it->second.blockNumber == -1) {
			it->second.blockNumber = i;
			i++;
		}
	}
}


void FPGALoadAccessPolicy::reallocateSubBlocks()
{
	//This divides up the total available RAM (for this device) between the engines.
	//The RAM is uniformly distributed, unless one of the sub blocks is of fixed size.
	boost::unique_lock< boost::shared_mutex > writeLock(engineMutex);

	if(ramSubBlocks.size() == 0)
		return;

	uInt32 wordsPerSubBlock = static_cast<uInt32>(
		floor(
		(1.0*fullRAMBlock->getSizeInWords()) / (1.0*ramSubBlocks.size())
		));

//	int targetBlock = 0;
	int subBlocksRemaining = ramSubBlocks.size();
	
	RAMSubBlockMap::iterator last;
	RAMSubBlockMap::iterator next;

	if(!findTargetBlockNumber(0, last)) {
		//Error! Block 0 must exist.  Reindex.
		reindexSubBlockNumbers();
		last = ramSubBlocks.begin();
	}

	uInt32 oldSize = last->second.ramBlock->getSizeInWords();

	last->second.ramBlock->setStartWord(fullRAMBlock->getStartWord());
	if(last->second.resizable) {
		last->second.ramBlock->setRAM_Block_Size(wordsPerSubBlock);	//sets the end word appropriately
	}
	else {
		last->second.ramBlock->setRAM_Block_Size(oldSize);	//sets the end word appropriately
	}

	subBlocksRemaining--;
	
	while(getNextSubBlock(last, next) && subBlocksRemaining > 0)
	{
		oldSize = next->second.ramBlock->getSizeInWords();
		next->second.ramBlock->setStartWord(last->second.ramBlock->getEndWord() + 1);

		if(next->second.resizable) {
			wordsPerSubBlock = static_cast<uInt32>(
				floor(
				(1.0*(fullRAMBlock->getEndWord() - last->second.ramBlock->getEndWord() + 1)) / (1.0*subBlocksRemaining)
				));

			next->second.ramBlock->setRAM_Block_Size(wordsPerSubBlock);
		}
		else {
			next->second.ramBlock->setRAM_Block_Size(oldSize);
		}

		last = next;
		subBlocksRemaining--;
	}

	if(last->second.resizable) {
		last->second.ramBlock->setEndWord(fullRAMBlock->getEndWord());	//take up any slack in the last sub block
	}

	//Save the new start words as the default.
	for(RAMSubBlockMap::iterator it = ramSubBlocks.begin(); it != ramSubBlocks.end(); ++it) {
		it->second.defaultStartWord = it->second.ramBlock->getStartWord();
	}

}

bool FPGALoadAccessPolicy::getNextSubBlock(RAMSubBlockMap::iterator thisSubBlock, 
										   RAMSubBlockMap::iterator& nextSubBlock)
{
	if(thisSubBlock == ramSubBlocks.end())
		return false;

	int targetBlockNumber = (thisSubBlock->second.blockNumber + 1);

	return findTargetBlockNumber(targetBlockNumber, nextSubBlock);

	//RAMSubBlockMap::iterator next = ++thisSubBlock;

	//if(next != ramSubBlocks.end()) {
	//	//Next iterator is valid; it should contain the next block number if the map is ordered correctly.
	//	if(next->second.blockNumber == targetBlockNumber ) {
	//		//Next iterator points to the next sub block (this is expected).
	//		nextSubBlock = next;
	//		return true;
	//	}
	//}
	//else {
	//	//Next iterator is not valid; probably there are no sub blocks higher than thisSubBlock. Check this:
	//	if(targetBlockNumber == ramSubBlocks.size())
	//		return false;	//thisSubBlock is the last sub block.
	//}

	////This should not happen usually. In case, do a brute force search for the next block number.
	//return findTargetBlockNumber(targetBlockNumber, nextSubBlock);
}


bool FPGALoadAccessPolicy::findTargetBlockNumber(int targetBlockNumber, RAMSubBlockMap::iterator& targetSubBlock)
{
	for(RAMSubBlockMap::iterator it = ramSubBlocks.begin(); it != ramSubBlocks.end(); ++it) {
		if(it->second.blockNumber == targetBlockNumber) {
			targetSubBlock = it;
			return true;
		}
	}
	return false;	//Next block not found 
}

bool FPGALoadAccessPolicy::getPreviousSubBlock(RAMSubBlockMap::iterator thisSubBlock, 
											   RAMSubBlockMap::iterator& previousSubBlock)
{
	if(thisSubBlock == ramSubBlocks.end())
		return false;
	
	int targetBlockNumber = (thisSubBlock->second.blockNumber - 1);
	
	return findTargetBlockNumber(targetBlockNumber, previousSubBlock);

	//if(thisSubBlock != ramSubBlocks.begin()) {
	//	RAMSubBlockMap::iterator prev = --thisSubBlock;
	//	if( prev->second.blockNumber == targetBlockNumber ) {
	//		//Previous iterator points to the previous sub block (this is expected).
	//		previousSubBlock = prev;
	//		return true;
	//	}
	//}
	//else {
	//	//Previous iterator is not valid; probably there are no sub blocks lower than thisSubBlock. Check this:
	//	if(targetBlockNumber < 0)
	//		return false;	//thisSubBlock is the smallest sub block.
	//}

	////This should not happen usually. In case, do a brute force search for the next block number.
	//return findTargetBlockNumber(targetBlockNumber, previousSubBlock);
}


bool FPGALoadAccessPolicy::getNextNonOverlappingSubBlock(const RAMSubBlockMap::iterator& initialSubBlock, RAMSubBlockMap::iterator& nextSubBlock)
{
	bool found = false;
	RAMSubBlockMap::iterator next = initialSubBlock;

	while(!found && getNextSubBlock(next, next))	//keep incrementing next as long as the next sub block exists
	{
		found = (next->second.ramBlock->getStartAddress() > initialSubBlock->second.ramBlock->getEndAddress());
	}
	if(found) {
		nextSubBlock = next;
	}
	return found;
}

bool FPGALoadAccessPolicy::getPreviousNonOverlappingSubBlock(const RAMSubBlockMap::iterator& initialSubBlock, RAMSubBlockMap::iterator& previousSubBlock)
{
	bool found = false;
	RAMSubBlockMap::iterator previous = initialSubBlock;

	while(!found && getPreviousSubBlock(previous, previous))	//keep decrementing previous as long as the previous sub block exists
	{
		found = (previous->second.ramBlock->getEndAddress() < initialSubBlock->second.ramBlock->getStartAddress());
	}
	if(found) {
		previousSubBlock = previous;
	}
	return found;
}

void FPGALoadAccessPolicy::setSubBlockSize(const EngineID& engineID, uInt32 targetBlockSizeInWords)
{
	boost::unique_lock< boost::shared_mutex > writeLock(engineMutex);

	RAMSubBlockMap::iterator it = ramSubBlocks.find(engineID);

	if(it == ramSubBlocks.end())	//engine not found
		return;

	if(!it->second.resizable || it->second.ramBlock->getSizeInWords() == targetBlockSizeInWords)
		return;

	//Attempt to resize sub block to the targetBlockSizeInWords.
	
//	unsigned blockNumber = it->second.blockNumber;
	FPGA_RAM_Block_ptr subRAMBlock = it->second.ramBlock;
	
	RAMSubBlockMap::iterator nextSubBlock;
	RAMSubBlockMap::iterator previousSubBlock;

	//Begin by fitting this sub block between the adjacent sub blocks.
	//This is an attempt to minimize overlap.
	//Find the boundaries:
	uInt32 startWord = fullRAMBlock->getStartWord();
	uInt32 endWord = fullRAMBlock->getEndWord();

	if(getNextSubBlock(it, nextSubBlock)) {
		endWord = nextSubBlock->second.ramBlock->getStartWord();
	}
	if(getPreviousSubBlock(it, previousSubBlock)) {
		startWord = previousSubBlock->second.ramBlock->getEndWord() + 1;
	}

	//Resize to eliminate overlap with adjacent blocks (best case).
	subRAMBlock->setStartWord(startWord);
	subRAMBlock->setEndWord(endWord);

	//Check to see if it can still be shrunk.
	if(subRAMBlock->getSizeInWords() >= targetBlockSizeInWords) {
		//The sub block is larger and can be reduced in size.
		subRAMBlock->setRAM_Block_Size(targetBlockSizeInWords);	//resizes by moving end word
		return; //no need to proceed; the sub block is the correct size and does not overlap with adjacent sub blocks
	}

	//The sub block is too small. It must be expanded and may overlap with nearby sub blocks.

	bool success = true;
	bool resizing = true;
	bool higherAllowed;
	bool lowerAllowed;

	//Look at nearby sub blocks.
	//First try to expand the target block towards nearby block without overlapping (if there is room).
	//If more room is neaded, attempt to overlap with nearby blocks until enough room is found.
	//Given a choice of sub block, overlap with the one that has not been loaded recently.
	while(resizing)
	{
		higherAllowed = false;
		lowerAllowed = false;
		success = false;

		//Explore expanding toward higher block numbers
		if(!success && getNextNonOverlappingSubBlock(it, nextSubBlock)) {
			if(subRAMBlock->getEndWord() < nextSubBlock->second.ramBlock->getStartWord() - 1) {
				//Possible to grow towards the next sub block without overlapping
				subRAMBlock->setEndWord(nextSubBlock->second.ramBlock->getStartWord() - 1);
				success = true;
			}
			higherAllowed = nextSubBlock->second.overlappable;
		}

		//Explore expanding toward lower block numbers
		if(!success && getPreviousNonOverlappingSubBlock(it, previousSubBlock)) {
			if(subRAMBlock->getStartWord() > previousSubBlock->second.ramBlock->getEndWord() + 1) {
				//Possible to grow towards the previous sub block without overlapping
				subRAMBlock->setStartWord(previousSubBlock->second.ramBlock->getEndWord() + 1);
				success = true;
			}
			lowerAllowed = previousSubBlock->second.overlappable;
		}

		if(!success) {
			//Attempt to overlap with nearby sub blocks.
			//Pick an allowed side to expand towards.
			if(higherAllowed && lowerAllowed) {
				//Avoid the direction with the more recently loaded block.
				if(isLessRecentlyLoaded(nextSubBlock->first, previousSubBlock->first)) {
					//nextSubBlock's engine was loaded less recently; overlap with it
					subRAMBlock->setEndWord(nextSubBlock->second.ramBlock->getEndWord());	//sub blocks now overlap
				}
				else {
					//previousSubBlock's engine was loaded less recently; overlap with it
					subRAMBlock->setStartWord(previousSubBlock->second.ramBlock->getStartWord());	//sub blocks now overlap
				}
				success = true;
			}
			else if(higherAllowed) {
				subRAMBlock->setEndWord(nextSubBlock->second.ramBlock->getEndWord());	//sub blocks now overlap
				success = true;
			}
			else if(lowerAllowed) {
				subRAMBlock->setStartWord(previousSubBlock->second.ramBlock->getStartWord());	//sub blocks now overlap
				success = true;
			}
		}

		resizing = success && (subRAMBlock->getSizeInWords() < targetBlockSizeInWords);
	}
}

bool FPGALoadAccessPolicy::isLessRecentlyLoaded(const EngineID& lessRecent, const EngineID& moreRecent)
{
	//The larger the position in the loadRecord list, the more recent the engine was loaded.
	int lessRecentPos = -2;
	int moreRecentPos = -1;

	int pos = 0;
	for(std::vector<EngineID>::iterator it = loadRecord.begin(); it != loadRecord.end(); ++it) {
		if((*it) == lessRecent) {
			lessRecentPos = pos;
		}
		if((*it) == moreRecent) {
			moreRecentPos = pos;
		}
		pos++;
	}

	return (lessRecentPos <= moreRecentPos);
}

void FPGALoadAccessPolicy::markBlockAsLoaded(const EngineID& engineID)
{
	boost::unique_lock< boost::shared_mutex > writeLock(engineMutex);

	//If this engine is already in the loadRecord, find it and remove it (so it can be put at the top of the list).
	removeEngineFromLoadRecord(engineID);

	//By convention, the last element in the list is the most recently loaded.
	loadRecord.push_back(engineID);	//this engine is now at the top of the list (most recently loaded)
}


void FPGALoadAccessPolicy::removeEngineFromLoadRecord(const EngineID& engineID)
{
	//Usage: Lock the engineMutex before calling this to ensure synchronization.

	using STI::TimingEngine::EngineID;

	for(std::vector<EngineID>::iterator it = loadRecord.begin(); it != loadRecord.end(); ++it) {
		if(*it == engineID) {
			loadRecord.erase(it);
			break;
		}
	}
}

