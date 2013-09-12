#ifndef STI_TIMINGENGINE_ENGINEID_H
#define STI_TIMINGENGINE_ENGINEID_H

#include "utils.h"
#include <sstream>

namespace STI
{
namespace TimingEngine
{

class EngineID
{
public:
	EngineID(int id) : number(id) { description = STI::Utils::valueToString(id); }
	EngineID(int id, std::string name) : number(id), description(name) {}
	
	EngineID& operator=(EngineID rhs)
	{
		number = rhs.number;
		description = rhs.description;
		return *this;
	}

	bool operator<(const EngineID& rhs) const { return (this->number < rhs.number); }
	bool operator==(const EngineID& rhs) const { return (this->number == rhs.number);}

	std::string print() const
	{
		std::stringstream desc;
		desc << "'" << description << "' (ID = " << number << ")";
		return desc.str();
	}

private:
	int number;
	std::string description;
};

class EngineTimestamp
{
//	static set<EngineTimestamp> timestamps;	//to ensure uniqueness?
public:

	EngineTimestamp() : repeatID(0)
	{
	}

	EngineTimestamp& operator=(EngineTimestamp rhs)
	{
		timestamp = rhs.timestamp;
		repeatID = rhs.repeatID;
		return *this;
	}

	bool operator==(const EngineTimestamp &rhs) const { return (timestamp == rhs.timestamp) && (repeatID == rhs.repeatID); }
	bool operator<(const EngineTimestamp &rhs) const { return (timestamp < rhs.timestamp) || ((timestamp == rhs.timestamp) && (repeatID < rhs.repeatID)); }
	bool operator!=(const EngineTimestamp &rhs) const { return !((*this) == rhs); }

	double timestamp;
	unsigned repeatID;
};

class EngineInstance
{
//	static set<EngineInstance> globalInstances;	//one or the other...
public:
	EngineInstance(const EngineID& engineID) : id(engineID) {}
	EngineInstance(const EngineInstance& engineInstance) 
		: id(engineInstance.id), 
		parseTimestamp(engineInstance.parseTimestamp), 
		playTimestamp(engineInstance.playTimestamp) {}

	bool operator==(const EngineInstance &rhs) const 
	{
		return (id == rhs.id && parseTimestamp == rhs.parseTimestamp && playTimestamp == rhs.playTimestamp);
	}
	bool operator!=(const EngineInstance &rhs) const { return !((*this) == rhs); }

	const EngineID id;
	EngineTimestamp parseTimestamp;
	EngineTimestamp playTimestamp;
};

}
}

#endif
