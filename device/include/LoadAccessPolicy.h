#ifndef STI_TIMINGENGINE_LOADACCESSPOLICY_H
#define STI_TIMINGENGINE_LOADACCESSPOLICY_H

namespace STI
{
namespace TimingEngine
{

class LoadAccessPolicy
{
public:

	enum LoadPolicyType { Exclusive, Shared, Mixed };
	
	struct SharedAccessPair
	{
		EngineID id1;
		EngineID id2;
	};

	LoadPolicy() : policyType = Exclusive { sharedPairs.clear(); }
	LoadPolicy(LoadPolicyType type) : policyType = type { sharedPairs.clear(); }

	bool isSharedAccessAllowed(EngineID& id1, EngineID& id2)
	{
		if( policyType == Exclusive )
			return false;
		else if( policyType == Shared )
			return true;
		else {
			return (sharedPairs.count( SharedAccessPair(id1, id2) ) == 1);
		}
	}

	void addSharedAccess(EngineID& id1, EngineID& id2)
	{
		policyType = Mixed;

		sharedPairs.insert( SharedAccessPair(id1, id2) );
	}

	void removeSharedAccess(EngineID& id1, EngineID& id2)
	{
		policyType = Mixed;

		it = sharedPairs.find( SharedAccessPair(id1, id2) );
		sharedList.erase(it);
	}

private:

	LoadPolicyType policyType;
	std::set<SharedAccessPair> sharedPairs;
};

}
}

#endif
