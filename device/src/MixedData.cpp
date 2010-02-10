/*! \file MixedData.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class MixedData
 *  \section license License
 *
 *  Copyright (C) 2010 Jason Hogan <hogan@stanford.edu>\n
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


#include <MixedData.h>
#include <sstream>
#include <iostream>

MixedData::MixedData()
{
	type = Vector;
}
MixedData::MixedData(const MixedData& copy)
{
	setValue(copy);
}

MixedData::~MixedData()
{
}




bool MixedData::operator==(const MixedData& other) const
{
	if(type != other.getType())
		return false;

	bool result;

	switch(type)
	{
	case Boolean:
		result = ( value_b == other.getBoolean() );
		break;
	case Octet:
		result = ( value_o == other.getOctet() );
		result = true;
		break;
	case Int:
		result = ( value_i == other.getInt() );
		break;
	case Double:
		result = ( value_d == other.getDouble() );
		break;
	case String:
		result = ( value_s.compare(other.getString()) == 0 );
		break;
	case File:
//		result = ( value_f == other.getFile() );
		result = true;
		break;
	case Vector:
		{
			const MixedDataVector& otherValues = other.getVector();

			if(values.size() != otherValues.size())
			{
				result = false;
			}
			else
			{
				result = true;

				for(unsigned i = 0; (i < otherValues.size() && i < values.size()); i++)
				{
					result &= ( values.at(i) == otherValues.at(i) );
				}
			}
		}
		break;
	case Empty:
		result = true;
		break;
	default:
		//this should never happen
		result = false;
		break;
	}

	return result;
}

bool MixedData::operator!=(const MixedData& other) const
{
	return !( (*this) == other );
}


void MixedData::setValue(bool value)
{
	clear();

	value_b = value;
	type = Boolean;
}
void MixedData::setValue(unsigned char value)
{
	clear();

	value_o = value;
	type = Octet;
}
void MixedData::setValue(int value)
{
	clear();

	value_i = value;
	type = Int;
}


void MixedData::setValue(double value)
{
	clear();

	value_d = value;
	type = Double;
}

void MixedData::setValue(std::string value)
{
	clear();

	value_s = value;
	type = String;
}
//void MixedData::setValue(TFile value)
//{
//	clear();
//
//	value_f = value;
//	type = File;
//}
void MixedData::setValue(const MixedData& value)
{
	clear();
	type = value.getType();

	switch( type )
	{
	case Boolean:
		value_b = value.getBoolean();
		break;
	case Octet:
		value_o = value.getOctet();
		break;
	case Int:
		value_i = value.getInt();
		break;
	case Double:
		value_d = value.getDouble();
		break;
	case String:
		value_s = value.getString();
		break;
	case File:
//		value_f = value.getFile();
		break;
	case Vector:
		{
			const MixedDataVector& newValues = value.getVector();
		
			for(unsigned i = 0; i < newValues.size(); i++)
			{
				addValue( newValues.at(i) );
			}
		}
		break;
	case Empty:
		break;
	default:
		//this should never happen
		break;
	}

}

void MixedData::setValue(const STI::Types::TDataMixed& value)
{
	switch( value._d() )
	{
	case STI::Types::DataBoolean:
		setValue( value.booleanVal() );
		break;
	case STI::Types::DataLong:
		setValue( value.longVal() );
		break;
	case STI::Types::DataDouble:
		setValue( value.doubleVal() );
		break;
	case STI::Types::DataString:
		setValue( std::string( value.stringVal() ) );
		break;
	case STI::Types::DataFile:
		setValue( value.file() );
		break;
	case STI::Types::DataVector:
		setValue( value.vector() );
		break;
	default:
		//this should never happen
		break;
	}
}

void MixedData::setValue(const STI::Types::TDataMixedSeq& value)
{
	clear();
	type = Vector;

	for(unsigned i = 0; i < value.length(); i++)
	{
		addValue( value[i] );
	}
}

void MixedData::clear()
{
	values.clear();
	type = Vector;
}

MixedData::MixedDataType MixedData::getType() const
{
	return type;
}
std::string MixedData::getTypeString() const
{
	switch(type)
	{
	case Boolean:
		return "Boolean";
	case Octet:
		return "Octet";
	case Int:
		return "Int";
	case Double:
		return "Double";
	case String:
		return "String";
	case Picture:
		return "Picture";
	case File:
		return "File";
	case Vector:
		return "Vector";
	case Empty:
	default:
		return "Empty";
	}
}


bool MixedData::getBoolean() const
{
	return value_b;
}
unsigned char MixedData::getOctet() const
{
	return value_o;
}

int MixedData::getInt() const
{
	return value_i;
}

double MixedData::getDouble() const
{
	return value_d;
}

double MixedData::getNumber() const
{
	switch(type)
	{
	case Boolean:
		return ( static_cast<double>(value_b) );
		break;
	case Int:
		return ( static_cast<double>(value_i) );
		break;
	case Double:
		return value_d;
		break;
	default:
		return 0;
	}
}

std::string MixedData::getString() const
{
	return value_s;
}

const MixedDataVector& MixedData::getVector() const
{
	return values;
}

const STI::Types::TDataMixed MixedData::getTDataMixed() const
{
	STI::Types::TDataMixed value;

	switch(type)
	{
	case Boolean:
		value.booleanVal( value_b );
		break;
	case Octet:
		value.octetVal( value_o );
		break;
	case Int:
		value.longVal( value_i );
		break;
	case Double:
		value.doubleVal( value_d );
		break;
	case String:
		value.stringVal( value_s.c_str() );
		break;
	case Vector:
		value.vector(STI::Types::TDataMixedSeq());
		value.vector().length( values.size() );
		for(unsigned i = 0; i < values.size(); i++)
		{
			value.vector()[i] = values.at(i).getTDataMixed();
		}
		break;
	}

	return value;
}

void MixedData::convertToVector()
{
	if(type == Vector)
		return;

	MixedDataType oldType = type;

	clear();
	type = Vector;

	switch(oldType)
	{
	case Boolean:
		addValue(value_b);
		break;
	case Octet:
		addValue(value_o);
		break;
	case Int:
		addValue(value_i);
		break;
	case Double:
		addValue(value_d);
		break;
	case String:
		addValue(value_s);
		break;
	case File:
//		addValue(value_f);
		break;
	default:
		//this should never happen
		break;
	}
}

std::string MixedData::print() const
{
	std::stringstream result;
	
	switch(type)
	{
	case Boolean:
		result << value_b;
		break;
	case Octet:
		result.setf( std::ios::hex, std::ios::basefield );
		result << value_o;
		break;
	case Int:
		result << value_i;
		break;
	case Double:
		result << value_d;
		break;
	case String:
		result << value_s;
		break;
	case Vector:
		result << "(";
		for(unsigned i = 0; i < values.size(); i++)
		{
			if(i > 0 && values.at(i).getType() != Octet)
			{
				result << ",";
			}
			result << values.at(i).print();
		}
		result << ")";
		break;
	case Empty:
		result << "Null";
		break;
	default:
		//this should never happen
		break;
	}

	return result.str();
}

