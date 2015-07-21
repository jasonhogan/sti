/*! \file CameraAbs.cpp
 *  \author Brannon Klopfer bbklopfer@gmail.com
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

#define GAIN_NORMAL 0
#define GAIN_HIGH 1
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
		} else {
			DEBUG("Arming seemed to have failed?");
		}
	}
	DEBUG("Nominally instantiated.");

	cout << "Mode: " << pcocam->Mode() << endl;

	this->singlePhotonRad = 1;
	this->singlePhotonThresh = 80;

}

// TODO: Verify that this actually works...
void CameraAbs::printErrors()
{
	DEBUG("TODO: implement printErrors");
}

CameraAbs::~CameraAbs()
{
	DWORD err;

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
	if(pcobuf) {
		int change=FALSE;
		pcobuf->Allocate();
	}
}

// TODO: DECOMP! This should be a feature with our pco.edge as well
// Simple photon-counting for use with PicoStar (or any high-gain setup, I guess)
int CameraAbs::photonCount(string filename, int npics)
{
	int bufstat, err = 0;
	int len; // So...this is sketchy, given that we assume we're set to the correct width/height...
	short *cimg, *img;
	int r = singlePhotonRad; // 'radius' for photons..

	len = height*width;
	img = (short*) malloc(sizeof(short)*len);
	cimg = (short*) malloc(sizeof(short)*len); // current img, for npics > 1
	/* Initialize the photon image */
	for (int i = 0; i < len; i++)
		img[i] = 0;

	err = pcocam->Start(); // Is this right? Does this belong in loop?

	for (int i = 0; i < npics; i++) {
		err = pcocam->SendTrigger();
		pcocam->GetImage(pcobuf);
		bufstat = pcobuf->Status();

		if((bufstat & 0x0F000) != 0) {
			DEBUG("Image error flags set status: " << bufstat);
			err++;
		} else if ((bufstat & 0x0002) == 0) {
			DEBUG("NO IMAGE GRABBED!");
			err++;
		}

		void *buf = pcobuf->bufadr();
		memcpy((void*) cimg, pcobuf->bufadr(), sizeof(short)*len);

		// Oh god, this is pretty silly. Too many nested loops! Decomp please.
		for (int p = 0; p < pcobuf->Width() * pcobuf->Height(); p++) {
			if (cimg[p] > singlePhotonThresh) {
				short tmp = cimg[p];
				int tmpidx = p;
				for (int ri = p-r; ri <= p+r; ri++) {
					for (int c = -r; c <= r; c++) { // Column, needs to be unrolled.
						int idx = (c*width)+ri;
						if (idx < 0 || idx > len)
							continue;

						if (cimg[idx] > tmp) {
							tmpidx = idx;
							tmp = cimg[idx];
						}
						// Don't double-count anything.
						cimg[idx] = 0;
					}
				}
				cimg[tmpidx] = 1;
				img[tmpidx]++;
			} else {
				cimg[p] = 0;
			}
		}
		pcocam->CancelBuffers(); // Here? Once? At end?
	}

	Magick::Image image(width, height, "I", MagickCore::ShortPixel, img);
	image.write(filename);

	std::free((void*) img);
	std::free((void*) cimg);

	commitAttributes(); 

	return err;
}
// I have no idea if this is right. 
int CameraAbs::takePicture(string filename)
{
	int err = 0;
	int bufstat;
	
	
	/* I believe this is required for ext. triggering...?
	pcocam->TriggerMode(0x001);
	*/

	err = pcocam->Start();
	err = pcocam->SendTrigger();
	pcocam->GetImage(pcobuf);
	bufstat=pcobuf->Status();

	if ((bufstat & 0x0F000) != 0) {
		DEBUG("Image error flags set status: " << bufstat);
		err++;
	} else if ((bufstat & 0x0002) == 0) {
		DEBUG("NO IMAGE GRABBED!");
		err++;
	}
	
	storeTiff(filename, pcobuf);
	pcocam->CancelBuffers();

	// Not sure how much of this is required...?
	commitAttributes(); // Maybe we don't need to adjust buffer size...who really knows?

	return err;
}

void CameraAbs::commitAttributes()
{
	pcocam->Stop();
	pcocam->ArmCamera();
	adjustBuffersize();

	// FIXME: Might not work with ROI'ing!
	this->width = pcocam->CCDSize_X()/pcocam->HorizontalBinning();
	this->height = pcocam->CCDSize_Y()/pcocam->VerticalBinning();
}


#include <bitset>

// I think we die in this function from time to time. Is it file I/O, maybe the file isn't writable or something?
int CameraAbs::storeTiff(string filename, PCOBuffer* pcobuf)
{
	int width = pcobuf->Width();
	int height = pcobuf->Height();
	void *buf = pcobuf->bufadr();
	
	// This is for 2 bytes per pixel. Seems to be only 14 bits per pixel of dynamic range.
	//MagickCore::StorageType bpp = MagickCore::ShortPixel;
	MagickCore::StorageType bpp = MagickCore::ShortPixel;
	// AHA! Store as Intensity ("I"), NOT RGB!
	Magick::Image image(width, height, "I", bpp, buf);
	image.write(filename);

	return 0;
}

int CameraAbs::getGain()
{
	return (pcocam->Mode() >> 8) & 0xFF;
}

int CameraAbs::setGain(int gain)
{
	int type, submode, mode;
	
	mode = pcocam->Mode();
	type = mode & 0xFF;
	submode = (mode >> 16) & 0xFF;

	// Huh? Does the & 0xFF do anything here? Prob no. Copypasta from manual...
	pcocam->Mode(((type & 0xFF) | ((gain & 0xFF)<<8) | ((submode & 0xFF) << 16)));

	return getGain();
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