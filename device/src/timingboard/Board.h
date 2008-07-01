#ifndef BOARD_H
#define BOARD_H

// Filename: Board.h
//
// Purpose: Defines the abstract Board class which is an interface for various 
//			specific module board classes (Analog, Digitital, DDS, etc.). 
//			A Board object stores its own the raw timing table containing
//			the list of desired states.  It has methods to convert this 
//			table to a register-level table and ultimately to a clock-edge-
//			level table suitable for export to the FPGA.

#include <list>
#include <vector>
#include <string>
#include "types.h"
#include "stringTableLine.h"
#include "bitTableLine.h"

class Board
{
public:

	Board(std::string boardName);
	virtual ~Board() {};

	//Access functions
	virtual BoardID boardID() const=0;
	virtual std::string boardType() const=0;
	virtual std::string boardName() const;
	virtual void setBoardName(std::string boardName);
	virtual int numberOfColumns() const=0;
	virtual int numberOfRows() const=0;
	virtual int numberOfBits() const=0;
	virtual uInt64 startupTime() const;

	virtual void ConvertToBinary()=0;
	virtual void CreateReset()=0;

	virtual const std::list<bitTableLine> *bitTable();

	virtual double getFpgaClk() const;
	virtual void setFpgaClk(double fpgaClk);
	uInt64 time_to_cycles(double time);

	//Loads the columns [startcol; startcol+numberOfColumns-1] and will
	//return either startcol+numberOfColumns or total number of columns in data_in
	//(whichever is lower)
	//Additionally, it copies the time-column (which isn't counted above)
	//The timecolumn gets incremented by offset.
	virtual int LoadData(const std::list<stringTableLine> &data_in, int startcol,
		uInt64 timeOffset=0) = 0;

	virtual std::string printRawTableLine(int row, int colWidth) const = 0;


protected:
	//Bit pattern timing table
	//Contains the absolute time and the bit pattern to be sent to the data
	//lines for a single board.
	//You need to delete this list whenever the source-data gets changed.
	//This flags the need to run ConvertToBinary().
	std::list<bitTableLine> bitTable_l;

	std::string boardName_l;

	double fpgaClk;      //FPGA clock in Hz
	uInt64 startupTime_l; //How long does this board need to write to all channels
};

#endif
