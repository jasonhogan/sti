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
	// TODO: Wrapper...
	//int store_b16(const char *filename, int width, int height, void *buf, Bild *strBild);
	int storeTiff(std::string filename, int width, int height, void *buf);
	
	double getExposureMs();
	
	bool setExposureMs(double newExposure);

	bool initialized;
	
	short CameraAbs::getVbin();
	bool CameraAbs::setVbin(short vbinin);
	short CameraAbs::getHbin();
	bool CameraAbs::setHbin(short hbinin);
	bool CameraAbs::setRoi(short x1, short x2, short y1, short y2);
	
	void CameraAbs::commitAttributes();
	/*
	//PcoEdge_ROI CameraAbs::getRoi();
	//void CameraAbs::getTemperature(short &ccdtemp, short &camtemp, short &powtemp);
	//void CameraAbs::setSetpointTemp(short &setpoint);
	//void CameraAbs::getSetpointTemp(short &setpoint);

	// Might be of public use:
	//short CameraAbs::maxHorzRes();
	//short CameraAbs::maxVertRes();
	*/
private:
	PCOCamera *pcocam;
	PCOBuffer *pcobuf;
	
	int bpp;
	int height;
	int width;

	void CameraAbs::adjustBuffersize();
	/*
	// Load the support libraries for the camera.
	// May load evil NI shizzle!
	int loadLibraries();
	double dwExposure;
	unsigned long npicstried;
	unsigned long npicsgood;
	short hbin, vbin;

	bool CameraAbs::setBin(short *toset, short val);
	bool CameraAbs::setBinning();

	char errbuffer[EBUFS];

	// These (plus others...) were previously globals:
	short roix1, roix2, roiy1, roiy2;
	HINSTANCE SC2Lib;
	HANDLE hCamera;
	int err;
	PCO_Description caminfo;
	PCO_CameraType strCamType;
	WORD wStorageMode;
	unsigned short recstate;
	WORD expbase;
	SHORT sBufNr;
	DWORD imgsize;
	DWORD bufsize;
	WORD* wBuf;
	WORD trigger_result;

	WORD wCameraBusyState;
	WORD wActSeg;
	DWORD dwStatusDll;
	DWORD dwStatusDrv;
	DWORD dwValidImageCnt;
	DWORD dwMaxImageCnt;
	WORD wXResAct; // Actual X Resolution
	WORD wYResAct; // Actual Y Resolution
	WORD wXResMax; // Maximum X Resolution
	WORD wYResMax; // Maximum Y Resolution
	short ccdtemp, camtemp, powtemp;
	HANDLE hEvent;
	*/
protected:

};
