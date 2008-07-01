#ifndef BITTABLELINE_H
#define BITTABLELINE_H

#include "types.h"
#include <string>
#include <vector>

class bitTableLine
{
public:

	bitTableLine();
	bitTableLine(uInt64 Time, std::vector<bool> Data);

	uInt64 time;
	std::vector<bool> data;

	void loadLine(uInt64 Time, std::vector<bool> Data);

	std::string printLine() const;

	uInt32 get_as_u32(int n) const;  //n=0,1,2,... counts the word to output

	bool operator<(const bitTableLine & rhs) const;

	void operator|=(bitTableLine & rhs);


};

#endif
