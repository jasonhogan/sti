#ifndef STI_TIMINGENGINE_NULLENGINECALLBACKHANDLER_H
#define STI_TIMINGENGINE_NULLENGINECALLBACKHANDLER_H

#include "TimingEngineTypes.h"
#include "EngineCallbackHandler.h"
#include "DeviceID.h"
#include "EngineID.h"
#include "NullEngineCallbackTarget.h"


namespace STI
{
namespace TimingEngine
{


class NullEngineCallbackHandler : public EngineCallbackHandler
{
public:
	NullEngineCallbackHandler() : 
	  EngineCallbackHandler(
		  STI::Device::DeviceID("","",0), 
		  STI::TimingEngine::EngineInstance(STI::TimingEngine::EngineID(0)),
		  STI::TimingEngine::NullEngineCallbackTarget::createNullTarget()
		  ) {}

	  static EngineCallbackHandler_ptr createNullHandler()
	  {
		  EngineCallbackHandler_ptr handler = EngineCallbackHandler_ptr(new NullEngineCallbackHandler());
		  return handler;
	  }
};


}
}

#endif

