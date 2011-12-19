/*! \file ExpSequence_i.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class ExpSequence_i
 *  \section license License
 *
 *  Copyright (C) 2008 Jason Hogan <hogan@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  The STI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The STI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the STI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ExpSequence_i.h"

#include <iostream>
using std::cerr;
using std::endl;

using std::string;
using std::vector;

ExpSequence_i::ExpSequence_i(STI_Server* server) : sti_server(server)
{
	vars.clear();
	rows.clear();
	expNumber = 0;

	currentOverwritten = STI::Types::TOverwrittenSeq_var(new STI::Types::TOverwrittenSeq);

}


ExpSequence_i::~ExpSequence_i()
{
}


bool ExpSequence_i::setExpSequence(const STI::Types::TStringSeq& Variables, 
					const STI::Types::TRowSeq& Experiments)
{
	using STI::Types::TRow;
	using STI::Types::TRow_var;

	variables(Variables);	//resets vars and rows

	expNumber = 0;
	for(unsigned i = 0; i < Experiments.length(); i++)
	{
		if(Experiments[i].val.length() == vars.size())
		{
			rows.push_back( TRow( Experiments[i] ) );
		}
		else
			return true;		//error; column number mismatch
	}
	return false;
}


::CORBA::Boolean ExpSequence_i::appendRow(const STI::Types::TStringSeq& newRow)
{
	using STI::Types::TRow;
	using STI::Types::TRow_var;

	if( newRow.length() == vars.size() )
	{
		rows.push_back( TRow() );
		rows.back().done = false;
		rows.back().row = rows.size() - 1;
		rows.back().val.length( newRow.length() );

		for(unsigned i = 0; i < newRow.length(); i++)
		{
			rows.back().val[i] = CORBA::string_dup( newRow[i] );
		}
		return false;
	}
	else
		return true;	//error; column number mismatch
}


::CORBA::Boolean ExpSequence_i::moveRow(::CORBA::ULong oldPos, ::CORBA::ULong newPos)
{
	using STI::Types::TRow;
	using STI::Types::TRow_var;

	bool error = false;

	if(oldPos >=0 && oldPos < rows.size() && newPos >= 0)
	{
		TRow_var tempRow( new TRow(rows[oldPos]) );      //copy
		rows.erase(rows.begin() + oldPos);               //delete from oldPos
		if(newPos < rows.size())
			rows.insert(rows.begin() + newPos, tempRow); //insert at newPos
		else
			rows.push_back( tempRow );		             //insert at the end

		for(unsigned i = ((oldPos < newPos) ? oldPos : newPos); i < rows.size() 
			&& i <= ((oldPos < newPos) ? newPos : oldPos ); i++)
		{
			rows.at(i).row = i;
		}
	}
	else
		error = true;	//error; out of bounds
	
	return error;
}


::CORBA::Boolean ExpSequence_i::editRow(::CORBA::ULong pos, const STI::Types::TRow& newRow)
{
	// can fail if already done
	return true;
}

::CORBA::Boolean ExpSequence_i::deleteRow(::CORBA::ULong pos)
{
	if(pos < rows.size() && pos >= 0)
	{
		rows.erase(rows.begin() + pos);
		
		for(unsigned i = pos; i < rows.size(); i++)
		{
			rows.at(i).row = i;
		}

		return false;
	}
	else
		return true;	//error; out of bounds
}


void ExpSequence_i::clear()
{
	expNumber = 0;
	rows.clear();
}

::CORBA::Boolean ExpSequence_i::editRowDone(::CORBA::ULong pos, ::CORBA::Boolean newDone)
{
	if(pos < rows.size() && pos >= 0)
	{
		rows.at(pos).done = newDone;

		//send "experiment done" event to clients
		STI::Pusher::TSequenceEvent sequenceEvt;
		sequenceEvt.type = STI::Pusher::UpdateDoneStatus;
		sequenceEvt.newRow = STI::Types::TRow( rows.at(pos) );

		sti_server->sendEvent( sequenceEvt );

		return false;
	}
	else
		return true;	//error; out of bounds
}



void ExpSequence_i::variables(const STI::Types::TStringSeq& _v)
{
	vars.clear();
	rows.clear();

	for(unsigned i = 0; i < _v.length(); i++)
		vars.push_back( string( _v[i] ) );
}


STI::Types::TStringSeq* ExpSequence_i::variables()
{
	using STI::Types::TStringSeq;
	using STI::Types::TStringSeq_var;

	TStringSeq_var variablesSeq( new TStringSeq );
	variablesSeq->length( vars.size() );

	for(unsigned i = 0; i < vars.size(); i++)
		variablesSeq[i] = CORBA::string_dup( vars[i].c_str() );

	return variablesSeq._retn();
}


STI::Types::TRowSeq* ExpSequence_i::experiments()
{
	using STI::Types::TRow;
	using STI::Types::TRow_var;
	using STI::Types::TRowSeq;
	using STI::Types::TRowSeq_var;

	TRowSeq_var rowSeq( new TRowSeq );
	rowSeq->length( rows.size() );

	for(unsigned i = 0; i < rows.size(); i++)
	{
//		rowSeq[i].val.length( rows[i].val.length() );
		rowSeq[i] = TRow( rows[i] );
	}

	return rowSeq._retn();
}

const STI::Types::TOverwrittenSeq& ExpSequence_i::getCurrentOverwritten()
{

	currentOverwritten->length( vars.size() );

	for(unsigned i = 0; i < vars.size(); i++)
	{
		currentOverwritten[i].name  = CORBA::string_dup( vars.at(i).c_str() );
		currentOverwritten[i].value = CORBA::string_dup( rows.at(expNumber).val[i] );
	}

	return currentOverwritten;
}

bool ExpSequence_i::setCurrentExperimentToDone()
{
	return !editRowDone( expNumber, true);
}

bool ExpSequence_i::getNextExperiment()
{
	if(getNextNotDoneExperiment(expNumber, expNumber))
	{
		return true;	//another row needs to be run
	}
	else
		return false;	//all rows are done
}


bool ExpSequence_i::getNextNotDoneExperiment(unsigned startSearch, unsigned& nextExp)
{
	for(unsigned i = startSearch; i < rows.size(); i++)
	{
		if( !rows.at(i).done )
		{
			nextExp = i;
			return true;
		}
	}

	if( startSearch == 0 )
		return false;	//no more rows need to be run
	else
		return getNextNotDoneExperiment(0, nextExp);	//search the whole list for rows that need to be run
}


void ExpSequence_i::setupVariables(const vector<libPython::ParsedValue> &variables)
{
	vars.clear();

	unsigned i;

	for(i = 0; i < variables.size(); i++)
	{
		vars.push_back(variables.at(i).str());
	}
}

bool ExpSequence_i::setupExperiments(const vector<libPython::ParsedValue> &experiments)
{
	rows.clear();

	using STI::Types::TRow;
	unsigned i, j;

	for(i = 0; i < experiments.size(); i++)
	{
		if(experiments.at(i).type != libPython::VTlist)
			return true;	//experiments must be a list of lists
		if(experiments.at(i).list.size() != vars.size())
			return true;	//column number mismatch
		
		rows.push_back(TRow());
		rows.back().done = false;
		rows.back().row = rows.size() - 1;
		rows.back().val.length( experiments.at(i).list.size() );

		for(j = 0; j < experiments.at(i).list.size(); j++)
		{
			rows.back().val[j] = CORBA::string_dup( experiments.at(i).list.at(j).str().c_str() );
		}
	}
	return false;
}


void ExpSequence_i::printExpSequence()
{
	unsigned i,j;
	cerr << "Row" << " | ";
	for(i = 0; i < vars.size(); i++)
		cerr << vars[i] << " | ";
	cerr << "Done" << endl;

	for(i = 0; i < rows.size(); i++)
	{
		cerr << rows.at(i).row << " | ";
		for(j = 0; j < rows[i].val.length(); j++)
			cerr << rows[i].val[j] << " | ";
		cerr << (rows[i].done ? "Yes" : "No") << endl;
	}
	cerr << endl;
}
void ExpSequence_i::resetExpNumber()
{
	expNumber = 0;
}
