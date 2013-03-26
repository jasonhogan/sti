
#include "EventEngineStateMachine.h"

#include <set>

using namespace STI::TimingEngine;
using namespace std;

EventEngineStateMachine::EventEngineStateMachine()
{
	currentState = Empty;
	lastStaticState = Empty;

	defineStaticStates();
}


bool EventEngineStateMachine::inState(EventEngineState state) const
{
	boost::shared_lock< boost::shared_mutex > lock(stateMutex);
	return currentState == state;
}

EventEngineState EventEngineStateMachine::getState() const
{
	boost::shared_lock< boost::shared_mutex > lock(stateMutex);
	return currentState;
}

bool EventEngineStateMachine::setState(EventEngineState newState)
{
	boost::unique_lock< boost::shared_mutex > lock(stateMutex);
	if( isAllowedTransition(currentState, newState) ) {
		currentState = newState;

		if( isStaticState(currentState) )
			lastStaticState = currentState;		//returns to this state upon call to leaveState(...)

		return true;
	}
	return false;
}

bool EventEngineStateMachine::leaveState(EventEngineState currentState)
{
	boost::unique_lock< boost::shared_mutex > lock(stateMutex);

	if( !isStaticState(currentState) 
		&& isAllowedTransition(currentState, lastStaticState) ) 
	{
		currentState = lastStaticState;
		return true;
	}
	return false;
}

bool EventEngineStateMachine::isStaticState(EventEngineState state)
{
	return staticStates.count(state) == 1;
}


bool EventEngineStateMachine::isAllowedTransition(EventEngineState beginState, EventEngineState endState)
{
	//Empty, Parsing, Parsed, Clearing
	//PreparingToLoad, Loading, Loaded, 
	//RequestingPlay, PreparingToPlay, WaitingForTrigger, Triggered, Playing, 
	//Pausing, Paused, Resuming, Stopping

	if (beginState == endState)
		return true;

	bool allowedTransition = false;

	switch(beginState) {
	case Empty:
		allowedTransition = 
			(endState == Parsing) ||
			(endState == Stopping);
		break;
	case Parsing:
		allowedTransition = 
			(endState == Parsed) ||
			(endState == Stopping);
		break;
	case Parsed:
		allowedTransition = 
			(endState == PreparingToLoad) ||
			(endState == Clearing) ||
			(endState == Stopping);
		break;
	case Clearing:
		allowedTransition = 
			(endState == Empty);
		break;
	case PreparingToLoad:
		allowedTransition = 
			(endState == Loading) ||
			(endState == Stopping);
		break;
	case Loading:
		allowedTransition = 
			(endState == Loaded) ||
			(endState == Stopping);
		break;
	case Loaded:
		allowedTransition = 
			(endState == RequestingPlay) ||
			(endState == Parsed) ||
			(endState == Clearing) ||
			(endState == Stopping);
		break;
	case RequestingPlay:
		allowedTransition = 
			(endState == PreparingToPlay) ||
			(endState == Stopping);
		break;
	case PreparingToPlay:
		allowedTransition = 
			(endState == WaitingForTrigger) ||
			(endState == Stopping);
		break;
	case WaitingForTrigger:
		allowedTransition = 
			(endState == Triggered) ||
			(endState == Stopping);
		break;
	case Triggered:
		allowedTransition = 
			(endState == Playing) ||
			(endState == Stopping);
		break;
	case Playing:
		allowedTransition = 
			(endState == Pausing) ||
			(endState == WaitingForTrigger) ||
			(endState == Stopping);
		break;
	case Pausing:
		allowedTransition = 
			(endState == Paused) ||
			(endState == Stopping);
		break;
	case Paused:
		allowedTransition = 
			(endState == Resuming) ||
			(endState == Stopping);
		break;
	case Resuming:
		allowedTransition = 
			(endState == Playing) ||
			(endState == Stopping);
		break;
	case Stopping:
		allowedTransition = 
			(endState == Empty) ||
			(endState == Parsed) ||
			(endState == Loaded);
		break;
	default:
		allowedTransition = false;
		break;
	}

	return allowedTransition;
}

void EventEngineStateMachine::defineStaticStates()
{
	staticStates.insert(Empty);
	staticStates.insert(Parsed);
	staticStates.insert(Loaded);
}

