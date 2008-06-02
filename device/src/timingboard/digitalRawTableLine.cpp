#ifdef _MSC_VER
//#  include "stdafx.h"
#endif
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "digitalRawTableLine.h"
#include "types.h"
#include "utils.h"
#include <iostream>


using namespace std;

digitalRawTableLine::digitalRawTableLine(Int64 time, std::vector<std::string>::const_iterator start)
	: time(time)
{
	
	//Check for blank table entries
	if((start->length() == 0) || ((start + 1)->length() == 0))
	{
		cerr << "Error:  Blank table entry encountered at time " 
			<< u64_to_str(time).c_str() << "." << endl;
	}		

	for(int i=0; i<24; i++)
	{
		if(str_to_u32(*(start + i)) == 1)
			data.push_back(true);
		else
			data.push_back(false);
	}

}

std::string digitalRawTableLine::printLine(int colWidth) const
{
	int i;
	string printString;

	printString += u64_to_str(time);

	//padding
	for(i = printString.length(); i < colWidth; i++)
	{
		printString += " ";
	}
	printString += "|";

	//data columns
	for(vector<bool>::const_iterator it=data.begin(); it!=data.end(); it++)
	{
		//padding
		for(i = 1; i < colWidth; i++)
		{
			printString += " ";
		}

		if(*it)
			printString += "1";
		else
			printString += "0";

		printString += "|";
	}

	return printString;

}
