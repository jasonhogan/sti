#ifdef _MSC_VER
//#  include "stdafx.h"
#endif
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "Board.h"

using namespace std;


Board::Board(string boardName)
{
	setFpgaClk(100e6);
	setBoardName(boardName);
}

string Board::boardName() const
{
	return boardName_l;
}

void Board::setBoardName(string boardName)
{
	boardName_l=boardName;
}

const list<bitTableLine> *Board::bitTable()
{
	if(bitTable_l.size()==0)
		ConvertToBinary();

	return &bitTable_l;
}

double Board::getFpgaClk() const
{
	return fpgaClk;
}

void Board::setFpgaClk(double fpgaClk)
{
	if (fpgaClk > 0)
	{
		this->fpgaClk = fpgaClk;
	}
}

uInt64 Board::startupTime() const
{
	return startupTime_l;
}

uInt64 Board::time_to_cycles(double time)
{
	//Rounds up to the nearest clock cycle
	//Assumes time in ns
	return static_cast<uInt64>(time * getFpgaClk()  + 0.5);
}
