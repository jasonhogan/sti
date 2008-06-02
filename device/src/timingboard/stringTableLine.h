#ifndef STRINGTABLELINE_H
#define STRINGTABLELINE_H

#include <vector>
#include <string>
#include "types.h"

class stringTableLine
{
public:

	stringTableLine();
	stringTableLine(Int64 Time, std::vector<std::string> Data);
	~stringTableLine() {};

	Int64 time;
	std::vector<std::string> data;

	std::string printLine(int colWidth) const;

};



#endif
