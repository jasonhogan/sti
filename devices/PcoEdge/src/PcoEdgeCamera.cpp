/*! \file PcoEdgeCamera.cpp
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

/* This should contain the code to talk to the camera.
 *
 * This does should not care about anything STI-related. */


#include <stdio.h>
#include <string>
#include <ORBManager.h>
#include <Magick++.h>
#include <iostream>
#include "PcoEdgeCamera.hpp"
#include "load.h"
#include <sc2_SDKStructures.h>
#include <sc2_defs.h>
#include <PCO_err.h>
#include <PCO_errt.h>

#define DEBUG(str) //std::cerr << str << std::endl;

#define PCO_ERRT_H_CREATE_OBJECT

#pragma pack (4)
#define FILEVERSION302 302
#define HEADERLEN 128

typedef struct
{
  char ucPco[4];
  unsigned int uiFileLen;
  unsigned int uiHeaderLen;
  unsigned int uiXRes;
  unsigned int uiYRes;
  unsigned int uiLutSign;
  unsigned int uiColor;
  unsigned int uiBWMin;
  unsigned int uiBWMax;
  unsigned int uiBWLut;
  unsigned int uiRMin;
  unsigned int uiRMax;
  unsigned int uiGMin;
  unsigned int uiGMax;
  unsigned int uiBMin;
  unsigned int uiBMax;
  unsigned int uiColLut;
  unsigned int uiDS;
  unsigned int uiDummy[HEADERLEN-18];
}B16_HEADER;
#pragma pack ()

#define NOFILE	-100

using namespace std;

// Mine:
#define MAX_WAIT_FOR_BUFFER_TRIES 100
#define BUF_SLEEP_TIME 30

PcoEdgeCamera::PcoEdgeCamera()
{
	// Housekeeping
	initialized = false;

	// Keep track of number of pictures attempted vs. (nominally) completed
	npicstried = 0;
	npicsgood = 0;

	// TODO: How much of this needs to be initialized?
	bool bbufferextern = TRUE;
	SC2Lib = NULL;
	wXResAct = 640; // Actual X Resolution
	wYResAct = 480; // Actual Y Resolution
	HANDLE hEvent = NULL;
	hbin = vbin = 1;


	// TODO: DECOMP!
	dwExposure = 10;

	/************************************************************
	load SC2 library; please note SC2_Cam.dll has dependency on 
	sc2_1394.dll, this dll must be present in your dll search path.
	************************************************************/

	DEBUG("Constructor for PcoEdgeCamera");
	err = loadLibraries();
	if (err) {
		sprintf_s(errbuffer, EBUFS, "\nCan not load SC2 library. Error: %d\n", err);
		return;
	}

	DEBUG("Libraries loaded");

	err = OpenCamera(&hCamera, 0);
	if (err) {
		DEBUG("Looks like there was an error opening duder...");
		sprintf_s(errbuffer, EBUFS, "\nPCO_OpenCamera error (hex): %lx\n", err);
		return;
	}

	DEBUG("Camera opened.");

	err = GetSizes(hCamera, &wXResAct, &wYResAct, &wXResMax, &wYResMax);
	if (err) {
		sprintf_s(errbuffer, EBUFS, "\nPCO_GetSizes error(hex): %lx\n", err);
		return;
	}

	DEBUG("Image Resolution = " << wXResAct << "x" << wYResAct);

	/*********************************************************
	wSize of any defined structure must be filled before used 
	in funciton call.
	**********************************************************/

	strCamType.wSize = sizeof(strCamType);
	err = GetCameraType(hCamera, &strCamType);
	if (err != 0) {
		sprintf_s(errbuffer, EBUFS, "\nPCO_GetCameraType error (hex): %lx\n", err);
		return;
	}

	caminfo.wSize = sizeof(PCO_Description);
	if (GetCameraDescription(hCamera, &caminfo)) {
		sprintf_s(errbuffer, EBUFS, "\nPCO_GetCameraDescription error (hex): %lx\n", err);
		return;
	}

	// Set up some basic parameters:
	// Note that the STI device will probably reset all of these.
	setExposureMs(10);
	roix1 = 0;
	roix2 = 0;
	roiy1 = 0;
	roiy2 = 0;
	// setting the binning calls setRoi()
	setBinning();

	// We should be able to take pictures now
	initialized = true;
}

// TODO: Verify that this actually works...
void PcoEdgeCamera::printErrors()
{
	DEBUG("errbuffer says:");
	printf(errbuffer);
	printf("Refer to PCO_err.h for explanation of some error codes\n");
	cerr << "You should probably kill the program, lest you leak memory" << endl;

}
PcoEdgeCamera::~PcoEdgeCamera()
{
	// Clean up...
	DEBUG("DESTRUCTOR YO!");
	char errbuffer[EBUFS];
	printf("\r\nNow we produce an error to show the function PCO_GetErrorText.\r\n");
	err = CloseCamera(&hCamera);// This call produces an error!
	if (err != 0) {
		PCO_GetErrorText(err, errbuffer, 400);

		printf("Here you can see the error explanation:\r\n%s", errbuffer);
		err = CloseCamera(hCamera);// Correct code...
	}

	printf("\r\nClosing program without error.");
	free(wBuf);
	DEBUG("RETURN 0");
	return;
}

PcoEdge_ROI PcoEdgeCamera::getRoi()
{
	PcoEdge_ROI roi;

	roi.x1 = roix1;
	roi.x2 = roix2;
	roi.y1 = roiy1;
	roi.y2 = roiy2;

	return roi;
}

short PcoEdgeCamera::maxHorzRes()
{
	short tmp;

	if(caminfo.wRoiHorStepsDESC > 0) {
		tmp = caminfo.wMaxHorzResStdDESC/hbin/caminfo.wRoiHorStepsDESC;
		tmp *= caminfo.wRoiHorStepsDESC;
	} else {
		tmp = caminfo.wMaxHorzResStdDESC/hbin;
	}

	return tmp;
}

short PcoEdgeCamera::maxVertRes()
{
	short tmp;

	if(caminfo.wRoiVertStepsDESC > 0) {
		tmp = caminfo.wMaxVertResStdDESC/vbin/caminfo.wRoiVertStepsDESC;
		tmp *= caminfo.wRoiVertStepsDESC;
	} else {
		tmp = caminfo.wMaxVertResStdDESC/vbin;
	}

	return tmp;
}

// NB: Vertical ROI must be symmetric! (chop off same amount from top and bottom)
bool PcoEdgeCamera::setRoi(short x1, short x2, short y1, short y2)
{
	bool validRoi = true;
	/* 0,0 -> maximize, any other invalid ROI -> maximize but return error*/
	if (x1 < 1 || x2 < 1 || x1 >= x2 || x2 > maxHorzRes()) {
		if (x1 || x2)
			validRoi = false;

		roix1 = 1;
		roix2 = maxHorzRes();
	} else {
		roix1 = x1;
		roix2 = x2;
	}

	if (y1 < 1 || y2 < 1 || y1 >= y2 || y2 > maxVertRes()) {
		if (y1 || y2)
			validRoi = false;

		roiy1 = 1;
		roiy2 = maxVertRes();
	} else {
		roiy1 = y1;
		roiy2 = y2;
	}

	/* ROI is only settable in increments: */
	if (caminfo.wRoiHorStepsDESC > 1) {
		roix1 = (roix1-1)/caminfo.wRoiHorStepsDESC;
		roix1 *= caminfo.wRoiHorStepsDESC;
		roix1++;
		roix2 = std::ceil((double)roix2/caminfo.wRoiHorStepsDESC);
		roix2 *= caminfo.wRoiHorStepsDESC;
	}
	if (caminfo.wRoiVertStepsDESC > 1) {
		roiy1 = (roiy1-1)/caminfo.wRoiVertStepsDESC;
		roiy1 *= caminfo.wRoiVertStepsDESC;
		roiy1++;
		roiy2 = std::ceil((double)roiy2/caminfo.wRoiVertStepsDESC);
		roiy2 *= caminfo.wRoiVertStepsDESC;
	}

	// For the pco.edge, we must set symmetric vertical ROI
	// To do that, let's pick the smallest allowed ROI that
	// includes the specified ROI.
	if (roiy1 > (maxVertRes() - roiy2))
		roiy1 = (maxVertRes() - roiy2 + 1);
	else
		roiy2 = maxVertRes() - (roiy1 - 1);

	/* Write to camera */
	DEBUG("Attempting to set ROI: [" << roix1 << ", " << roix2 << "], [" << roiy1 << ", " << roiy2 << "]");
	err = SetROI(hCamera, roix1, roiy1, roix2, roiy2);
	if (err) {
		DEBUG("PCO_SetROI error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_SetROI error (hex): %lx\n", err);
		return false;
	}

	return validRoi;
}


// I have no idea if this is right. 
int PcoEdgeCamera::takePicture(string filename)
{
	Bild pic;
	char errbuffer[EBUFS];
	unsigned int uiBitMax;
	bool bbufferextern = TRUE;
	npicstried++;
	/***********************************************************
	The following step is a must. 
	In SC2, bin proceeds ROI in control.  
	ROI "field of definition" is subject to bin settings.
	Please be advised that this is opposite to SensiCam SDK
	*************************************************************/
	err = SetTriggerMode(hCamera, 0x0000);
	if (err != 0) {
		DEBUG("PCO_SetTriggerMode error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_SetTriggerMode error (hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	// recorder mode auto
	err = SetStorageMode(hCamera, 0);
	if (err) {
		DEBUG("PCO_SetStorageMode error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_SetStorageMode error (hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	// recorder submode ---- ring buffer
	err = SetRecorderSubmode(hCamera, 1);
	if (err) {
		DEBUG("PCO_SetRecorderSubmode error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_SetRecorderSubmode (hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	// all the images taken be stored
	err = SetAcquireMode(hCamera, 0);

	// set gain
	if (caminfo.wConvFactDESC[1]>0)
	{
		err = SetConversionFactor(hCamera, caminfo.wConvFactDESC[1]);
		if (err != 0) {
			DEBUG("PCO_SetDelayExposureTime error(hex)" << std::hex << err);
			sprintf_s(errbuffer, EBUFS, "\nPCO_SetDelayExposureTime (hex): %lx\n", err);
			return PCOEDGE_ERROR_GENERIC;
		}
	}
  
	/***********************************************************
	Cam Ram can be partitioned and set active. 
	by deafult, it is a single piece. An ID is returned
	*************************************************************/
	if((caminfo.dwGeneralCapsDESC1 & GENERALCAPS1_NO_RECORDER) == 0) {// camera has got recorder memory
		err = GetActiveRamSegment(hCamera, &wActSeg);
		if (err) {
			DEBUG("PCO_GetActiveRamSegment error(hex)" << std::hex << err);
			sprintf_s(errbuffer, EBUFS, "\nPCO_GetActiveRamSegment(hex): %lx\n", err);
			return PCOEDGE_ERROR_GENERIC;
		}
	}

	/***********************************************************
	ArmCamera validates settings.  
	recorder must be turned off to ArmCamera
	*************************************************************/

	err = GetRecordingState(hCamera, &recstate);
	if (err != 0) {
		DEBUG("PCO_GetRecordingState error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_GetRecordingState(hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	if (recstate>0) {
		err = SetRecordingState(hCamera, 0x0000);
		if (err != 0) {
				DEBUG("PCO_SetRecordingState error(hex)" << std::hex << err);
				sprintf_s(errbuffer, EBUFS, "\nPCO_SetRecordingState(hex): %lx\n", err);
				return PCOEDGE_ERROR_GENERIC;
			}
	}
	err = ArmCamera(hCamera);
	if (err != 0) {
		DEBUG("PCO_GetSizes error(hex) from arm: 0x" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_ArmCamera(hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	/***********************************************************
	GetSizes gets correct resolutions following ArmCamera.
	buffer is allocated accordingly
	*************************************************************/

	err = GetSizes(hCamera, &wXResAct, &wYResAct, &wXResMax, &wYResMax);
	if (err) {
		DEBUG("PCO_GetSizes error(hex)  from getsizes" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_GetSizes error(hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	DEBUG("Image Resolution = " << wXResAct << "x" << wYResAct);

	imgsize = wXResAct*wYResAct*2;
	bufsize = imgsize;
	if (bbufferextern) {
		if (bufsize % 0x1000) {
			bufsize = imgsize / 0x1000;
			bufsize += 2;
			bufsize *= 0x1000;
		} else bufsize += 0x1000;

		wBuf = (WORD*)malloc(bufsize);
	} else wBuf = NULL;

	sBufNr = -1;                         // -1 produces a new buffer

	DEBUG("malloc'd some buffers");

	err = AllocateBuffer(hCamera, &sBufNr, bufsize, &wBuf, &hEvent);
	if (err != 0) {
		DEBUG("PCO_AllocateBuffer error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_AllocateBuffer error(hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	err = CamLinkSetImageParameters(hCamera, wXResAct, wYResAct);
	if (err != 0) {
		DEBUG("PCO_CamLinkSetImageParameters error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_CamLinkSetImageParameters error(hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	// to turn recorder
	err = SetRecordingState(hCamera, 0x0001);
	err = GetCameraBusyStatus(hCamera, &wCameraBusyState);

	if (bbufferextern)
		err = AddBufferEx(hCamera, 0, 0, sBufNr, wXResAct, wYResAct, caminfo.wDynResDESC);
	else
		err = AddBuffer(hCamera, 0, 0, sBufNr);
	if (err) {
		DEBUG("PCO_AddBuffer error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_AddBuffer error(hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	int tmp = 0;
	bool bufferOk = false;
	do {
		bufferOk = true;
		Sleep(BUF_SLEEP_TIME);
		err = GetBufferStatus(hCamera, sBufNr, &dwStatusDll, &dwStatusDrv);
		if (err) {
			DEBUG("PCO_GetBufferStatus error(hex)" << std::hex << err);
			sprintf_s(errbuffer, EBUFS, "\nPCO_GetBufferStatus error(hex): %lx\n", err);
			bufferOk = false;

			return PCOEDGE_ERROR_GENERIC;
		}

		if (dwStatusDrv != 0)
			bufferOk = false;
	} while ((!(dwStatusDll & 0x00008000) || dwStatusDrv) && tmp++ < MAX_WAIT_FOR_BUFFER_TRIES);

	if (!bufferOk) {
		sprintf_s(errbuffer, EBUFS, "\nPCO_GetBufferStatus dwStatusDrv(hex): %lx\n", dwStatusDrv);
		DEBUG("UH-OH! Something's not right with dwStatusDrv!");
	}

	err = SetRecordingState(hCamera, 0x0000);
	/***********************************************************
	The image should be in wBuf->. 
	*************************************************************/
	if((caminfo.dwGeneralCapsDESC1 & GENERALCAPS1_NO_RECORDER) == 0) // camera has got recorder memory
	{
		err = GetNumberOfImagesInSegment(hCamera, wActSeg, &dwValidImageCnt, &dwMaxImageCnt);

		GetImage(hCamera, wActSeg, 1, 1, sBufNr);

		if (err) {
			DEBUG("PCO_Get error(hex)" << std::hex << err);
			sprintf_s(errbuffer, EBUFS, "\nPCO_GetNumberOfImagesInSegment error(hex): %lx\n", err);
			return PCOEDGE_ERROR_GENERIC;
		} // see demo.cpp for possible else statements
	}

	// TODO: decomp.
	memset(&pic.pic12, 0, sizeof(Bild));
	pic.bAlignUpper = 1;
	pic.bDouble = 0;
	sprintf_s(pic.cText, 40, "Demo");
	pic.iBitRes = caminfo.wDynResDESC;
	uiBitMax = (1 << pic.iBitRes) - 1;

	pic.iRMax = uiBitMax;
	pic.iRMax2 = uiBitMax;
	pic.iRMin = 0;
	pic.iRMin2 = 0;
	pic.iGMax = uiBitMax;
	pic.iGMax2 = uiBitMax;
	pic.iGMin = 0;
	pic.iGMin2 = 0;
	pic.iBMax = uiBitMax;
	pic.iBMax2 = uiBitMax;
	pic.iBMin = 0;
	pic.iBMin2 = 0;
	pic.dGammaLut = 1.0;
	pic.dGammaLut2 = 1.0;
	pic.dGammaLutC = 1.0;
	pic.dGammaLutC2 = 1.0;
	pic.dSaturation = 100;
	pic.iColLut = 0;
	pic.iColLut2 = 0;
	pic.iColor = 0;
	pic.iColorPatternType = 0;

	pic.iBWMin = 0;
	pic.iBWMin2 = 0;
	pic.iBWMax = uiBitMax;
	pic.iBWMax2 = uiBitMax;
	pic.iBWLut = 0;
	pic.iBWLut2 = 0;
	pic.iTicks = 0;
	pic.iXRes = wXResAct;
	pic.iYRes = wYResAct;
	pic.pic12 = wBuf;

	GetSystemTime(&pic.sTime);

	/***** Take the picture *****/
	err = FreeBuffer(hCamera, sBufNr);
	if (err) {
		DEBUG("PCO_FreeBuffer error(hex)" << std::hex << err);
		sprintf_s(errbuffer, EBUFS, "\nPCO_FreeBuffer error(hex): %lx\n", err);
		return PCOEDGE_ERROR_GENERIC;
	}

	DEBUG("Taking picture...");
	err = storeTiff(filename, wXResAct, wYResAct, wBuf, (Bild*)&pic.pic12);
	DEBUG("Took picture.");

	free(wBuf);
	if (!err) {
		DEBUG("Image stored as " << filename);
	} else {
		printf("Image not saved beacuse of file error. Probably an access rights problem.");
	}

	npicsgood++;

	DEBUG("RETURN 0");
	return PCOEDGE_ERROR_NONE;
}

// Load required pco libraries
int PcoEdgeCamera::loadLibraries(void)
{
	DWORD liberror;
	SC2Lib = LoadLibrary("SC2_Cam");
	if (SC2Lib == NULL)
	{
		liberror = GetLastError();
		return liberror;
	}

	DEBUG("Loaded SC2_Cam");

	if ((GetGeneral = (int(__stdcall *)(HANDLE ph, PCO_General *strGeneral))
	     GetProcAddress(SC2Lib, "PCO_GetGeneral")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetCameraType = (int(__stdcall *)(HANDLE ph, PCO_CameraType *strCamType))
	     GetProcAddress(SC2Lib, "PCO_GetCameraType")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetCameraHealthStatus = (int(__stdcall *)(HANDLE ph, DWORD* dwWarn, DWORD* dwErr, DWORD* dwStatus))
	     GetProcAddress(SC2Lib, "PCO_GetCameraHealthStatus")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((ResetSettingsToDefault = (int(__stdcall *)(HANDLE ph))
	     GetProcAddress(SC2Lib, "PCO_ResetSettingsToDefault")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((InitiateSelftestProcedure = (int(__stdcall *)(HANDLE ph))
	     GetProcAddress(SC2Lib, "PCO_InitiateSelftestProcedure")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetTemperature = (int(__stdcall *)(HANDLE ph, SHORT* sCCDTemp, SHORT* sCamTemp, SHORT* sPowTemp))
	     GetProcAddress(SC2Lib, "PCO_GetTemperature")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetSensorStruct = (int(__stdcall *)(HANDLE ph, PCO_Sensor *strSensor))
	     GetProcAddress(SC2Lib, "PCO_GetSensorStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetSensorStruct = (int(__stdcall *)(HANDLE ph, PCO_Sensor *strSensor))
	     GetProcAddress(SC2Lib, "PCO_SetSensorStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetCameraDescription = (int(__stdcall *)(HANDLE ph, PCO_Description *strDescription))
	     GetProcAddress(SC2Lib, "PCO_GetCameraDescription")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetSensorFormat = (int(__stdcall *)(HANDLE ph, WORD* wSensor))
	     GetProcAddress(SC2Lib, "PCO_GetSensorFormat")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetSensorFormat = (int(__stdcall *)(HANDLE ph, WORD wSensor))
	     GetProcAddress(SC2Lib, "PCO_SetSensorFormat")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetSizes = (int(__stdcall *)(HANDLE ph, WORD *wXResAct, WORD *wYResAct, WORD *wXResMax, WORD *wYResMax))
	     GetProcAddress(SC2Lib, "PCO_GetSizes")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetROI = (int(__stdcall *)(HANDLE ph, WORD *wRoiX0, WORD *wRoiY0, WORD *wRoiX1, WORD *wRoiY1))
	     GetProcAddress(SC2Lib, "PCO_GetROI")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetROI = (int(__stdcall *)(HANDLE ph, WORD wRoiX0, WORD wRoiY0, WORD wRoiX1, WORD wRoiY1))
	     GetProcAddress(SC2Lib, "PCO_SetROI")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetBinning = (int(__stdcall *)(HANDLE ph, WORD *wBinHorz, WORD *wBinVert))
	     GetProcAddress(SC2Lib, "PCO_GetBinning")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetBinning = (int(__stdcall *)(HANDLE ph, WORD wBinHorz, WORD wBinVert))
	     GetProcAddress(SC2Lib, "PCO_SetBinning")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetPixelRate = (int(__stdcall *)(HANDLE ph, DWORD *dwPixelRate))
	     GetProcAddress(SC2Lib, "PCO_GetPixelRate")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetPixelRate = (int(__stdcall *)(HANDLE ph, DWORD dwPixelRate))
	     GetProcAddress(SC2Lib, "PCO_SetPixelRate")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetConversionFactor = (int(__stdcall *)(HANDLE ph, WORD *wConvFact))
	     GetProcAddress(SC2Lib, "PCO_GetConversionFactor")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetConversionFactor = (int(__stdcall *)(HANDLE ph, WORD wConvFact))
	     GetProcAddress(SC2Lib, "PCO_SetConversionFactor")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetDoubleImageMode = (int(__stdcall *)(HANDLE ph, WORD *wDoubleImage))
	     GetProcAddress(SC2Lib, "PCO_GetDoubleImageMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetDoubleImageMode = (int(__stdcall *)(HANDLE ph, WORD wDoubleImage))
	     GetProcAddress(SC2Lib, "PCO_SetDoubleImageMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetADCOperation = (int(__stdcall *)(HANDLE ph, WORD *wADCOperation))
	     GetProcAddress(SC2Lib, "PCO_GetADCOperation")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetADCOperation = (int(__stdcall *)(HANDLE ph, WORD wADCOperation))
	     GetProcAddress(SC2Lib, "PCO_SetADCOperation")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetIRSensitivity = (int(__stdcall *)(HANDLE ph, WORD *wIR))
	     GetProcAddress(SC2Lib, "PCO_GetIRSensitivity")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetIRSensitivity = (int(__stdcall *)(HANDLE ph, WORD wIR))
	     GetProcAddress(SC2Lib, "PCO_SetIRSensitivity")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetCoolingSetpointTemperature = (int(__stdcall *)(HANDLE ph, SHORT *sCoolSet))
	     GetProcAddress(SC2Lib, "PCO_GetCoolingSetpointTemperature")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetCoolingSetpointTemperature = (int(__stdcall *)(HANDLE ph, SHORT sCoolSet))
	     GetProcAddress(SC2Lib, "PCO_SetCoolingSetpointTemperature")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetOffsetMode = (int(__stdcall *)(HANDLE ph, WORD *wOffsetRegulation))
	     GetProcAddress(SC2Lib, "PCO_GetOffsetMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetOffsetMode = (int(__stdcall *)(HANDLE ph, WORD wOffsetRegulation))
	     GetProcAddress(SC2Lib, "PCO_SetOffsetMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetTimingStruct = (int(__stdcall *)(HANDLE ph, PCO_Timing *strTiming))
	     GetProcAddress(SC2Lib, "PCO_GetTimingStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetTimingStruct = (int(__stdcall *)(HANDLE ph, PCO_Timing *strTiming))
	     GetProcAddress(SC2Lib, "PCO_SetTimingStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetDelayExposureTime = (int(__stdcall *)(HANDLE ph, DWORD* dwDelay, DWORD* dwExposure, WORD* wTimeBaseDelay, WORD* wTimeBaseExposure))
	     GetProcAddress(SC2Lib, "PCO_GetDelayExposureTime")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetDelayExposureTime = (int(__stdcall *)(HANDLE ph, DWORD dwDelay, DWORD dwExposure, WORD wTimeBaseDelay, WORD wTimeBaseExposure))
	     GetProcAddress(SC2Lib, "PCO_SetDelayExposureTime")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetDelayExposureTimeTable = (int(__stdcall *)(HANDLE ph, DWORD* dwDelay, DWORD* dwExposure, WORD* wTimeBaseDelay, WORD* wTimeBaseExposure, WORD wCount))
	     GetProcAddress(SC2Lib, "PCO_GetDelayExposureTimeTable")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetDelayExposureTimeTable = (int(__stdcall *)(HANDLE ph, DWORD* dwDelay, DWORD* dwExposure, WORD wTimeBaseDelay, WORD wTimeBaseExposure, WORD wCount))
	     GetProcAddress(SC2Lib, "PCO_SetDelayExposureTimeTable")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetTriggerMode = (int(__stdcall *)(HANDLE ph, WORD* wTriggerMode))
	     GetProcAddress(SC2Lib, "PCO_GetTriggerMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetTriggerMode = (int(__stdcall *)(HANDLE ph, WORD wTriggerMode))
	     GetProcAddress(SC2Lib, "PCO_SetTriggerMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((ForceTrigger = (int(__stdcall *)(HANDLE ph, WORD *wTriggered))
	     GetProcAddress(SC2Lib, "PCO_ForceTrigger")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetCameraBusyStatus = (int(__stdcall *)(HANDLE ph, WORD* wCameraBusyState))
	     GetProcAddress(SC2Lib, "PCO_GetCameraBusyStatus")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetPowerDownMode = (int(__stdcall *)(HANDLE ph, WORD* wPowerDownMode))
	     GetProcAddress(SC2Lib, "PCO_GetPowerDownMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetPowerDownMode = (int(__stdcall *)(HANDLE ph, WORD wPowerDownMode))
	     GetProcAddress(SC2Lib, "PCO_SetPowerDownMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetUserPowerDownTime = (int(__stdcall *)(HANDLE ph, DWORD* dwPowerDownTime))
	     GetProcAddress(SC2Lib, "PCO_GetUserPowerDownTime")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetUserPowerDownTime = (int(__stdcall *)(HANDLE ph, DWORD dwPowerDownTime))
	     GetProcAddress(SC2Lib, "PCO_SetUserPowerDownTime")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetExpTrigSignalStatus = (int(__stdcall *)(HANDLE ph, WORD* wExpTrgSignal))
	     GetProcAddress(SC2Lib, "PCO_GetExpTrigSignalStatus")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetCOCRuntime = (int(__stdcall *)(HANDLE ph, DWORD* dwTime_s, DWORD* dwTime_us))
	     GetProcAddress(SC2Lib, "PCO_GetCOCRuntime")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetStorageStruct = (int(__stdcall *)(HANDLE ph, PCO_Storage *strStorage))
	     GetProcAddress(SC2Lib, "PCO_GetStorageStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetStorageStruct = (int(__stdcall *)(HANDLE ph, PCO_Storage *strStorage))
	     GetProcAddress(SC2Lib, "PCO_SetStorageStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetCameraRamSize = (int(__stdcall *)(HANDLE ph, DWORD* dwRamSize, WORD* wPageSize))
	     GetProcAddress(SC2Lib, "PCO_GetCameraRamSize")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetCameraRamSegmentSize = (int(__stdcall *)(HANDLE ph, DWORD* dwRamSegSize))
	     GetProcAddress(SC2Lib, "PCO_GetCameraRamSegmentSize")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetCameraRamSegmentSize = (int(__stdcall *)(HANDLE ph, DWORD* dwRamSegSize))
	     GetProcAddress(SC2Lib, "PCO_SetCameraRamSegmentSize")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((ClearRamSegment = (int(__stdcall *)(HANDLE ph))
	     GetProcAddress(SC2Lib, "PCO_ClearRamSegment")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetActiveRamSegment = (int(__stdcall *)(HANDLE ph, WORD* wActSeg))
	     GetProcAddress(SC2Lib, "PCO_GetActiveRamSegment")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetActiveRamSegment = (int(__stdcall *)(HANDLE ph, WORD wActSeg))
	     GetProcAddress(SC2Lib, "PCO_SetActiveRamSegment")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetRecordingStruct = (int(__stdcall *)(HANDLE ph, PCO_Recording *strRecording))
	     GetProcAddress(SC2Lib, "PCO_GetRecordingStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetRecordingStruct = (int(__stdcall *)(HANDLE ph, PCO_Recording *strRecording))
	     GetProcAddress(SC2Lib, "PCO_SetRecordingStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetStorageMode = (int(__stdcall *)(HANDLE ph, WORD* wStorageMode))
	     GetProcAddress(SC2Lib, "PCO_GetStorageMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetStorageMode = (int(__stdcall *)(HANDLE ph, WORD wStorageMode))
	     GetProcAddress(SC2Lib, "PCO_SetStorageMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetRecorderSubmode = (int(__stdcall *)(HANDLE ph, WORD* wRecSubmode))
	     GetProcAddress(SC2Lib, "PCO_GetRecorderSubmode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetRecorderSubmode = (int(__stdcall *)(HANDLE ph, WORD wRecSubmode))
	     GetProcAddress(SC2Lib, "PCO_SetRecorderSubmode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetRecordingState = (int(__stdcall *)(HANDLE ph, WORD* wRecState))
	     GetProcAddress(SC2Lib, "PCO_GetRecordingState")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetRecordingState = (int(__stdcall *)(HANDLE ph, WORD wRecState))
	     GetProcAddress(SC2Lib, "PCO_SetRecordingState")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((ArmCamera = (int(__stdcall *)(HANDLE ph))
	     GetProcAddress(SC2Lib, "PCO_ArmCamera")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetAcquireMode = (int(__stdcall *)(HANDLE ph, WORD* wAcquMode))
	     GetProcAddress(SC2Lib, "PCO_GetAcquireMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetAcquireMode = (int(__stdcall *)(HANDLE ph, WORD wAcquMode))
	     GetProcAddress(SC2Lib, "PCO_SetAcquireMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetAcqEnblSignalStatus = (int(__stdcall *)(HANDLE ph, WORD* wAcquEnableState))
	     GetProcAddress(SC2Lib, "PCO_GetAcqEnblSignalStatus")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetDateTime = (int(__stdcall *)(HANDLE ph, BYTE ucDay, BYTE ucMonth, WORD wYear, WORD wHour, BYTE ucMin, BYTE ucSec))
	     GetProcAddress(SC2Lib, "PCO_SetDateTime")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetTimestampMode = (int(__stdcall *)(HANDLE ph, WORD* wTimeStampMode))
	     GetProcAddress(SC2Lib, "PCO_GetTimestampMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((SetTimestampMode = (int(__stdcall *)(HANDLE ph, WORD wTimeStampMode))
	     GetProcAddress(SC2Lib, "PCO_SetTimestampMode")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetImageStruct = (int(__stdcall *)(HANDLE ph, PCO_Image *strImage))
	     GetProcAddress(SC2Lib, "PCO_GetImageStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetSegmentStruct = (int(__stdcall *)(HANDLE ph, WORD wSegment, PCO_Segment *strSegment))
	     GetProcAddress(SC2Lib, "PCO_GetSegmentStruct")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetSegmentImageSettings = (int(__stdcall *)(HANDLE ph, WORD wSegment, WORD* wXRes, WORD* wYRes, WORD* wBinHorz, WORD* wBinVert, WORD* wRoiX0, WORD* wRoiY0, WORD* wRoiX1, WORD* wRoiY1))
	     GetProcAddress(SC2Lib, "PCO_GetSegmentImageSettings")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetNumberOfImagesInSegment = (int(__stdcall *)(HANDLE ph, WORD wSegment, DWORD* dwValidImageCnt, DWORD* dwMaxImageCnt))
	     GetProcAddress(SC2Lib, "PCO_GetNumberOfImagesInSegment")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((OpenCamera = (int(__stdcall *)(HANDLE *ph, WORD wCamNum))
	     GetProcAddress(SC2Lib, "PCO_OpenCamera")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((CloseCamera = (int(__stdcall *)(HANDLE ph))
	     GetProcAddress(SC2Lib, "PCO_CloseCamera")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((AllocateBuffer = (int(__stdcall *)(HANDLE ph, SHORT* sBufNr, DWORD size, WORD** wBuf, HANDLE *hEvent))
	     GetProcAddress(SC2Lib, "PCO_AllocateBuffer")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((FreeBuffer = (int(__stdcall *)(HANDLE ph, SHORT sBufNr))
	     GetProcAddress(SC2Lib, "PCO_FreeBuffer")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((AddBuffer = (int(__stdcall *)(HANDLE ph, DWORD dw1stImage, DWORD dwLastImage, SHORT sBufNr))
	     GetProcAddress(SC2Lib, "PCO_AddBuffer")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((AddBufferEx = (int(__stdcall *)(HANDLE ph, DWORD dw1stImage, DWORD dwLastImage, SHORT sBufNr, WORD wXRes, WORD wYRes, WORD wBitRes))
	     GetProcAddress(SC2Lib, "PCO_AddBufferEx")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetBufferStatus = (int(__stdcall *)(HANDLE ph, SHORT sBufNr, DWORD *dwStatusDll, DWORD *dwStatusDrv))
	     GetProcAddress(SC2Lib, "PCO_GetBufferStatus")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((RemoveBuffer = (int(__stdcall *)(HANDLE ph))
	     GetProcAddress(SC2Lib, "PCO_RemoveBuffer")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetImage = (int(__stdcall *)(HANDLE ph, WORD dwSegment, DWORD dw1stImage, DWORD dwLastImage, SHORT sBufNr))
	     GetProcAddress(SC2Lib, "PCO_GetImage")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((GetPendingBuffer = (int(__stdcall *)(HANDLE ph, int *count))
	     GetProcAddress(SC2Lib, "PCO_GetPendingBuffer")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((CancelImages = (int(__stdcall *)(HANDLE ph))
	     GetProcAddress(SC2Lib, "PCO_CancelImages")) == NULL)
		return SC2_ERROR_SDKDLL;
	if ((CheckDeviceAvailability = (int(__stdcall *)(HANDLE ph, WORD wNum))
	     GetProcAddress(SC2Lib, "PCO_CheckDeviceAvailability")) == NULL)
		return SC2_ERROR_SDKDLL;
	if((CamLinkSetImageParameters = (int (__stdcall *)(HANDLE ph, WORD wXResAct, WORD wYResAct))
	     GetProcAddress(SC2Lib, "PCO_CamLinkSetImageParameters")) == NULL)
		return SC2_ERROR_SDKDLL;

	DEBUG("Done with wall of functions...");

	return FALSE;
}

// I think we die in this function from time to time. Is it file I/O, maybe the file isn't writable or something?
int PcoEdgeCamera::storeTiff(string filename, int width, int height, void *buf, Bild *strBild)
{
	// This is for 2 bytes per pixel. Seems to be only 14 bits per pixel of dynamic range.
	MagickCore::StorageType bpp = MagickCore::ShortPixel;

	// We need to store this in the RGB colorspace so it converts properly to grayscale. Storing as blacK would make most sense, but doesn't seem to work...
	Magick::Image image(width, height, "R", bpp, buf);
	image.colorSpace(MagickCore::GRAYColorspace);
	DEBUG("Writing " << filename << "...");
	image.write(filename);
	DEBUG("done.");

	return 0;
}

// This is copied from the demo. Easiest for us to just use TIFF instead.
// NB: This appears to put saturated pixels to black.
int PcoEdgeCamera::store_b16(const char *filename, int width, int height, void *buf, Bild *strBild)
{
	unsigned char *cptr;
	//  unsigned char *c1;
	B16_HEADER *pb16;
	//  int *b1;
	int e;
	unsigned char *pi;
	HANDLE hfstore;
	unsigned long z, zz;
	DWORD headerl;
	//  char of[20];

	cptr = (unsigned char *)malloc(2000);
	memset(cptr, 0, 2000);
	headerl = 512;

	pb16 = (B16_HEADER*) cptr;
	pb16->ucPco[0] = 'P';
	pb16->ucPco[1] = 'C';
	pb16->ucPco[2] = 'O';
	pb16->ucPco[3] = '-';
	pb16->uiFileLen = (width*height*2) + headerl;
	pb16->uiHeaderLen = headerl;
	pb16->uiXRes = width;
	pb16->uiYRes = height;
	pb16->uiLutSign = 0xFFFFFFFF;
	pb16->uiColor = strBild->iColor;
	pb16->uiBMin = strBild->iBWMin;
	pb16->uiBWMax = strBild->iBWMax;
	pb16->uiBWLut = strBild->iBWLut;
	pb16->uiRMin = strBild->iRMin;
	pb16->uiRMax = strBild->iRMax;
	pb16->uiGMin = strBild->iGMin;
	pb16->uiGMax = strBild->iGMax;
	pb16->uiBMin = strBild->iBMin;
	pb16->uiBMax = strBild->iBMax;
	pb16->uiColLut = strBild->iColLut;
	pb16->uiDS;
	if (strBild->bDouble) // bei Doubleshutter Bild, Kennung ablegen
		pb16->uiDS = 0x5344;// DS (SD) -> Kennung für 'DoubleShutter'
	else
	{                     // kein Doubleshutter: Lut-Werte kopieren in 2. Wertesatz
		strBild->iBWMin2 = strBild->iBWMin;                   // Lut bw min
		strBild->iBWMax2 = strBild->iBWMax;                   // Lut bw max
		strBild->iBWLut2 = strBild->iBWLut;                   // Lut lin log
		strBild->iRMin2 = strBild->iRMin;                    // red min
		strBild->iRMax2 = strBild->iRMax;                    // red max
		strBild->iGMin2 = strBild->iGMin;                    // green min
		strBild->iGMax2 = strBild->iGMax;                    // green max
		strBild->iBMin2 = strBild->iBMin;                    // blue min
		strBild->iBMax2 = strBild->iBMax;                    // blue max
		strBild->iColLut2 = strBild->iColLut;                  // Lut lin log color
	}
	strBild->iVersion = FILEVERSION302;

	pi = cptr + 128;
	memset(pi, 0, 384);// Strukturdaten auf 0
	memcpy(pi, &strBild->sTime, sizeof(Bild) - sizeof(WORD*));// Struktur ablegen

	hfstore = CreateFile(filename,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		0);
	if (hfstore== INVALID_HANDLE_VALUE)
	{
		free(cptr);
		return (NOFILE);
	}
  
	// Write the header:
	z = headerl;
	e = WriteFile(hfstore, (void *)cptr, z, &zz, NULL);
	if ((e == 0) ||(z != zz)) {
		CloseHandle(hfstore);
		DeleteFile(filename);
		free(cptr);
		return (NOFILE);
	}

	// Write the image (2 bytes per pixel?)
	z = width*height*2;
	e = WriteFile(hfstore, (void *)buf, z, &zz, NULL);
	if ((e == 0) ||(z != zz)) {
		CloseHandle(hfstore);
		DeleteFile(filename);
		free(cptr);
		return (NOFILE);
	}

	CloseHandle(hfstore);
	free(cptr);
	return 0;
}

// Print out some camera properties (stdout for now)
void PcoEdgeCamera::cameraStats()
{
	/***********************************************************
	Note the return of ccd temperature must be divided 
	by 10 to get its true value
	*************************************************************/

	GetTemperature(hCamera, &ccdtemp, &camtemp, &powtemp);
	cout << "Operating temperatures:"
	     << "\tCCD: " << ccdtemp/10 << " C" << endl
	     << "\tElectronics: " << camtemp << " C" << endl
	     << "\tPower: " << powtemp << " C" << endl
	     << "#pictures tried, good: " << npicstried << ", " << npicsgood << endl;
}

void PcoEdgeCamera::getTemperature(short &ccdtemp, short &camtemp, short &powtemp)
{
	GetTemperature(hCamera, &ccdtemp, &camtemp, &powtemp);
	ccdtemp /= 10;
}

// The pco.edge 5.5 is fixed at 5C setpoint. These might be
// useful if we get a different model pco camera.
void PcoEdgeCamera::setSetpointTemp(short &setpoint)
{
	SetCoolingSetpointTemperature(hCamera, setpoint);
}

// Passing by reference since getTemperature() does the same
void PcoEdgeCamera::getSetpointTemp(short &setpoint)
{
	GetCoolingSetpointTemperature(hCamera, &setpoint);
}

double PcoEdgeCamera::getExposureMs()
{
	return dwExposure;
}

bool PcoEdgeCamera::setExposureMs(double newExposure)
{
	// TODO: Implement delay at some point.
	dwExposure = newExposure;
	expbase = 2;
	err = SetDelayExposureTime(hCamera, // Timebase: 0-ns; 1-us; 2-ms
	                           0,		// DWORD dwDelay
	                           (DWORD)dwExposure,
	                           0,		// WORD wTimeBaseDelay,
	                           expbase);	// WORD wTimeBaseExposure

	if (err != 0) {
		sprintf_s(errbuffer, EBUFS, "\nPCO_SetDelayExposureTime (hex): %lx\n", err);
		return false; // TODO: Should have error flags or something...
	}

	// TODO: More rigorous error checking!
	return newExposure > 0;
}

// Binning
short PcoEdgeCamera::getVbin()
{
	return vbin;
}

bool PcoEdgeCamera::setVbin(short vbinin)
{
	// If our resolution is at max, then changing the bin should keep us at max:
	if (roiy1 == 1 && roiy2 == maxVertRes())
		roiy1 = roiy2 = 0;

	return setBin(&vbin, vbinin);
}

short PcoEdgeCamera::getHbin()
{
	return hbin;
}

bool PcoEdgeCamera::setHbin(short hbinin)
{
	// If our resolution is at max, then changing the bin should keep us at max:
	if (roix1 == 1 && roix2 == maxHorzRes())
		roix1 = roix2 = 0;

	return setBin(&hbin, hbinin);
}

bool PcoEdgeCamera::setBin(short *bin, short val)
{
	if (val == 1 || val == 2 || val == 4) {
		*bin = val;

		return setBinning();
	}

	return false;
}

bool PcoEdgeCamera::setBinning()
{
	if (SetBinning(hCamera, hbin, vbin)) {
		cerr << "Failed to set binning: " << hbin << ", " << vbin << endl;

		return false;
	}

	DEBUG("Set binning to " << hbin << ", " << vbin);

	// Hmmm, what should we do with this guy's return value?
	setRoi(roix1, roix2, roiy1, roiy2);

	return true;
}
