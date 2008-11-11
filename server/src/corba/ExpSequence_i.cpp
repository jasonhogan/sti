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

ExpSequence_i::ExpSequence_i()
{
	vars.clear();
	rows.clear();
}


ExpSequence_i::~ExpSequence_i()
{
}

bool ExpSequence_i::setExpSequence(const STI_Client_Server::TStringSeq& Variables, 
					const STI_Client_Server::TRowSeq& Experiments)
{
	using STI_Client_Server::TRow;
	using STI_Client_Server::TRow_var;

	variables(Variables);	//resets vars and rows

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


::CORBA::Boolean ExpSequence_i::appendRow(const STI_Client_Server::TStringSeq& newRow)
{
	using STI_Client_Server::TRow;
	using STI_Client_Server::TRow_var;

	if( newRow.length() == vars.size() )
	{
		rows.push_back( TRow() );
		rows.back().done = false;
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
	using STI_Client_Server::TRow;
	using STI_Client_Server::TRow_var;

	bool error = false;

	if(oldPos >=0 && oldPos < rows.size() && newPos >= 0)
	{
		TRow_var tempRow( new TRow(rows[oldPos]) );      //copy
		rows.erase(rows.begin() + oldPos);               //delete from oldPos
		if(newPos < rows.size())
			rows.insert(rows.begin() + newPos, tempRow); //insert at newPos
		else
			rows.push_back( tempRow );		             //insert at the end
	}
	else
		error = true;	//error; out of bounds
	
	return error;
}


::CORBA::Boolean ExpSequence_i::editRow(::CORBA::ULong pos, const STI_Client_Server::TRow& newRow)
{
	return true;
}

::CORBA::Boolean ExpSequence_i::deleteRow(::CORBA::ULong pos)
{
	if(pos < rows.size() && pos >= 0)
	{
		rows.erase(rows.begin() + pos);
		return false;
	}
	else
		return true;	//error; out of bounds
}


void ExpSequence_i::clear()
{
	rows.clear();
}

void ExpSequence_i::editDone(::CORBA::ULong pos, ::CORBA::Boolean newDone)
{
}

void ExpSequence_i::variables(const STI_Client_Server::TStringSeq& _v)
{
	vars.clear();
	rows.clear();

	for(unsigned i = 0; i < _v.length(); i++)
		vars.push_back( string( _v[i] ) );
}


STI_Client_Server::TStringSeq* ExpSequence_i::variables()
{
	using STI_Client_Server::TStringSeq;
	using STI_Client_Server::TStringSeq_var;

	TStringSeq_var variablesSeq( new TStringSeq );
	variablesSeq->length( vars.size() );

	for(unsigned i = 0; i < vars.size(); i++)
		variablesSeq[i] = CORBA::string_dup( vars[i].c_str() );

	return variablesSeq._retn();
}


STI_Client_Server::TRowSeq* ExpSequence_i::experiments()
{
	using STI_Client_Server::TRow;
	using STI_Client_Server::TRow_var;
	using STI_Client_Server::TRowSeq;
	using STI_Client_Server::TRowSeq_var;

	TRowSeq_var rowSeq( new TRowSeq );
	rowSeq->length( rows.size() );

	for(unsigned i = 0; i < rows.size(); i++)
	{
//		rowSeq[i].val.length( rows[i].val.length() );
		rowSeq[i] = TRow( rows[i] );
	}

	return rowSeq._retn();
}

void ExpSequence_i::printExpSequence()
{
	unsigned i,j;
	for(i = 0; i < vars.size(); i++)
		cerr << vars[i] << " | ";
	cerr << "Done" << endl;

	for(i = 0; i < rows.size(); i++)
	{
		for(j = 0; j < rows[i].val.length(); j++)
			cerr << rows[i].val[j] << " | ";
		cerr << (rows[i].done ? "Yes" : "No") << endl;
	}
	cerr << endl;
}

