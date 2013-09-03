/*! \file PcoEdgeCamera.hpp
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

#ifndef PcoEdgeCamera_H
#define PcoEdgeCamera_H


#include <windows.h>
#include <string>
#include <sc2_SDKStructures.h>

#define EBUFS 400
#define PCOEDGE_ERROR_GENERIC -1
#define PCOEDGE_ERROR_NONE 0

typedef struct
{
  WORD *pic12;                         // original image pointer
  SYSTEMTIME sTime;                    // shows the exact time stamp of the image  // 20 byte
  int        iTicks;                   // milliseconds gone after start in stime
  int        iXRes;                    // X Resolution
  int        iYRes;                    // Y Resolution                             // 32 byte
  char cText[40];                      // text which should be placed inside the image// 72 byte
  bool       bDouble;                  // shows if pic is Doubleshutter image
  bool       bDummy[3];                // since bool is only one byte, we've to fill up to four bytes// 76 byte
  int        iBWMin;                   // Lut bw min                               // 80 byte
  int        iBWMax;                   // Lut bw max
  int        iBWLut;                   // Lut lin log
  int        iRMin;                    // red min                                  // 92 byte
  int        iRMax;                    // red max
  int        iGMin;                    // green min                                // 100 byte
  int        iGMax;                    // green max
  int        iBMin;                    // blue min
  int        iBMax;                    // blue max                                 // 112 byte
  int        iColLut;                  // Lut lin log color
  int        iColor;                   // image from Color-CCD: 1 otherwise 0      // 120 byte
  int        iVersion;                 // Version of b16 extended info
  int        iBWMin2;                   // Lut bw min
  int        iBWMax2;                   // Lut bw max                              // 132 byte
  int        iBWLut2;                   // Lut lin log
  int        iRMin2;                    // red min                                 // 140 byte
  int        iRMax2;                    // red max
  int        iGMin2;                    // green min
  int        iGMax2;                    // green max                               // 152 byte
  int        iBMin2;                    // blue min
  int        iBMax2;                    // blue max                                // 160 byte
  int        iColLut2;                  // Lut lin log color
  bool       bAlignUpper;               // Align MSB (0-MSB is bit14, 1-MSB is bit 16)
  bool       bDummy2[3];                // since bool is only one byte, we've to fill up to four bytes // 168 byte
  double     dGammaLut;                 // Gamma value b/w
  double     dGammaLutC;                // Gamma value color
  double     dGammaLut2;                // Gamma value b/w 2
  double     dGammaLutC2;               // Gamma value color 2                     // 200 byte
  int        iColorPatternType;         // Demosaicking type for the color pattern
  int        iBitRes;                   // Bit resolution of image                 // 208 byte
  double     dSaturation;               // Color saturation common for both ds images // 216 byte
}Bild;// ACHTUNG: noch 172 Bytes frei, sonst muss headerlen in file12 angepasst werden!
//   Headerlen         = 512
// - alter Header      = 128
// - Bild (ohne WORD*) = 212
// ergibt freie bytes  = 172


struct PcoEdge_ROI {
	short x1;
	short x2;
	short y1;
	short y2;
};

class PcoEdgeCamera
{
public:
	PcoEdgeCamera();
	~PcoEdgeCamera();
	void PcoEdgeCamera::printErrors();
	void cameraStats();

	int takePicture(std::string filename);
	// TODO: Wrapper...
	int store_b16(const char *filename, int width, int height, void *buf, Bild *strBild);
	int storeTiff(std::string filename, int width, int height, void *buf, Bild *strBild);

	double getExposureMs();
	bool setExposureMs(double newExposure);

	bool initialized;

	short PcoEdgeCamera::getVbin();
	bool PcoEdgeCamera::setVbin(short vbinin);
	short PcoEdgeCamera::getHbin();
	bool PcoEdgeCamera::setHbin(short hbinin);
	bool PcoEdgeCamera::setRoi(short x1, short x2, short y1, short y2);
	PcoEdge_ROI PcoEdgeCamera::getRoi();
	void PcoEdgeCamera::getTemperature(short &ccdtemp, short &camtemp, short &powtemp);
	void PcoEdgeCamera::setSetpointTemp(short &setpoint);
	void PcoEdgeCamera::getSetpointTemp(short &setpoint);

	// Might be of public use:
	short PcoEdgeCamera::maxHorzRes();
	short PcoEdgeCamera::maxVertRes();

private:
	// Load the support libraries for the camera.
	// May load evil NI shizzle!
	int loadLibraries();
	double dwExposure;
	unsigned long npicstried;
	unsigned long npicsgood;
	short hbin, vbin;

	bool PcoEdgeCamera::setBin(short *toset, short val);
	bool PcoEdgeCamera::setBinning();

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

protected:

};

#endif
