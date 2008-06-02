#ifdef _MSC_VER
//#  include "stdafx.h"
#endif
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "Digital.h"
#include <list>
#include <vector>
#include <iostream>

using namespace std;

Digital::Digital(std::string boardName) : Board(boardName)
{
	startupTime_l=0;
}

BoardID Digital::boardID() const
{
	return DIGITAL;
}

std::string Digital::boardType() const
{
	return "Digital";
}

int Digital::numberOfColumns() const
{
	return 24;
}

int Digital::numberOfBits() const
{
	return 24;
}

int Digital::numberOfRows() const
{
	return rawTable.size();
}

int Digital::LoadData(const std::list<stringTableLine> &data_in, int startcol,
		Int64 timeOffset)
{
	digitalRawTableLine line;

	bitTable_l.clear(); // This also marks the bitTable as not valid
	rawTable.clear();

	//Copy the time and numberOfColumns() columns starting at startcol
	for(list<stringTableLine>::const_iterator it=data_in.begin(); it!=data_in.end(); it++)
	{
		line = digitalRawTableLine(it->time+timeOffset, it->data.begin() + startcol);
		rawTable.push_back(line);
	}

	return startcol + numberOfColumns(); //return the new starting column
}

std::string Digital::printRawTableLine(int row, int colWidth) const
{
	std::string rowString;
	std::list<digitalRawTableLine>::const_iterator it = rawTable.begin();

	if(row >= numberOfRows())
	{
		rowString = "Error:  Table length exceeded.";
	}
	else
	{
		for(int j = 0; j < row; j++)
		{
			it++; //move the iterator to the correct row
		}

		rowString = it->printLine(colWidth);
	}

	return rowString;
}


void Digital::ConvertToBinary()
{
	bitTable_l.clear();

	for(list<digitalRawTableLine>::iterator it = rawTable.begin(); it != rawTable.end(); it++)
	{
		bitTable_l.push_back(bitTableLine(it->time,it->data));
	}

}

void Digital::CreateReset()
{
	bitTable_l.clear();

	vector<bool> data;
	data.assign(numberOfBits(),false);
	bitTable_l.push_back(bitTableLine(0,data));
}
