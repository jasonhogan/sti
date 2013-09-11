
#ifndef STI_FPGA_FPGADEVICEEVENTENGINE_H
#define STI_FPGA_FPGADEVICEEVENTENGINE_H

#include "DeviceEventEngine.h"
#include "FPGA_RAM_Block.h"
#include "FPGATypes.h"

namespace STI
{
namespace FPGA
{

struct FPGADeviceEventEngineSetup
{
	short wordsPerEvent;
};

class FPGADeviceEventEngine : public STI::TimingEngine::DeviceEventEngine
{
public:

	FPGADeviceEventEngine(const STI::TimingEngine::EngineID& engineID, STI::FPGA::FPGA_Device& fpgaDevice, 
		const FPGADeviceEventEngineSetup& engineSetup, const FPGA_RAM_Block_ptr& ramBlock);

	void preParse();

	void preLoad();
//	void postLoad();

	void prePlay(const STI::TimingEngine::EngineTimestamp& parseTimeStamp, 
		const STI::TimingEngine::EngineTimestamp& playTimeStamp, 
		const STI::TimingEngine::PlayOptions_ptr& playOptions,
		const STI::TimingEngine::DocumentationOptions_ptr& docOptions, const STI::TimingEngine::EngineCallbackHandler_ptr& callBack);

	void postStop();

protected:

//	void writeRAM_Parameters();
//	short wordsPerEvent();

	STI::TimingEngine::EngineID localEngineID;

private:

	void waitUntilEvent(const STI::TimingEngine::SynchronousEvent& evt, STI::TimingEngine::EventEngineState stateCondition);

	uInt32 getCurrentEventNumber();
	void waitForEvent(unsigned eventNumber);
//	void stopWaiting();
//	void resetStopFlag();
	
//	bool stopWaiting_l;
//	mutable boost::shared_mutex waitForEventMutex;
	
	FPGA_Device* _device;
	FPGADeviceEventEngineSetup setup;
	FPGA_RAM_Block_ptr engineRamBlock;

	uInt32 numberOfEvents;
	

//	short wordsPerEvent_l;

};

}
}

#endif
