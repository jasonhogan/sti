#ifndef STI_TIMINGENGINE_ENGINEID_H
#define STI_TIMINGENGINE_ENGINEID_H

#include "utils.h"

namespace STI
{
namespace TimingEngine
{

class EngineID
{
public:
	EngineID(int id) : number(id) { description = STI::Utils::valueToString(id); }
	EngineID(int id, std::string name) : number(id), description(name) {}

	bool operator<(const EngineID& rhs) const { return (this->number < rhs.number); }
	bool operator==(const EngineID& rhs) const { return (this->number == rhs.number);}

private:
	int number;
	std::string description;
};

class EngineTimestamp
{
//	static set<EngineTimestamp> timestamps;	//to ensure uniqueness?
public:

	bool operator==(const EngineTimestamp &rhs) const { return timestamp == rhs.timestamp; }
	bool operator<(const EngineTimestamp &rhs) const { return timestamp < rhs.timestamp; }
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
	
	const EngineID id;
	EngineTimestamp parseTimestamp;
	EngineTimestamp playTimestamp;
};

}
}

#endif
