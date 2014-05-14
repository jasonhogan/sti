

//class Version
//{
//public:
//	std::string major();
//	std::string minor();
//	std::string patch();
//
//private:
//
//};

#include <sstream>
#include <string>

std::string getSTIServerVersion()
{
	std::string major = "1";
	std::string minor = "1";
	std::string patch = "0";

	std::stringstream version;
	version << major << "." << minor << "." << patch;

	return version.str();
}