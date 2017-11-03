/*! \file xstring.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class xstring
 *  \section license License
 *
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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


#ifndef XSTRING_H
#define XSTRING_H

#include <string>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_USE

class xstring 
{
public:

	xstring();
	xstring(const xstring& copy);
	xstring& operator=(const xstring& rhs);

	xstring(const char* const cstr);
	xstring(const std::string& str);
	~xstring();

	const XMLCh* toXMLCh() const;
	const std::string str() const;

private:
	
	XMLCh* unicodeForm;
	std::string strForm;

};

#endif

