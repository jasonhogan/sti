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
#include <iostream>
#include <orbTypes.h>

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

	enum MixedDataType {Boolean, Octet, Int, Double, String, Picture, File, Vector, Empty};

	template<typename T> void setValue(T value)
	{
		//This version of the function is call for all T values that are unsupported.
		//This template is called for all types that don't have an explicitly overloaded setValue function.

		std::cout << "Error: Unsupported type was passed to the MixedValue template constructor." << std::endl;
	}

	template<typename T> void setValue(const std::vector<T>& value)
	{
		clear();
		type = Vector;

		for(unsigned i = 0; i < value.size(); i++)
		{
			addValue( value.at(i) );
		}
	}
	
	void setValue(bool value);
	void setValue(unsigned char value);
	void setValue(int value);
	void setValue(double value);
	void setValue(std::string value);
	//void setValue(File value);
	void setValue(const MixedData& value);
	
	void setValue(const STI::Types::TDataMixed& value);
	void setValue(const STI::Types::TDataMixedSeq& value);




	void clear();

	template<typename T> void addValue(T value)
	{
		if(type != Vector)
			convertToVector();

		values.push_back( MixedData(value) );
	}

	MixedDataType getType() const;
	
	bool getBoolean() const;
	unsigned char getOctet() const;
	int getInt() const;
	double getDouble() const;
	double getNumber() const;
	std::string getString() const;
//	TFile getFile() const;
	const MixedDataVector& getVector() const;
	
	const STI::Types::TDataMixed getTDataMixed() const;

	std::string print() const;

private:

	void convertToVector();

	MixedDataVector values;
	
	MixedDataType type;
	
	bool				value_b;
	unsigned char		value_o;
	int					value_i;
	double				value_d;
	std::string			value_s;



};

#endif
