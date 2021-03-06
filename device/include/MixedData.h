/*! \file MixedData.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class MixedData
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

#ifndef MIXEDDATA_H
#define MIXEDDATA_H

#include <vector>
#include <string>
#include <math.h>
#include <orbTypes.h>


namespace STI { 
	namespace Types { 
		class TDataMixed;
		class TDataMixedSeq;
	}; 
};

class MixedData;

typedef std::vector<MixedData> MixedDataVector;


class MixedData
{
public:
	
	MixedData();	//for std::vector
	template<typename T> MixedData(const T& value)
	{
		setValue(value);
	}	
	MixedData(const MixedData& copy);

	~MixedData();

	template<typename T> MixedData& operator= (const T& other)
	{
		setValue(other);
		return (*this);
	}

	
	template<typename T> bool operator==(const T& other) const
	{
		return (*this) == MixedData(other);
	}
	template<typename T> bool operator!=(const T& other) const
	{
		return (*this) != MixedData(other);
	}


	bool operator==(const MixedData& other) const;
	bool operator!=(const MixedData& other) const;

	bool operator<(const MixedData& other) const;
	bool operator>(const MixedData& other) const;

	enum MixedDataType {Boolean, Octet, Int, Double, String, Picture, File, Vector, Empty};

	template<typename T> void setValue(T value)
	{
		//This version of the function is call for all T values that are unsupported.
		//This template is called for all types that don't have an explicitly overloaded setValue function.

		printSetValueError();
	}


	template<typename T> void setValue(const std::vector<T>& value)
	{
		clear();
		type = Vector;

//		values.reserve( value.size() );

		for(unsigned i = 0; i < value.size(); i++)
		{
			addValue( value.at(i) );
		}
	}
	
	void setValue(bool value);
	void setValue(unsigned char value);
	void setValue(int value);
	void setValue(long value);
	void setValue(double value);
	void setValue(std::string value);
	void setValue(const char* value);
	//void setValue(File value);
	void setValue(const MixedData& value);
	
	void setValue(const STI::Types::TDataMixed& value);
	void setValue(const STI::Types::TDataMixedSeq& value);
	void setValue(const STI::Types::TFile& value);


	void clear();

	template<typename T> void addValue(T value)
	{
		if(type != Vector)
			convertToVector();

		values.push_back( MixedData(value) );
		
		//Homogeneous vectors are flat and have entries with all the same type. The type must not be a vector.
		homogeneous &= (values.back().getType() != Vector);

		if(values.size() > 1) {
			//See if all the types added to this vector are the same.
			//Compare most recent entry type to last entry.
			homogeneous &= (values.back().getType() == values.at( values.size() - 2 ).getType());
		}
	}

	MixedDataType getType() const;
	std::string getTypeString() const;

	bool isHomogeneous() const { return homogeneous; }

	bool getBoolean() const;
	unsigned char getOctet() const;
	int getInt() const;
	double getDouble() const;
	double getNumber() const;
	std::string getString() const;
	const STI::Types::TFile* getFile() const;
	const MixedDataVector& getVector() const;

	MixedData& getValueAt(unsigned i);

	const STI::Types::TDataMixed getTDataMixed() const;

	std::string print() const;

	template<typename T> MixedData operator +(const T& other) const
	{
		return (*this) + MixedData(other);
	}
	template<typename T> MixedData operator -(const T& other) const
	{
		return (*this) - MixedData(other);
	}
	template<typename T> MixedData operator *(const T& other) const
	{
		return (*this) * MixedData(other);
	}
	template<typename T> MixedData operator /(const T& other) const
	{
		return (*this) / MixedData(other);
	}

	MixedData operator +(const MixedData& other) const;
	MixedData operator -(const MixedData& other) const;
	MixedData operator *(const MixedData& other) const;
	MixedData operator /(const MixedData& other) const;


	MixedData sqroot();

private:

	void convertToVector();
	void printSetValueError();

	MixedDataVector values;
	
	MixedDataType type;
	
	bool				value_b;
	unsigned char		value_o;
	int					value_i;
	double				value_d;
	std::string			value_s;
	STI::Types::TFile   value_file;

	bool homogeneous;

};

#endif
