


#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include "ParsedTag.h"

using std::string;
using libPython::ParsedTag;
using libPython::ParsedPos;


ParsedTag::ParsedTag(const std::string& tagName, double tagTime, const ParsedPos& pos, bool timeIncluded, unsigned eventListSize)
: name(tagName), position(pos), time(tagTime), timeDefined(timeIncluded)
{
	nextEventNumber = eventListSize;
//	this->position = new ParsedPos(pos);
}

ParsedTag::~ParsedTag()
{
	//if(position != NULL)
	//	delete position;
}