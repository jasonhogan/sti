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

#include <orbTypes.h>
#include <MixedData.h>
#include <utils.h>

#include <sstream>
#include <iostream>

MixedData::MixedData()
{
	type = Empty;
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


bool MixedData::operator<(const MixedData& other) const
{
/*
	if(type != other.getType())
		return false;

	bool result;

	switch(type)
	{
	case Boolean:
		result = ( value_b < other.getBoolean() );
		break;
	case Octet:
		result = ( value_o < other.getOctet() );
		result = true;
		break;
	case Int:
		result = ( value_i < other.getInt() );
		break;
	case Double:
		result = ( value_d < other.getDouble() );
		break;
	case String:
		result = ( value_s.compare(other.getString()) < 0 );
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
					result &= ( values.at(i) < otherValues.at(i) );
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
	*/

	bool result;
	MixedDataVector v1;
	MixedDataVector v2;

	result = false;

	if(type == Vector && other.getType() != Vector)
	{
		v1=this->getVector();
		for(unsigned int i = 0; i < v1.size(); i++)
		{
			result |= (v1.at(i) < other);
		}
	}
	else if(type != Vector && other.getType() == Vector)
	{
		v2 = other.getVector();
		for(unsigned int i = 0; i < v2.size(); i++)
		{
			result |= (*this < v2.at(i));
		}
	}
	else if (type != Vector)
		result = (this->getNumber() < other.getNumber());
	else
	{
		v1 = this->getVector();
		v2 = other.getVector();

		if(v1.size() != v2.size())
			return false;

		for (unsigned int i = 0; i < v1.size(); i++)
			result |= (v1.at(i) < v2.at(i));
	}

	return result;
}
bool MixedData::operator>(const MixedData& other) const
{
	bool result;
	MixedDataVector v1;
	MixedDataVector v2;

	result = false;

	if(type == Vector && other.getType() != Vector)
	{
		v1=this->getVector();
		for(unsigned int i = 0; i < v1.size(); i++)
		{
			result |= (v1.at(i) > other);
		}
	}
	else if(type != Vector && other.getType() == Vector)
	{
		v2 = other.getVector();
		for(unsigned int i = 0; i < v2.size(); i++)
		{
			result |= (*this > v2.at(i));
		}
	}
	else if (type != Vector)
		result = (this->getNumber() > other.getNumber());
	else
	{
		v1 = this->getVector();
		v2 = other.getVector();

		if(v1.size() != v2.size())
			return false;

		for (unsigned int i = 0; i > v1.size(); i++)
			result |= (v1.at(i) > v2.at(i));
	}

	return result;
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
void MixedData::setValue(long value)
{
	setValue(static_cast<int>(value));
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

void MixedData::setValue(const char* value)
{
	setValue(std::string(value));
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
		value_file = *value.getFile();
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
	STI::Types::TDataMixed_var test;

	switch( value._d() )
	{
	case STI::Types::DataBoolean:
		setValue( value.booleanVal() );
		break;
	case STI::Types::DataOctet:
		setValue( value.octetVal() );
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
		std::cerr << "Error in MixedData::setValue(const STI::Types::TDataMixed& value)! Unsupported type." << std::endl;
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

void MixedData::setValue(const STI::Types::TFile& value)
{
	type = File;
	value_file = value;
}

void MixedData::clear()
{
	values.clear();
	type = Vector;
	homogeneous = true;
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
	double result;

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
	case String:
		{
			if(STI::Utils::stringToValue(value_s, result))
				return result;
			else
			{
				result = 0;
				return (0.0 / result);	//NaN
			}
		}
		break;
	default:
		result = 0;
		return (0.0 / result);	//NaN
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
		value.longVal( static_cast<CORBA::Long>(value_i) );
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
	case File:
		value.file( value_file );
		break;
	case Empty:
	default:
		value.outVal(true);
		break;
	}

	return value;
}

const STI::Types::TFile* MixedData::getFile() const
{
	return &value_file;
}

MixedData& MixedData::getValueAt(unsigned i)
{
	if(getType() == Vector && i < values.size()) {
		return (values.at(i));
	} 
	else {
		return (*this);
	}
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
		//this happens when the MixedData was empty
		break;
	}

	homogeneous = true;
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

void MixedData::printSetValueError()
{
	std::cout << "Error: Unsupported type was passed to the MixedValue template constructor." << std::endl;
}

MixedData MixedData::operator + (const MixedData &other) const
{
	MixedData result;
	MixedDataVector v1;
	MixedDataVector v2;
	double zero = 0;

/*
	if((type == Vector && other.getType() != Vector) || (type != Vector && other.getType() == Vector))
	{
		return 0.0/zero;
	}
*/

	if(type == Vector && other.getType() != Vector)
	{
		v1=this->getVector();
		for(unsigned int i = 0; i < v1.size(); i++)
		{
			result.addValue(v1.at(i) + other);
		}
	}
	else if(type != Vector && other.getType() == Vector)
	{
		v2 = other.getVector();
		for(unsigned int i = 0; i < v2.size(); i++)
		{
			result.addValue(*this + v2.at(i));
		}
	}
	else if (type != Vector)
		result.setValue(this->getNumber() + other.getNumber());
	else
	{
		v1 = this->getVector();
		v2 = other.getVector();

		if(v1.size() != v2.size())
			return 0.0/zero;

		for (unsigned int i = 0; i < v1.size(); i++)
			result.addValue(v1.at(i) + v2.at(i));
	}

	return result;
}

MixedData MixedData::operator - (const MixedData &other) const
{
	MixedData result;
	MixedDataVector v1;
	MixedDataVector v2;
	double zero = 0;

/*
	if((type == Vector && other.getType() != Vector) || (type != Vector && other.getType() == Vector))
		return 0.0/zero;
*/

	if(type == Vector && other.getType() != Vector)
	{
		v1=this->getVector();
		for(unsigned int i = 0; i < v1.size(); i++)
		{
			result.addValue(v1.at(i) - other);
		}
	}
	else if(type != Vector && other.getType() == Vector)
	{
		v2 = other.getVector();
		for(unsigned int i = 0; i < v2.size(); i++)
		{
			result.addValue(*this - v2.at(i));
		}
	}
	else if (type != Vector)
		result.setValue(this->getNumber() - other.getNumber());
	else
	{
		v1 = this->getVector();
		v2 = other.getVector();

		if(v1.size() != v2.size())
			return 0.0/zero;

		for (unsigned int i = 0; i < v1.size(); i++)
			result.addValue(v1.at(i) - v2.at(i));
	}

	return result;
}

MixedData MixedData::operator * (const MixedData &other) const
{
	MixedData result;
	MixedDataVector v1;
	MixedDataVector v2;
	double zero = 0;

/*
	if((type == Vector && other.getType() != Vector) || (type != Vector && other.getType() == Vector))
		return 0.0/zero;
*/

	if(type == Vector && other.getType() != Vector)
	{
		v1=this->getVector();
		for(unsigned int i = 0; i < v1.size(); i++)
		{
			result.addValue(v1.at(i) * other);
		}
	}
	else if(type != Vector && other.getType() == Vector)
	{
		v2 = other.getVector();
		for(unsigned int i = 0; i < v2.size(); i++)
		{
			result.addValue((*this) * v2.at(i));
		}
	}
	else if (type != Vector)
		result.setValue(this->getNumber() * other.getNumber());
	else
	{
		v1 = this->getVector();
		v2 = other.getVector();

		if(v1.size() != v2.size())
			return 0.0/zero;

		for (unsigned int i = 0; i < v1.size(); i++)
			result.addValue(v1.at(i) * v2.at(i));
	}

	return result;
}

MixedData MixedData::operator / (const MixedData &other) const
{
	MixedData result;
	MixedDataVector v1;
	MixedDataVector v2;
	double zero = 0;

/*
	if((type == Vector && other.getType() != Vector) || (type != Vector && other.getType() == Vector))
		return 0.0/zero;
*/


	if(type == Vector && other.getType() != Vector)
	{
		v1=this->getVector();
		for(unsigned int i = 0; i < v1.size(); i++)
		{
			result.addValue(v1.at(i) / other);
		}
	}
	else if(type != Vector && other.getType() == Vector)
	{
		v2 = other.getVector();
		for(unsigned int i = 0; i < v2.size(); i++)
		{
			result.addValue((*this) / v2.at(i));
		}
	}
	else if (type != Vector)
		result.setValue(this->getNumber() / other.getNumber());
	else
	{
		v1 = this->getVector();
		v2 = other.getVector();

		if(v1.size() != v2.size())
			return 0.0/zero;

		for (unsigned int i = 0; i < v1.size(); i++)
			result.addValue(v1.at(i) / v2.at(i));
	}

	return result;
}

MixedData MixedData::sqroot()
{
	MixedData result;
	MixedDataVector v1;

	if (type != Vector)
		result.setValue(sqrt(this->getNumber()));
	else
	{
		v1 = this->getVector();

		for (unsigned int i = 0; i < v1.size(); i++)
			result.addValue(v1.at(i).sqroot());
	}

	return result;

}


