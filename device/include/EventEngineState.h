#ifndef STI_TIMINGENGINE_EVENTENGINESTATE_H
#define STI_TIMINGENGINE_EVENTENGINESTATE_H

namespace STI
{
namespace TimingEngine
{

enum EventEngineState {
	Empty, Parsing, Parsed, Clearing,
	RequestingLoad, PreparingToLoad, Loading, Loaded,
	RequestingPlay, PreparingToPlay, WaitingForTrigger, Triggered, Playing, 
	Pausing, Paused, PreparingToResume, RequestingPublish, Publishing, Stopping, EngineStateUnknown
};

}
}

#endif
