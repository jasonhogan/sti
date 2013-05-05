#ifndef STI_TIMINGENGINE_LOADACCESSPOLICY_H
#define STI_TIMINGENGINE_LOADACCESSPOLICY_H

#include "EngineID.h"

namespace STI
{
namespace TimingEngine
{

	//If you can load while its playing, then assume it can stay loaded when you load.
	//If you can load while its loaded, then it still might need to be unloaded when you load.
//bool sharedLoadingAllowed(const EngineID& loader, const EngineID& loaded)


class LoadAccessPolicy
{
public:
	
//	enum LoadPolicyType { Exclusive, Shared, Mixed };
	virtual bool loadWhilePlayingAllowed(const EngineID& loader, const EngineID& player) const = 0;	//if true, overrides isSharedLoadingAllowed for shrard loading decisions
	virtual bool loadWhileLoadedAllowed(const EngineID& loader, const EngineID& loaded) const = 0;
};


//	
//	struct SharedAccessPair
//	{
//		SharedAccessPair(EngineID ID1, EngineID ID2) : id1(ID1), id2(ID2) {}
//		EngineID id1;
//		EngineID id2;
//	};
//
//	LoadAccessPolicy() : policyType(Exclusive) { sharedPairs.clear(); }
//	LoadAccessPolicy(LoadPolicyType type) : policyType(type) { sharedPairs.clear(); }
//
//	bool isSharedAccessAllowed(EngineID& id1, EngineID& id2)
//	{
//		if( policyType == Exclusive )
//			return false;
//		else if( policyType == Shared )
//			return true;
//		else {
//			return (sharedPairs.count( SharedAccessPair(id1, id2) ) == 1);
//		}
//	}
//
//	void addSharedAccess(EngineID& id1, EngineID& id2)
//	{
//		policyType = Mixed;
//
//		sharedPairs.insert( SharedAccessPair(id1, id2) );
//	}
//
//	void removeSharedAccess(EngineID& id1, EngineID& id2)
//	{
//		policyType = Mixed;
//
////		it = sharedPairs.find( SharedAccessPair(id1, id2) );
////		sharedList.erase(it);
//	}
//
//private:
//
//	LoadPolicyType policyType;
//	std::set<SharedAccessPair> sharedPairs;

}
}

#endif
