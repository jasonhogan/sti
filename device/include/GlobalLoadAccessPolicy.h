#ifndef STI_TIMINGENGINE_GLOBALLOADACCESSPOLICY_H
#define STI_TIMINGENGINE_GLOBALLOADACCESSPOLICY_H

#include "LoadAccessPolicy.h"
#include "EngineID.h"

namespace STI
{
namespace TimingEngine
{

class GlobalLoadAccessPolicy : public LoadAccessPolicy
{
public:
	GlobalLoadAccessPolicy(bool allowLoadWhilePlaying, bool allowLoadWhileLoaded) : 
	  loadWhilePlaying(allowLoadWhilePlaying), loadWhileLoaded(allowLoadWhileLoaded) {}

	bool loadWhilePlayingAllowed(const EngineID& loader, const EngineID& player) const 
	{ return loadWhilePlaying; }
	
	bool loadWhileLoadedAllowed(const EngineID& loader, const EngineID& loaded) const 
	{ return (loadWhileLoaded || loadWhilePlaying); }

	const bool loadWhilePlaying;
	const bool loadWhileLoaded;
};


}
}

#endif
