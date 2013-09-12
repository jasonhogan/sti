#ifndef STI_UTILS_SYNCHRONIZEDMAP
#define STI_UTILS_SYNCHRONIZEDMAP

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <set>

namespace STI
{
namespace Utils
{

template<class Key>
class SynchronizedMapPolicy
{
public:
	virtual ~SynchronizedMapPolicy() {}

	virtual bool include(const Key& key) const = 0;
	virtual bool replace(const Key& oldKey, const Key& newKey) const = 0;
};

template<class Key>
class DefaultSynchronizedMapPolicy : public SynchronizedMapPolicy<Key>
{
	bool include(const Key& key) const { return true; }
	bool replace(const Key& oldKey, const Key& newKey) const { return (oldKey == newKey); }
};


//Const Key is not allowed because std::set<Key> cannot have const Key (must be copyable in STL)

template<class Key, class T>
class SynchronizedMap
{
public:

//	typedef boost::ptr_map<Key, T> TMap;
	typedef std::map<Key, T> TMap;
	typedef SynchronizedMapPolicy<Key> KeyPolicy;
	typedef boost::shared_ptr<KeyPolicy> KeyPolicy_ptr;

	SynchronizedMap(KeyPolicy_ptr Policy = defaultPolicy) : policy(Policy) { }
	virtual ~SynchronizedMap() { }

	void setPolicy(KeyPolicy_ptr Policy);

	bool contains(const Key& key) const;
	bool include(const Key& key) const;
	bool get(const Key& key, T& item) const;
	void getKeys(std::set<Key>& keys) const;
	unsigned size() const;

	bool add(const Key& key, T item);
	bool remove(const Key& key);
	
	void clear();

private:

	static boost::shared_ptr<DefaultSynchronizedMapPolicy<Key> > defaultPolicy;
	
	KeyPolicy_ptr policy;
	TMap items;

	mutable boost::shared_mutex mapMutex;

};

template<class Key, class T>
boost::shared_ptr<DefaultSynchronizedMapPolicy<Key> > SynchronizedMap<Key, T>::defaultPolicy = boost::shared_ptr<DefaultSynchronizedMapPolicy<Key> >(new DefaultSynchronizedMapPolicy<Key>());

} // UTILS
} // STI


//Implementation

template<class Key, class T>
void STI::Utils::SynchronizedMap<Key, T>::setPolicy(KeyPolicy_ptr Policy)
{
	boost::unique_lock< boost::shared_mutex > writeLock(mapMutex);

	policy = Policy;
}

template<class Key, class T>
bool STI::Utils::SynchronizedMap<Key, T>::contains(const Key& key) const
{
	boost::shared_lock< boost::shared_mutex > readLock(mapMutex);
	return (items.find(key) != items.end());
}

template<class Key, class T>
bool STI::Utils::SynchronizedMap<Key, T>::include(const Key& key) const
{
	boost::shared_lock< boost::shared_mutex > readLock(mapMutex);
	if(policy != NULL)
		return policy->include(key);
	return false;
}

template<class Key, class T>
bool STI::Utils::SynchronizedMap<Key, T>::add(const Key& key, T item)
{
	boost::upgrade_lock< boost::shared_mutex > readLock(mapMutex);

	if(policy == NULL)
		return false;

	if(policy->include(key))
	{
		if(contains(key))
		{
			if( policy->replace((items.find(key)->first), key) )
			{
				boost::upgrade_to_unique_lock< boost::shared_mutex > writeLock(readLock);

				items.erase( items.find(key) );
				items.insert( std::make_pair(key, item) );
			}
		}
		else
		{
			boost::upgrade_to_unique_lock< boost::shared_mutex > writeLock(readLock);

			items.insert( std::make_pair(key, item) );
		}
		return contains(key);
	}
	return false;
}

template<class Key, class T>
bool STI::Utils::SynchronizedMap<Key, T>::remove(const Key& key)
{
	boost::upgrade_lock< boost::shared_mutex > readLock(mapMutex);

	if(contains(key))
	{
		boost::upgrade_to_unique_lock< boost::shared_mutex > writeLock(readLock);
			
		items.erase(items.find(key));
	}
	else
	{
		return true;
	}
	return !contains(key);
}

template<class Key, class T>
bool STI::Utils::SynchronizedMap<Key, T>::get(const Key& key, T& item) const
{
	boost::shared_lock< boost::shared_mutex > readLock(mapMutex);

//using STI::Utils::SynchronizedMap<Key, T>::TMap;

	typename TMap::const_iterator it = items.find(key);

//std::map<Key, T>::const_iterator it = items.find(key);

	if(it != items.end())
	{
		item = it->second; 
		return true;
	}
	return false;
}

template<class Key, class T>
void STI::Utils::SynchronizedMap<Key, T>::getKeys(std::set<Key>& keys) const
{
	keys.clear();

	boost::shared_lock< boost::shared_mutex > readLock(mapMutex);

	for(typename TMap::const_iterator it = items.begin(); it != items.end(); it++)
	{
		keys.insert(it->first);
	}
}

template<class Key, class T>
unsigned STI::Utils::SynchronizedMap<Key, T>::size() const
{
	boost::shared_lock< boost::shared_mutex > readLock(mapMutex);
	return items.size();
}

template<class Key, class T>
void STI::Utils::SynchronizedMap<Key, T>::clear()
{
	boost::unique_lock< boost::shared_mutex > writeLock(mapMutex);
	items.clear();
}


#endif
