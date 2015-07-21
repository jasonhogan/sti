/*! \file CameraAbs.hpp
 *  \author Brannon Klopfer
 *  \brief Source-file for the class GenericDevice
 *  \section license License
 *
 *  Copyright (C) 2012 Brannon Klopfer <bbklopfer@stanford.edu>
 *  Code inspired by demo.cpp from PCO
 *
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

#include "PCO_SencamSDK.h"

class CameraAbs
{
public:
	CameraAbs();
	~CameraAbs();

	
	void CameraAbs::printErrors();
	//void cameraStats();

	int takePicture(std::string filename);
	int photonCount(std::string filename, int npics);
	// TODO: Wrapper...
	//int store_b16(const char *filename, int width, int height, void *buf, Bild *strBild);
	int storeTiff(std::string filename, PCOBuffer* pcobuf);
	
	double getExposureMs();
	
	bool setExposureMs(double newExposure);
	
	bool initialized;
	
	short CameraAbs::getVbin();
	bool CameraAbs::setVbin(short vbinin);
	short CameraAbs::getHbin();
	bool CameraAbs::setHbin(short hbinin);
	bool CameraAbs::setRoi(short x1, short x2, short y1, short y2);
	
	int CameraAbs::getGain();
	int CameraAbs::setGain(int gain);


	void CameraAbs::commitAttributes();

	int singlePhotonThresh;
	int singlePhotonRad;

private:
	PCOCamera *pcocam;
	PCOBuffer *pcobuf;

	int bpp;
	int height;
	int width;

	void CameraAbs::adjustBuffersize();

protected:

};
