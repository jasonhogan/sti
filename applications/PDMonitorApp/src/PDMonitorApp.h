


#ifndef PDMONITORAPP_H
#define PDMONITORAPP_H

#include <STI_ApplicationAdapter.h>
#include <iostream>

using boost::filesystem::path;

class PDMonitorApp : public STI_ApplicationAdapter
{
public:
	PDMonitorApp(ORBManager* orb_manager, std::string configFilename);
	~PDMonitorApp();

	void defineFunctionCalls() {};

	MixedData handleFunctionCall(std::string functionName, std::vector<MixedValue> args)
	{
		if(functionName.compare("func1")==0) {
			return func1(args.at(0).getInt());
		}
		if(functionName.compare("func2")==0) {
			return MixedData( func2() );
		}
		if(functionName.compare("func3")==0) {
			return MixedData( func3() );
		}
		return MixedData();
	}

	int func1(int j) {
		std::cout << "Hello func1: " << j << std::endl;
		return 5*j;
	}
	double func2() {return 4.21;}
	std::string func3() {return "hello";}

//	std::string Execute(int argc, char* argv[]) {return "In Test App!";};

	void defineAppChannels()
	{
		addOutputChannel(1, STI::Types::ValueNumber);
	}
	bool writeAppChannel(unsigned short channel, const MixedValue& value) 
	{
		std::cout << "Channel out: " << value.getNumber() << std::endl;
		return true;
	};

};

#endif