#ifndef PARSEDTAG_H
#define PARSEDTAG_H

#include <string>
#include "parsedpos.h"
#include "parsedvalue.h"

namespace libPython
{

class ParsedTag
{
public:
	ParsedTag(const std::string& tagName, double tagTime, const ParsedPos& pos, 
		bool timeIncluded, unsigned eventListSize);
	~ParsedTag();

	std::string name;
	ParsedPos position;
	double time;
	bool timeDefined;
	unsigned nextEventNumber;

};

}

#endif

