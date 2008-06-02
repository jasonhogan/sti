#ifndef BITTABLELINE_H
#define BITTABLELINE_H

#include "types.h"
#include <string>
#include <vector>

class bitTableLine
{
public:

	bitTableLine();
	bitTableLine(Int64 Time, std::vector<bool> Data);

	Int64 time;
	std::vector<bool> data;

	void loadLine(Int64 Time, std::vector<bool> Data);

	std::string printLine() const;

	Int32 get_as_u32(int n) const;  //n=0,1,2,... counts the word to output

	bool operator<(const bitTableLine & rhs) const;

	void operator|=(bitTableLine & rhs);


};

#endif
