
#include <TNetworkFileReader.h>

TNetworkFileReader::TNetworkFileReader(STI::Types::TNetworkFile_ptr file)
{
	data = NULL;
	networkFileRef = file;
	
	try {
		fileName = networkFileRef->fileName();
		accessible = true;
	} catch(CORBA::TRANSIENT& ex) {
		accessible = false;
	} catch(CORBA::SystemException& ex) {
		accessible = false;
	} catch(CORBA::Exception&) {
		accessible = false;
	} catch(...) {
		accessible = false;
	}
}
TNetworkFileReader::~TNetworkFileReader()
{
	if(data != NULL ) {
		delete data;
		data = NULL;
	}
}

bool TNetworkFileReader::isAccessible()
{
	return accessible;
}

int TNetworkFileReader::fileLength()
{
	return length;
}

bool TNetworkFileReader::read()
{
	try {
		length = networkFileRef->length();
		accessible = true;
	} catch(CORBA::TRANSIENT& ex) {
		accessible = false;
	} catch(CORBA::SystemException& ex) {
		accessible = false;
	} catch(CORBA::Exception&) {
		accessible = false;
	} catch(...) {
		accessible = false;
	}
	if(!accessible || length <= 0)
		return false;

	if(data != NULL ) {
		delete data;
		data = NULL;
	}

	data = new char[length];
	
	STI::Types::TOctetSeq_var tOctetData;

	bool success = true;
	int i = 0;
	int packetSize = 1000;
	int timeout = 3;

	while(i < length && timeout > 0) {
		try {
			networkFileRef->getBytes(i, packetSize, tOctetData.out());
			success = true;
		} catch(...) {
			success = false;
			timeout--;
		}
		if(success) {

			for(unsigned j = 0; j < tOctetData->length(); j++) 
			{
				data[i + j] = tOctetData[j];
			}

			i += packetSize;
		}
	}

	return success;

}
