#ifndef TESTAPP_H
#define TESTAPP_H

#include <orbTypes.h>
#include <string>

class TNetworkFileReader
{
public:
	TNetworkFileReader(STI::Types::TNetworkFile_ptr file);
	~TNetworkFileReader();

	bool read();
	bool isAccessible();
	int fileLength();

private:

	STI::Types::TNetworkFile_ptr networkFileRef;

	std::string fileName;
	char* data;
	int length;

	bool accessible;

};

#endif