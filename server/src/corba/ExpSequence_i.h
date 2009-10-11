/*! \file ExpSequence_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class ExpSequence_i
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

#ifndef EXPSEQUENCE_I_H
#define EXPSEQUENCE_I_H

#include "client.h"
#include <vector>
#include <string>
#include "parsedvalue.h"

class ExpSequence_i : public POA_STI::Client_Server::ExpSequence
{
public:

	ExpSequence_i();
	~ExpSequence_i();
    
    bool setExpSequence(const STI::Types::TStringSeq& Variables, 
		const STI::Types::TRowSeq& Experiments);

	STI::Types::TRowSeq* experiments();

    STI::Types::TStringSeq* variables();
    void variables(const STI::Types::TStringSeq& _v);

    ::CORBA::Boolean appendRow(const STI::Types::TStringSeq& newRow);
    ::CORBA::Boolean moveRow(::CORBA::ULong oldPos, ::CORBA::ULong newPos);
	::CORBA::Boolean editRow(::CORBA::ULong pos, const STI::Types::TRow& newRow);
    ::CORBA::Boolean deleteRow(::CORBA::ULong pos);
    void clear();
   
    void editRowDone(::CORBA::ULong pos, ::CORBA::Boolean newDone);


	// list of experiments
	std::vector<std::string> vars;	//column headers
	std::vector<STI::Types::TRow> rows;

	void setupVariables(const std::vector<libPython::ParsedValue> &variables);
	bool setupExperiments(const std::vector<libPython::ParsedValue> &experiments);

	void printExpSequence();

};

#endif
