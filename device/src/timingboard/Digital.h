#ifndef DIGITAL_H
#define DIGITAL_H

// Filename: Digital.h
//

#include "Board.h"
#include "digitalRawTableLine.h"

class Digital : public Board
{
public:

	Digital(std::string boardName);
	virtual ~Digital() {};

	//Access functions
	virtual BoardID boardID() const;
	virtual std::string boardType() const;
	virtual int numberOfColumns() const;
	virtual int numberOfRows() const;
	virtual int numberOfBits() const;

	virtual void ConvertToBinary();
	virtual void CreateReset();

	virtual int LoadData(const std::list<stringTableLine> &data_in, int startcol,
		Int64 timeOffset=0);

	virtual std::string printRawTableLine(int row, int colWidth) const;

protected:

private:

	std::list<digitalRawTableLine> rawTable;

};

#endif
