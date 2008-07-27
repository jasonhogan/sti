#ifdef _MSC_VER
//#  include "stdafx.h"
#endif
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "bitTableLine.h"
#include <list>
#include "utils.h"

using namespace std;

bitTableLine::bitTableLine()
{
}

bitTableLine::bitTableLine(uInt64 Time, vector<bool> Data)
{
	time = Time;
	data = Data;
}

void bitTableLine::loadLine(uInt64 Time, vector<bool> Data)
{
	time = Time;
	data = Data;
}

bool bitTableLine::operator<(const bitTableLine & rhs) const
{
	return (time < rhs.time); 
}

void bitTableLine::operator|=(bitTableLine & rhs)
{
	//performs a bitwise OR between this and rhs and assigns the
	//resulting vector<bool> to this

	for(unsigned i=0; i<data.size();i++)
	{
		data[i] = data[i] || rhs.data[i];
	}
}


string bitTableLine::printLine() const
{
	string printString;
	printString+=u64_to_str(time);
	printString+="|";

	int i=0;

	for(std::vector<bool>::const_iterator it=data.begin(); it!=data.end(); it++,i++)
	{
		if(i%4 == 0 && i>0)
		{
			//spacing
			printString += " ";
		}

		if(*it)
		{
			printString += "1";
		}
		else if (!*it)
		{
			printString += "0";
		}
		else
		{
			printString += "X";
		}
	}

	printString += "|";

	return printString;

}

//n=0,1,2,... counts the word to output
uInt32 bitTableLine::get_as_u32(int n) const
{
	int   i;
	uInt32 res;

	n=n*8*sizeof(uInt32);  //convert from 32bit-words to bits
	if(n+8*sizeof(uInt32)>data.size())
		i=data.size()-1;
	else
		i=n+8*sizeof(uInt32)-1;

	for(res=0; i>=n; i--)
		res=(res<<1)|(data[i]?1:0);

	return res;
}
