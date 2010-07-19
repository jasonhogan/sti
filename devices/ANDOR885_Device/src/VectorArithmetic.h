/*! \file VectorArithmetic.h
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

#ifndef VECTORARITHMETIC_H
#define VECTORARITHMETIC_H

#include <math.h>
#include <vector>

namespace VectorArithmetic
{

	template<typename T> std::vector <double> divide(std::vector <T>& a, std::vector <T>& b, bool &error)
	{
		std::vector <double> vOut;
		error = false;

		unsigned int i;

		if (a.empty() || b.empty() || a.size() != b.size()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				vOut.push_back((double) a.at(i)/ (double) b.at(i));
			}
		}

		return vOut;
	}

	template<typename T> std::vector <double> times(std::vector <T>& a, std::vector <T>& b, bool &error)
	{
		std::vector <double> vOut;
		error = false;

		unsigned int i;

		if (a.empty() || b.empty || a.size() != b.size()) {
			error = true;
		}
		else {
			for (i = 0; i < a.size(); i++)
			{
				vOut.push_back((double) a.at(i) * (double) b.at(i));
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

	template<typename T> std::vector <double> minus(std::vector <T>& a, std::vector <T>& b, bool &error)
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
				vOut.push_back((double)a.at(i) - (double)b.at(i));
			}
		}

		return vOut;
	}

	template<typename T> std::vector <double> plus(std::vector <T>& a, std::vector <T>& b, bool &error)
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
				vOut.push_back((double)a.at(i) + (double)b.at(i));
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