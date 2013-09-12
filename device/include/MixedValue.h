/*! \file MixedValue.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class MixedValue
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

#ifndef STI_UTILS_MIXEDVALUE_H
#define STI_UTILS_MIXEDVALUE_H

#include <vector>
#include <string>
#include <iostream>

//#include <orbTypes.h>

namespace STI
{
namespace Utils
{

enum MixedValueType {Boolean, Octet, Int, Double, String, File, Vector, Empty, Unknown};

class MixedValue;

typedef std::vector<MixedValue> MixedValueVector;


//
//////////////////////////////
//
//class MixedValue // abc
//{
//	template<typename T>
//	virtual void setValue(const T& value) = 0;
//
//	template<typename T> 
//	virtual bool getValue(T& value) = 0;
//
//};
//
//class LocalMixedValue : public MixedValue
//{
//	void setValue(const double& value) { value_d = value; setType(Double);}
//	
//	bool getValue(const double& value) { 
//		if(isType(Double)) {
//			value = value_d;
//			return true;
//		}
//		return false;
//	}
//
//	double value_d;
//};
//
//class NetworkMixedValue : public LocalMixedValue
//{
//	void setValue(const TValMixed& value) {}
//	void setValue(const MixedValue& value) {}
//};



////////////////////////////

//class MixedValueHolderBase
//{
//	template<typename T> 
//	virtual void setValue(const T& value) = 0;
//	
//	template<typename T> 
//	virtual void getValue(T& value) = 0;
//};
//
//template<typename T> 
//class MixedValueHolder : public MixedValueHolderBase
//{
//public:
//
//	MixedValueHolder(const T& any) { setValue(any); }
//
//	virtual void setValue(const T& value) { val = value; }
//	virtual void getValue(T& value) { value = val; }
//
//private:
//	T val;
//};


class MixedValue
{
public:
	
	MixedValue();	//for std::vector
	template<typename T> MixedValue(const T& value)
	{
		setValue(value);
	}	
	MixedValue(const MixedValue& copy);

	~MixedValue();

	template<typename T> MixedValue& operator= (const T& other)
	{
		setValue(other);
		return (*this);
	}

	template<typename T> bool operator==(const T& other) const
	{
		return (*this) == MixedValue(other);
	}
	template<typename T> bool operator!=(const T& other) const
	{
		return (*this) != MixedValue(other);
	}
	template<typename T> bool operator>(const T& other) const
	{
		return (*this) > MixedValue(other);
	}
	template<typename T> bool operator<(const T& other) const
	{
		return (*this) < MixedValue(other);
	}

	bool operator==(const MixedValue& other) const;
	bool operator!=(const MixedValue& other) const;
	bool operator<(const MixedValue& other) const;
	bool operator>(const MixedValue& other) const;

	//virtual everything below this and then
	//class RemoteMixedValue : public MixedValue
	//overrides and uses a remote mixed type to avoid reallocation?
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

		for(unsigned i = 0; i < value.size(); i++) {
			addValue( value.at(i) );
		}
	}
	void setValue(bool value);
	void setValue(unsigned char value);
	void setValue(int value);
	void setValue(double value);
	void setValue(const std::string& value);
	void setValue(const MixedValue& value);
	void setValue();	//Empty
	void setValue(const char* value) { setValue(std::string(value)); }
	void setValue(char* value) { setValue(std::string(value)); }
	void setValue(short value) { setValue(static_cast<int>(value)); }
	void setValue(unsigned short value) { setValue(static_cast<int>(value)); }
	void setValue(unsigned value) { setValue(static_cast<int>(value)); }

//	void setValue(const STI::Utils::MixedValue_ptr& value);

	void clear();

	template<typename T> void addValue(T value)
	{
		if(type != Vector) {
			convertToVector();
		}
		values.push_back( MixedValue(value) );
	}

	MixedValueType getType() const;

	// {Boolean, Octet, Int, Double, String, File, Vector, Empty}
	bool getBoolean() const;
	unsigned char getOctet() const;
	int getInt() const;
	double getDouble() const;
	double getNumber() const;
	std::string getString() const;
	const MixedValueVector& getVector() const;

	std::string print() const;

	//template<typename T> MixedValue operator +(const T& other) const
	//{
	//	return (*this) + MixedValue(other);
	//}
	//template<typename T> MixedValue operator -(const T& other) const
	//{
	//	return (*this) - MixedValue(other);
	//}
	//template<typename T> MixedValue operator *(const T& other) const
	//{
	//	return (*this) * MixedValue(other);
	//}
	//template<typename T> MixedValue operator /(const T& other) const
	//{
	//	return (*this) / MixedValue(other);
	//}

	//MixedValue operator +(const MixedValue& other) const;
	//MixedValue operator -(const MixedValue& other) const;
	//MixedValue operator *(const MixedValue& other) const;
	//MixedValue operator /(const MixedValue& other) const;

private:

	void convertToVector();

	MixedValueVector values;
	
	MixedValueType type;
	
	bool        value_b;
	unsigned char value_o;
	int         value_i;
	double      value_d;
	std::string value_s;

};



//class MixedValue
//{
//	void setValue(double* value, unsigned length)
//	{
//		value = value;
//	}
//	
//	const MixedValueVector& getVector() const;
//
//	MixedValue(double* value);
//
//	MixedValue& at(unsigned i)
//	{
//		if(i > length)
//			return MixedValue();
//		return MixedValue(value_da[i]);
//	}
//
//	double* value_da;
//};

}
}


#endif
