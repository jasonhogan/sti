/*! \file RCSTipTiltZ.h
 *  \author Susannah Dickerson
 *  \brief Include-file for the class RCSTipTiltZ
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

#ifndef RCSTIPTILTZ_H
#define RCSTIPTILTZ_H

#include <STI_Device.h>
#include <math.h>
#include "VectorAlgebra.h"

class RCSTipTiltZ
{
public:
	RCSTipTiltZ();
	~RCSTipTiltZ();

	double calculatetheta(std::vector <double> &axesPos, bool &error);	//Standard theta
	double calculatephi(std::vector <double> &axesPos, bool &error);	//Standard phi
	double calculatez(std::vector <double> &axesPos, bool &error);		//Height of plane at x = y = 0

	double calculateNanopositionerHeight(std::vector <double> &angles, double Ax, double Ay, bool &error);
	double calculateX(std::vector <double> &angles, bool &error);
	double calculateY(std::vector <double> &angles, bool &error);		
	double calculateZ(std::vector <double> &angles, bool &error);

private:
	double pi;
	double radToDeg;	// radians to degrees conversion factor
	double degToRad;	// degress to radians conversion factor

	// Geometry
	double radius;		//radius of circle on which nanopositinoers are positioned
	double height;			//height of mirror above nanopositioners; ~3"
	double Xx, Xy, Yx, Yy, Zx, Zy; // x and y positions of the positioners


	bool normalVector(std::vector <double> &axesPos, std::vector <double> &normal);
	void loadXYZ(std::vector <double> &axesPos, std::vector <double> &x, std::vector <double> &y, std::vector <double> &z);

};

#endif