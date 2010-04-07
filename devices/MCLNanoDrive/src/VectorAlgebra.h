/*! \file VectorAlgebra.h
 *  \author Susannah Dickerson
 *  \brief Include-file for the class VectorAlgebra
 *  \section license License
 *
 *  Copyright (C) 2010 Susannah Dickerson <sdickers@stanford.edu>\n
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

#ifndef VECTORALGEBRA_H
#define VECTORALGEBRA_H

#include <math.h>
#include <vector>

namespace VectorAlgebra
{
	template<typename T> std::vector <T> cross(std::vector <T>& a, std::vector <T>& b, bool &error)
	{
		std::vector <T> vOut;
		error = false;

		if (a.size() != 3 || b.size() != 3)
		{
			error = true;
		} else {

			vOut.clear();

			vOut.push_back(a.at(1)*b.at(2)-a.at(2)*b.at(1));
			vOut.push_back(a.at(2)*b.at(0)-a.at(0)*b.at(2));
			vOut.push_back(a.at(0)*b.at(1)-a.at(1)*b.at(0));
		}

		return vOut;
	}


	template<typename T> T dot(std::vector <T>& a, std::vector <T>& b, bool &error)
	{
		unsigned int i;
		T outValue;

		outValue = 0;
		error = false;

		if (a.empty() || b.empty() || a.size() != b.size()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				outValue += a.at(i)*b.at(i);
			}
		}

		return outValue;
	}

	template<typename T> std::vector <double> divide(std::vector <T>& a, T& b, bool &error)
	{
		std::vector <double> vOut;
		error = false;

		unsigned int i;

		if (a.empty()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				vOut.push_back((double) a.at(i)/ (double) b);
			}
		}

		return vOut;
	}

	template<typename T> std::vector <double> times(std::vector <T>& a, T& b, bool &error)
	{
		std::vector <double> vOut;
		error = false;

		unsigned int i;

		if (a.empty()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				vOut.push_back((double) a.at(i) * (double) b);
			}
		}

		return vOut;
	}

	template<typename T> std::vector <T> minus(std::vector <T>& a, std::vector <T>& b, bool &error)
	{
		std::vector <T> vOut;
		error = false;

		unsigned int i;

		if (a.empty()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				vOut.push_back(a.at(i) - b.at(i));
			}
		}

		return vOut;
	}

	template<typename T> std::vector <T> plus(std::vector <T>& a, std::vector <T>& b, bool &error)
	{
		std::vector <T> vOut;
		error = false;

		unsigned int i;

		if (a.empty()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				vOut.push_back(a.at(i) + b.at(i));
			}
		}

		return vOut;
	}

	template<typename T> std::vector <double> normalize(std::vector <T>& a, bool &error)
	{
		std::vector <double> vOut;
		double normF;
		error = false;

		if (a.empty()) {
			error = true;
		}
		else {
			normF = normFactor(a,error);
			vOut = divide(toDouble(a),normF,error);
		}

		return vOut;
	}

	template<typename T> double normFactor(std::vector <T>& a, bool &error)
	{
		double outValue;

		outValue = 0;
		error = false;

		if (a.empty()) {
			error = true;
		}
		else {
			outValue = sqrt((double) dot(a,a,error));
		}

		return outValue;
	}

	template<typename T> T total(std::vector <T>& a, bool &error)
	{
		T outValue;
		unsigned int i;

		outValue = 0;
		error = false;

		if (a.empty()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				outValue += a.at(i);
			}
		}

		return outValue;
	}

	template<typename T> double mean(std::vector <T>& a, bool &error)
	{
		double outValue;

		outValue = 0;
		error = false;

		if (a.empty()) {
			error = true;
		}
		else {
			outValue = (double)total(a, error);
			outValue /= a.size();
		}

		return outValue;
	}

	template<typename T> std::vector <double> midpoint(std::vector <T>& a, std::vector <T>& b, bool &error)
	{
		std::vector <double> vOut;
		unsigned int i;

		error = false;

		if (a.empty() || b.empty() || a.size() != b.size()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				vOut.push_back(((double) a.at(i) + (double) b.at(i)) / 2);
			}
		}

		return vOut;
	}

	template<typename T> std::vector <double> toDouble(std::vector <T>& a)
	{
		std::vector <double> vOut;
		unsigned int i;

		for (i = 0; i < a.size(); i++)
		{
			vOut.push_back((double) a.at(i));
		}

		return vOut;
	}
};

#endif