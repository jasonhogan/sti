#ifndef STI_TIMINGENGINE_EVENTENGINESTATEMACHINE_H
#define STI_TIMINGENGINE_EVENTENGINESTATEMACHINE_H

#include "EventEngineState.h"

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <set>

namespace STI
{
namespace TimingEngine
{

class EventEngineStateMachine
{
public:

	EventEngineStateMachine();
	virtual ~EventEngineStateMachine();

	bool inState(EventEngineState state) const;
	EventEngineState getState() const;

	bool setState(EventEngineState newState);
	bool leaveState(EventEngineState oldState);

	virtual bool isAllowedTransition(EventEngineState beginState, EventEngineState endState);
	virtual void defineStaticStates();
	bool isStaticState(EventEngineState state);

protected:

	EventEngineState currentState;
	EventEngineState lastStaticState;

	std::set<EventEngineState> staticStates;

	mutable boost::shared_mutex stateMutex;

};

}
}

#endif
