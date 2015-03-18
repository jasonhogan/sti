/*! \file CameraAbs.cpp
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
#include "CameraAbs.hpp"

#define DEBUG(str) std::cerr << str << std::endl;

using namespace std;

CameraAbs::CameraAbs()
{
	DEBUG("Instantiating...");
	DWORD err;
	pcocam = new PCOCamera();
	if (pcocam)
		err = pcocam->Open(0);
	else
		return;
	DEBUG("Ok...");
	if(!err)
	{
		pcobuf = new PCOBuffer(pcocam->GetHandle());
		
		if(pcobuf==NULL)
		{
			DEBUG("Buffer issues");
			delete this;
			return;
		}
		err = pcocam->Stop();
		pcocam->SetDefaultValues();
		pcocam->SetImageTimeout(1000); // punt
		err = pcocam->ArmCamera();
		if(!err) {
			adjustBuffersize();
		/*	SetupConvert();
			GetDlgItem(IDC_BUT_OPEN)->SetWindowText(_T("Close Camera"));
			if(pcocam->IsColor()&&((image_bpp==24)||(image_bpp==32)))
			GetDlgItem(IDC_CHECKBOX_COLOR)->EnableWindow(TRUE);
			else
			GetDlgItem(IDC_CHECKBOX_COLOR)->EnableWindow(FALSE);*/
		} else {
			DEBUG("Arming seemed to have failed?");
		}
	}
	DEBUG("Nominally instantiated.");
}

// TODO: Verify that this actually works...
void CameraAbs::printErrors()
{
	DEBUG("TODO: implement printErrors");
}

CameraAbs::~CameraAbs()
{
	DWORD err;

	/*if(grabthread)
	OnBnClickedButGrabthread();
	FreeConvert();
	if(pcodiacam)
	{
		pcodiacam->Unload();
		delete pcodiacam;
		pcodiacam=NULL;
	}
	if(pcobuf)
	{
		delete pcobuf;
		pcobuf=NULL;
	}
	if(image)
	{
		image->Destroy();
		delete image;
		image=NULL;
	}
	if(image_2)
	{
		image_2->Destroy();
		delete image_2;
		image_2=NULL;
	}
	*/
	if(pcocam)
	{
		if(pcocam->GetHandle())
		{
			err = pcocam->Stop();
			err = pcocam->Close();
		}
		delete pcocam;
		pcocam = NULL;
	}
}


// TODO: Not positive where this should live. Probably not here. Perhaps we should have some intermediate interface?
void CameraAbs::adjustBuffersize()
{
	DEBUG("Adjusting buffersize...");
	if(pcobuf) {
		int change=FALSE;
		pcobuf->Allocate();
	}
		/*if(image==NULL)
		change=TRUE;
		else if((pcobuf->Width() != image->GetWidth()) || (pcobuf->Height() != image->GetHeight()))
		change=TRUE;

		if(change==TRUE) {
			int bpp,height,width,pitch;
			void *pic; 
			if(image==NULL)
				image= new CImage;
			else
				image->Destroy();

			if(image_2==NULL)
				image_2= new CImage;
			else
				image_2->Destroy();
/*
			image->Create(pcobuf->Width(),-1*pcobuf->Height(),image_bpp,0);
			image_2->Create(pcobuf->Width(),-1*pcobuf->Height(),image_bpp,0);
			bpp=image->GetBPP();
			width=image->GetWidth();
			height=image->GetHeight();
			pitch=image->GetPitch();
			pic=image->GetPixelAddress(0,0);
			memset(pic,0,pitch*height);
			pic=image_2->GetPixelAddress(0,0);
			memset(pic,0,pitch*height);
			if(pcoconv) pcoconv->set_sizes(pcobuf->Width(),pcobuf->Height());
			
		}
	}*/
}

// I have no idea if this is right. 
int CameraAbs::takePicture(string filename)
{
	DEBUG("taking picture.");
	int err = 0;
	int bufstat;
	err = pcocam->Start();
	DEBUG("Start: " << err);

	err = pcocam->SendTrigger();
	DEBUG("SendTrigger(): " << err);

	pcocam->GetImage(pcobuf);
	bufstat=pcobuf->Status();
	if((bufstat & 0x0F000) != 0)
	{
		DEBUG("Image error flags set status: " << bufstat);
		err++;
	}
	else if ((bufstat & 0x0002) == 0)
	{
		DEBUG("NO IMAGE GRABBED!");
		err++;
	}
	else
	{
		DEBUG("Hot dawg!");
	}
	
	DEBUG("Nominal size: " << pcobuf->Width() << "x" << pcobuf->Height());

	storeTiff(filename, pcobuf->Width(), pcobuf->Height(), pcobuf->bufadr());
	pcocam->CancelBuffers();

	return err;
}

void CameraAbs::commitAttributes()
{
	pcocam->Stop();
	pcocam->ArmCamera();
	adjustBuffersize();
}


// I think we die in this function from time to time. Is it file I/O, maybe the file isn't writable or something?
int CameraAbs::storeTiff(string filename, int width, int height, void *buf)
{
	DEBUG("Attempting to store tiff...");
	// This is for 2 bytes per pixel. Seems to be only 14 bits per pixel of dynamic range.
	MagickCore::StorageType bpp = MagickCore::ShortPixel;

	// We need to store this in the RGB colorspace so it converts properly to grayscale. Storing as blacK would make most sense, but doesn't seem to work...
	Magick::Image image(width, height, "R", bpp, buf);
	image.colorSpace(MagickCore::GRAYColorspace);
	DEBUG("Writing " << filename << "...");
	image.write(filename);
	 
	/*cerr << "DISPLAYING! X11..." << endl;
	image.display();
	cerr << "Done displaying, I don't think that worked..." << endl;
	*/
	DEBUG("done.");

	return 0;
}

double CameraAbs::getExposureMs()
{
	return pcocam->Exptime_ms();
}

bool CameraAbs::setExposureMs(double newExposure)
{
	pcocam->Exptime_ms(newExposure);

	return (getExposureMs() == newExposure);
}

short CameraAbs::getVbin()
{
	return pcocam->VerticalBinning();
}
bool CameraAbs::setVbin(short vbinin)
{
	pcocam->VerticalBinning(vbinin);

	return true;
}
short CameraAbs::getHbin()
{
	return pcocam->HorizontalBinning();
}
bool CameraAbs::setHbin(short hbinin)
{
	pcocam->HorizontalBinning(hbinin);

	return true;
}
bool CameraAbs::setRoi(short x1, short x2, short y1, short y2)
{
	pcocam->SetROI(x1, x2, y1, y2);
	
	return true;
}