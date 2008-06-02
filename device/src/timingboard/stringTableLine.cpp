#ifdef _MSC_VER
//#  include "stdafx.h"
#endif
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "stringTableLine.h"

#include <list>
#include <string>
#include <vector>
#include <iostream>
#include "utils.h"

using namespace std;

stringTableLine::stringTableLine(Int64 Time, vector<string> Data) 
{
	time = Time; 
	data = Data;
}

std::string stringTableLine::printLine(int colWidth) const
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
	for(vector<string>::const_iterator it=data.begin(); it!=data.end(); it++)
	{
		//padding
		for(i = it->length(); i < colWidth; i++)
		{
			printString += " ";
		}

		printString += (*it);

		printString += "|";
	}

	return printString;

}
