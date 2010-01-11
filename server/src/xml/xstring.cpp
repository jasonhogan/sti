/*! \file xstring.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class xstring
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

#include "xstring.h"

xstring::xstring() : strForm( "" )
{
	unicodeForm = XMLString::transcode( strForm.c_str() );
}

xstring::xstring(const xstring& copy) : strForm( copy.str() )
{
	unicodeForm = XMLString::transcode( strForm.c_str() );
}

xstring& xstring::operator=(const xstring& rhs)
{
	strForm = rhs.str();

	XMLCh* newResource = XMLString::transcode( strForm.c_str() );
	XMLString::release( &unicodeForm );
	unicodeForm = newResource;

	return *this;
}


xstring::xstring(const char* const cstr) : strForm( cstr )
{
	unicodeForm = XMLString::transcode( strForm.c_str() );
}

xstring::xstring(const std::string& str) : strForm( str )
{
	unicodeForm = XMLString::transcode( strForm.c_str() );
}

xstring::~xstring()
{
	XMLString::release( &unicodeForm );
}

const XMLCh* xstring::toXMLCh() const
{
	return unicodeForm;
}

const std::string xstring::str() const
{
	return strForm;
}

