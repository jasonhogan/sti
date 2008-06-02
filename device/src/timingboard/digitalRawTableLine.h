#ifndef _DIGITALRAWTABLELINE_H
#define _DIGITALRAWTABLELINE_H

// Filename: digitalRawTableLine.h
//

#include "types.h"
#include <vector>
#include <string>
#include "utils.h"


class digitalRawTableLine
{
public:
	
	digitalRawTableLine() {};
	digitalRawTableLine(Int64 time, std::vector<std::string>::const_iterator start);
	
	Int64 time;
	std::vector<bool> data;

	std::string printLine(int colWidth) const;

};

#endif
