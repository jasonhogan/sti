/*! \file RCSTipTiltZ.cpp
 *  \author Susannah Dickerson
 *  \brief calculates angles from rotation compensation system (RCS) nanopositioner positions
 *  \section license License
 *
 *  Copyright (C) 2010 Susannah Dickerson <sdickers@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu> and David M.S. Johnson <david.m.johnson@stanford.edu>
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



#include "RCSTipTiltZ.h"

RCSTipTiltZ::RCSTipTiltZ()
{
	pi = 3.141592;
	radToDeg = 180/pi;
	degToRad = pi/180;

	radius = 75000; //radius in micrometers
	height = 76200;		// height of mirror above nanopositioner contacts. (3")

	Xx = radius;
	Xy = 0;

	Yx = -radius/2;
	Yy = -sqrt(3.)*radius/2;

	Zx = -radius/2;
	Zy = sqrt(3.)*radius/2;
}

RCSTipTiltZ::~RCSTipTiltZ()
{

}

void RCSTipTiltZ::loadXYZ(std::vector <double> &axesPos, std::vector <double> &x, std::vector <double> &y, std::vector <double> &z)
{
	// x, y, and z are labelled clockwise. x lies on the x-axis

	x.push_back(Xx);
	x.push_back(Xy);
	x.push_back(axesPos.at(0));

	y.push_back(Yx);
	y.push_back(Yy);
	y.push_back(axesPos.at(1));

	z.push_back(Zx);
	z.push_back(Zy);
	z.push_back(axesPos.at(2));
}

double RCSTipTiltZ::calculatetheta(std::vector <double> &axesPos, bool &error)
{
	std::vector <double> x;
	std::vector <double> y;
	std::vector <double> z;

	std::vector <double> normal;
	std::vector <double> normalZ;
	error = false;

	double th = 0;

	if (axesPos.size() != 3) {
		error = true;
	}
	else {

		loadXYZ(axesPos, x, y, z);

		normalZ.push_back(0);
		normalZ.push_back(0);
		normalZ.push_back(1);
		
		error = normalVector(axesPos, normal);
		/* Crazy! this doesn't work because the normal ends up rounding normal.at(2) to 1 due to precision limitations)

		if (!error)
			th = sqrt(2.)*sqrt(1-normal.at(2));
		*/

		if (!error)
			th = VectorAlgebra::normFactor(VectorAlgebra::cross(normal, normalZ, error), error);
	}

	return th;
}

double RCSTipTiltZ::calculatephi(std::vector <double> &axesPos, bool &error)
{
	std::vector <double> x;
	std::vector <double> y;
	std::vector <double> z;

	std::vector <double> normal;
	std::vector <double> zprime;
	std::vector <double> rAxis; //rotation axis

	error = false;

	double ph = 0;

	if (axesPos.size() != 3) {
		error = true;
	}
	else {

		loadXYZ(axesPos, x, y, z);

		error = normalVector(axesPos, normal);
		if (normal.at(0) == 0) {
			if (normal.at(1) > 0 )
				ph = pi/2;
			else if (normal.at(1) < 0)
				ph = -pi/2;
			else
				ph = 0;
		} else if (normal.at(0) < 0) {
			ph = pi + atan(normal.at(1)/normal.at(0));
		} else {
			ph = atan(normal.at(1)/normal.at(0));
		}
	}

	return ph;
}

double RCSTipTiltZ::calculatez(std::vector <double> &axesPos, bool &error)
{
	std::vector <double> x;
	std::vector <double> y;
	std::vector <double> z;

	std::vector <double> normal;

	error = false;

	double zOut = 0;

	if (axesPos.size() != 3) {
		error = true;
	}
	else {

		loadXYZ(axesPos, x, y, z);

		//Find height at center with [n.(r - r0) = 0] => [zOut = n.r0 / n_z]
		
		error = normalVector(axesPos, normal);

		if (!error)
			zOut = VectorAlgebra::dot(normal,y, error)/normal.at(2); // Choose Y because it is the point contact

		// add to this the error in the height of the mirror.
		if (!error)
			zOut += height*pow(calculatetheta(axesPos, error),2) / 2;
	
	}

	return zOut;

}

double RCSTipTiltZ::calculateNanopositionerHeight(std::vector <double> &angles, double Ax, double Ay, bool &error)
{
//	std::vector <double> zeros(3,0);
	double zTemp;

	std::vector <double> normal;
	std::vector <double> center;
	std::vector <double> anglesOnly;

	error = false;

	double out = 0;

	// Angles is a vector of (theta, phi, z)

	if (angles.size() != 3) {
		error = true;
	}
	else {

		//Find height at positioner with [n.(r - r0) = 0] => [out = (n.r0 - nx*x - ny*y) / n_z]

		zTemp = angles.back();
		anglesOnly.push_back(angles.at(0));
		anglesOnly.push_back(angles.at(1));
		error = normalVector(anglesOnly, normal);

		// Vector of the center point. 
		// Be sure to subtract the correction added due to the height of the mirror from the nanopositioners.
		center.push_back(0);
		center.push_back(0);
		center.push_back(zTemp - height*pow(anglesOnly.at(0),2) / 2);

		if (!error)
			out = (VectorAlgebra::dot(normal, center, error) - normal.at(0)*Ax - normal.at(1)*Ay)/normal.at(2);
	}

	return out;

}

double RCSTipTiltZ::calculateX(std::vector <double> &angles, bool &error)
{
	error = false;

	double out = 0;

	if (angles.size() != 3) {
		error = true;
	}
	else {

		out = calculateNanopositionerHeight(angles, Xx, Xy, error);
	}

	return out;
}
double RCSTipTiltZ::calculateY(std::vector <double> &angles, bool &error)	
{
	error = false;

	double out = 0;

	if (angles.size() != 3) {
		error = true;
	}
	else {

		out = calculateNanopositionerHeight(angles, Yx, Yy, error);
	}

	return out;
}

double RCSTipTiltZ::calculateZ(std::vector <double> &angles, bool &error)
{
	error = false;

	double out = 0;

	if (angles.size() != 3) {
		error = true;
	}
	else {

		out = calculateNanopositionerHeight(angles, Zx, Zy, error);
	}

	return out;
}

bool RCSTipTiltZ::normalVector(std::vector <double> &axesPos, std::vector <double> &normal)
{
	bool error = false;

	if (axesPos.size() == 3) {
		std::vector <double> x;
		std::vector <double> y;
		std::vector <double> z;

		loadXYZ(axesPos, x, y, z);

		normal = VectorAlgebra::normalize(VectorAlgebra::cross(
			VectorAlgebra::minus(z,x,error),VectorAlgebra::minus(y,x,error),error),error);

	}
	else if (axesPos.size() == 2) {
		double t = axesPos.at(0);	// theta
		double p = axesPos.at(1);	// phi

		normal.clear();
		normal.push_back(sin(t)*cos(p));
		normal.push_back(sin(t)*sin(p));
		normal.push_back(cos(t));
		
	}
	else {
		error = true;
	}
	return error;

}