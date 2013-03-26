
namespace STI
{
namespace TimingEngine
{

class EngineID
{
public:
	EngineID(int id) : number(id) { description = STI::Utils::valueToString(id); }
	EngineID(int id, std::string name) : number(id), description(name) {}

	bool operator==(const EngineID& rhs) { return (this->number == rhs.number);}
private:
	int number;
	std::string description;
};

class EngineTimestamp
{
	static set<EngineTimestamp> timestamps;	//to ensure uniqueness?

	double timestamp;
}

class EngineInstance
{
	static set<EngineInstance> globalInstances;	//one or the other...
	
	EngineID id;
	EngineTimestamp timestamp;
};

}
}
