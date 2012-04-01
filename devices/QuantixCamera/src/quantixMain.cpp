/*! \file main.cpp
 *  \author Susannah Dickerson 
 *  \brief main file for Quantix Camera Device
 *  \section license License
 *
 *  Copyright (C) 2009 Susannah M Dickerson <sdickers@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *	
 *	This structure shamlessly derived from source code originally by Jason
 *	Hogan <hogan@stanford.edu>
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

#include <string>
#include <iostream>
#include <exception>

#include <ORBManager.h>
#include "QuantixDevice.h"

using namespace std;

class stringException: public exception
{
public:
	std::string errString;
	stringException(std::string inString) : errString(inString) {}
	const char* what() const throw() {return errString.c_str();}
};

ORBManager* orbManager;
void printError();

int main(int argc, char* argv[])
{
	// for STI
	orbManager = new ORBManager(argc, argv);    
	string ipAddress = "epdesktop2.stanford.edu";

	// for cameras
	int16 totalCameras = 0;
	char camName[CAM_NAME_LEN];
	std::vector <std::string> cameraNames;
	std::vector <int16> cameraHandles;  
	int16 cameraHandle;
	rs_bool success = TRUE;
	bool initialized = false;
	
	QuantixDevice *quantixDevice;
	std::vector<QuantixDevice *> quantixDevices;
	try {
		success = pl_pvcam_init();
		if (!success)
			throw stringException("Could not initialize Quantix drivers");

		//Get the number of cameras and camera names
		pl_cam_get_total(&totalCameras);
		totalCameras = 2;
		for (int i = 1; i < totalCameras; i++) 
		{
			if(!pl_cam_get_name(i, camName))
				throw stringException("Could not get name of Quantix Camera " + STI::Utils::valueToString(i));
			if(!pl_cam_open(camName, &cameraHandle, OPEN_EXCLUSIVE ))
				throw stringException("Could not open Quantix Camera " + STI::Utils::valueToString(i));
			if (!pl_cam_get_diags(cameraHandle))
				throw stringException("Errors in diagnostics for Quantix Camera");
			
			rs_bool availFlag;
			pl_get_param(cameraHandle, PARAM_HEAD_SER_NUM_ALPHA, ATTR_AVAIL, (void *) &availFlag);
			pl_get_param(cameraHandle, PARAM_CHIP_NAME, ATTR_AVAIL, (void *) &availFlag);
			
			int16 type;
			pl_get_param(cameraHandle, PARAM_HEAD_SER_NUM_ALPHA, ATTR_TYPE, (void *) &type);

			char serialNum[CCD_NAME_LEN + 1];
			pl_get_param(cameraHandle, PARAM_CHIP_NAME, ATTR_CURRENT, (void *) &serialNum);
			cameraHandles.push_back(cameraHandle);

			quantixDevice = new QuantixDevice(orbManager, "Quantix " + STI::Utils::valueToString(i), ipAddress, i, cameraHandle, "C:\\code\\sti\\devices\\QuantixCamera\\src\\quantix.ini");
			initialized |= quantixDevice->initialized;
			if (quantixDevice->initialized)
				quantixDevices.push_back(quantixDevice);
			else 
				delete quantixDevice;
			
		}
	

		if (initialized) 
		{
			orbManager->run();
		} else {
			throw stringException("Error initializing Quantix camera(s)");
		}
	}
	catch (stringException& e)
	{
		//Print my message, then Quantix's message (which are somewhat obscure)
		std::cout << e.what() << std::endl;
		printError();
	}
	catch (...)
	{
		std::cout << "Unhandled exception; shutting down..." << std::endl;
	}

	//Shutdown cameras
	if (!cameraHandles.empty())
	{
		for (int i = 0; i < cameraHandles.size(); i++) 
		{
			if (pl_cam_check(cameraHandles.at(i)))
				pl_cam_close(cameraHandles.at(i));
		}
	}

    pl_pvcam_uninit();

	for(unsigned int i = 0; i < quantixDevices.size(); i++)
	{
		delete quantixDevices.at(i);
	}




	return 0;
}

void printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}

/*static void AcquireContinuous( int16 hCam );
static void AcquireContinuousWithWait( int16 hCam );
static void AcquireContinuousThreaded( int16 hCam );
void acquireContinuousWrapper(void* object);
void waitForImage();
void waitForImagesWrapper(void* object);
void waitForImages(uns32 frame_size);
void printError();

omni_mutex *testCameraMutex;
omni_condition *testCameraCondition;

int16 cameraHandle;

int main(int argc, char **argv)
{
	testCameraMutex = new omni_mutex();
	testCameraCondition = new omni_condition(testCameraMutex);

    char cam_name[CAM_NAME_LEN];    // camera name                    
    int16 hCam;                     // camera handle                  
    rs_bool avail_flag;             // ATTR_AVAIL, param is available 
	char a;

    // Initialize the PVCam Library and Open the First Camera 
    pl_pvcam_init();
    pl_cam_get_name( 0, cam_name );
    pl_cam_open(cam_name, &hCam, OPEN_EXCLUSIVE );

	cameraHandle = hCam;
    // check for circular buffer support 
    if( pl_get_param( hCam, PARAM_CIRC_BUFFER, ATTR_AVAIL, &avail_flag ) &&
		avail_flag )
	{
        AcquireContinuous( hCam );
		
		//omni_thread::create(acquireContinuousWrapper, &hCam, omni_thread::PRIORITY_LOW);
        //AcquireContinuousThreaded( hCam );
		//std::cout << "Press any key to shut down" << std::endl;
		//std::cin >> a;
		
	}
    else
        printf( "circular buffers not supported\n" );

    pl_cam_close( hCam );

    pl_pvcam_uninit();

	delete testCameraMutex;
	delete testCameraCondition;

    return 0;
}

void acquireContinuousWrapper(void* object)
{
	int16 *hCam = static_cast<int16*>(object);
	AcquireContinuous(*hCam);
}

void AcquireContinuous( int16 hCam )
{
    rgn_type region = { 0, 511, 1, 0, 511, 1 };
    uns32 buffer_size, frame_size;
    uns16 *buffer;
    int16 status;
    uns32 not_needed;
    void_ptr address;
    uns16 numberframes = 5;
	bool error = false;
	int i = 0;
    
    // Init a sequence set the region, exposure mode and exposure time 
    pl_exp_init_seq();
    pl_exp_setup_cont( hCam, 1, &region, TIMED_MODE, 10, &frame_size, 
                       CIRC_NO_OVERWRITE );

    // set up a circular buffer of 3 frames 
    buffer_size = frame_size * 3;
    buffer = (uns16*)malloc( buffer_size );

	std::cerr << "Frame size: " << frame_size << std::endl;
    // Start the acquisition 
    printf( "Collecting %i Frames\n", numberframes );
    pl_exp_start_cont(hCam, buffer, buffer_size );
    
    // ACQUISITION LOOP 
    while( numberframes && !error) {
        // wait for data or error 
        //while( pl_exp_check_cont_status( hCam, &status, &not_needed, 
         //                                &not_needed ) && 
         //      (status != READOUT_COMPLETE && status != READOUT_FAILED) );
		while(!error)
		{
			if (!pl_exp_check_cont_status( hCam, &status, &not_needed, &not_needed ))
				error = true;

			// Check Error Codes 
			if( status == READOUT_FAILED ) {
				printf( "Data collection error: %i\n", pl_error_code() );
				error = true;
			}

			if ( status == READOUT_COMPLETE && pl_exp_get_oldest_frame( hCam, &address )) {
				// address now points to valid data 
				printf( "Center Three Points: %i, %i, %i\n", 
						*((uns16*)address + frame_size/sizeof(uns16)/2 - 1),
						*((uns16*)address + frame_size/sizeof(uns16)/2),
						*((uns16*)address + frame_size/sizeof(uns16)/2 + 1) );
				numberframes--;
				printf( "Remaining Frames %i\n", numberframes );
				pl_exp_unlock_oldest_frame( hCam );
				printf("Slept %i times \n", i);
				i = 0;
				break;
			}

			Sleep(500);
			i++;
		}
    } // End while 

    // Stop the acquisition 
    pl_exp_stop_cont(hCam,CCS_HALT);

    // Finish the sequence 
    pl_exp_finish_seq( hCam, buffer, 0);
    
    //Uninit the sequence 
    pl_exp_uninit_seq();

    free( buffer );
}

void AcquireContinuousWithWait( int16 hCam )
{
    rgn_type region = { 0, 511, 1, 0, 511, 1 };
    uns32 buffer_size, frame_size;
    uns16 *buffer;
    int16 status;
    uns32 not_needed;
    void_ptr address;
    uns16 numberframes = 5;
	bool error = false;
	int i = 0;
    
    // Init a sequence set the region, exposure mode and exposure time 
    pl_exp_init_seq();
    pl_exp_setup_cont( hCam, 1, &region, TIMED_MODE, 100, &frame_size, 
                       CIRC_NO_OVERWRITE );

    // set up a circular buffer of 3 frames 
    buffer_size = frame_size * 3;
    buffer = (uns16*)malloc( buffer_size );

    // Start the acquisition 
    printf( "Collecting %i Frames\n", numberframes );
    pl_exp_start_cont(hCam, buffer, buffer_size );
    
    // ACQUISITION LOOP 
    while( numberframes && !error) {
        // wait for data or error 
        //while( pl_exp_check_cont_status( hCam, &status, &not_needed, 
        //                                 &not_needed ) && 
        //       (status != READOUT_COMPLETE && status != READOUT_FAILED) );
		
		waitForImage();

		if (!pl_exp_check_cont_status( hCam, &status, &not_needed, &not_needed ))
			error = true;

		// Check Error Codes 
		if( status == READOUT_FAILED ) {
			printf( "Data collection error: %i\n", pl_error_code() );
			error = true;
		}

		if ( status == READOUT_COMPLETE && pl_exp_get_oldest_frame( hCam, &address )) {
			// address now points to valid data 
			printf( "Center Three Points: %i, %i, %i\n", 
					*((uns16*)address + frame_size/sizeof(uns16)/2 - 1),
					*((uns16*)address + frame_size/sizeof(uns16)/2),
					*((uns16*)address + frame_size/sizeof(uns16)/2 + 1) );
			numberframes--;
			printf( "Remaining Frames %i\n", numberframes );
			pl_exp_unlock_oldest_frame( hCam );
			printf("Slept %i times \n", i);
			i = 0;
			break;
		}

		Sleep(500);
		i++;
		
    } // End while 

    // Stop the acquisition 
    pl_exp_stop_cont(hCam,CCS_HALT);

    // Finish the sequence 
    pl_exp_finish_seq( hCam, buffer, 0);
    
    //Uninit the sequence 
    pl_exp_uninit_seq();

    free( buffer );
}

void AcquireContinuousThreaded( int16 hCam )
{
    rgn_type region = { 0, 511, 1, 0, 511, 1 };
    uns32 buffer_size, frame_size;
    uns16 *buffer;
   
    
    // Init a sequence set the region, exposure mode and exposure time 
    pl_exp_init_seq();
    pl_exp_setup_cont( hCam, 1, &region, BULB_MODE, 100, &frame_size, 
                       CIRC_NO_OVERWRITE );

    // set up a circular buffer of 3 frames
    buffer_size = frame_size * 3;
    buffer = (uns16*)malloc( buffer_size );

    // Start the acquisition 
    pl_exp_start_cont(hCam, buffer, buffer_size );
    
    // ACQUISITION LOOP 
	omni_thread::create(waitForImagesWrapper, &frame_size, omni_thread::PRIORITY_LOW);
    
	//waitForImages();
	testCameraMutex->lock();
		testCameraCondition->wait();
	testCameraMutex->unlock();

    // Stop the acquisition 
    pl_exp_stop_cont(hCam,CCS_HALT);

    // Finish the sequence 
    pl_exp_finish_seq( hCam, buffer, 0);
    
    // Uninit the sequence 
    pl_exp_uninit_seq();

    free( buffer );
}
void waitForImage()
{
	bool stopEvent = false;
	int16 status;
    uns32 not_needed;
	int i = 0;

	//Wait for an image to be acquired; stop acquisition if an error has occurred.
	while(stopEvent == false)
	{
		i++;
		if(!pl_exp_check_cont_status( cameraHandle, &status, &not_needed, &not_needed ))
		{
			printError();
			break;
		}

		if (status == READOUT_FAILED)
		{
			std::cerr << "Readout failed" << std::endl;
			break;
		}

		if (status == READOUT_COMPLETE)
		{
			std::cerr << "Waited " << i << "times" << std::endl;
			return;
		}
		else
		{
			Sleep(500);
//			pauseCameraMutex->lock();
				//omni_thread::get_time(&waitS, &waitNS, 2, 0);
				//pauseCameraCondition->timedwait(waitS,waitNS);

			//pauseCameraMutex->unlock();
			continue;
		}

	}

	
	stopEvent = true;
	return;

}

void waitForImagesWrapper(void* object)
{
	uns32 *frame_size = static_cast<uns32*>(object);
	waitForImages(*frame_size);
}
void waitForImages(uns32 frame_size)
{
	bool error = false;
	uns16 numberframes = 5;

	int16 status;
    uns32 not_needed;
    void_ptr address;
	int i = 0;

    printf( "Collecting %i Frames\n", numberframes );

	while( numberframes && !error) {
        // wait for data or error 
        //while( pl_exp_check_cont_status( hCam, &status, &not_needed, 
        //                                 &not_needed ) && 
        //       (status != READOUT_COMPLETE && status != READOUT_FAILED) );
		
		waitForImage();

		if (!pl_exp_check_cont_status( cameraHandle, &status, &not_needed, &not_needed ))
			error = true;

		// Check Error Codes 
		if( status == READOUT_FAILED ) {
			printf( "Data collection error: %i\n", pl_error_code() );
			error = true;
		}

		if ( status == READOUT_COMPLETE && pl_exp_get_oldest_frame( cameraHandle, &address )) {
			// address now points to valid data 
			printf( "Center Three Points: %i, %i, %i\n", 
					*((uns16*)address + frame_size/sizeof(uns16)/2 - 1),
					*((uns16*)address + frame_size/sizeof(uns16)/2),
					*((uns16*)address + frame_size/sizeof(uns16)/2 + 1) );
			numberframes--;
			printf( "Remaining Frames %i\n", numberframes );
			pl_exp_unlock_oldest_frame( cameraHandle );
			printf("Slept %i times \n", i);
			i = 0;
		}
		i++;
		
    } // End while 

	testCameraMutex->lock();
		testCameraCondition->signal();
	testCameraMutex->unlock();

}
void printError()
{
	char msg[ERROR_MSG_LEN];		// for error handling
	pl_error_message(pl_error_code(), msg);
	std::cout << "Quantix Camera error: " << msg << std::endl;
}*/