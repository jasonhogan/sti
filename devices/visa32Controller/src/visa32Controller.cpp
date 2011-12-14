/*! \file visa32Controller.cpp
 *  \author David M.S. Johnson
 *  \brief header file
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
 *  This file is part of the Stanford Timing Interface (STI).
 *
 *  This serial communications is based on code from: Ramon de Klein (Ramon.de.Klein@ict.nl)
 *  Code was downloaded from: http://www.codeproject.com/KB/system/serial.aspx
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

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
/* Functions like strcpy are technically not secure because they do */
/* not contain a 'length'. But we disable this warning for the VISA */
/* examples since we never copy more than the actual buffer size.   */
#define _CRT_SECURE_NO_DEPRECATE
#endif


#include "visa32Controller.h"
#include <iostream>
#include <vector>



visa32Controller::visa32Controller(std::string comportString)
{
	
	ViStatus retErr;
    ViUInt32 retCnt, ii;
    ViChar cmd[256], rcv[256], idn[256];
    int i;

	//std::ostringstream oss; // create a stream

	std::vector<std::string> deviceIDs;
	std::string tempID;

	/*
    * First we must call viOpenDefaultRM to get the manager
    * handle.  We will store this handle in defaultRM.
    */
   status=viOpenDefaultRM (&defaultRM);
   if (status < VI_SUCCESS)  
   {
      printf ("Could not open a session to the VISA Resource Manager!\n");
      exit (EXIT_FAILURE);
   }
   /* Find all the USB TMC VISA resources in our system and store the  */
   /* number of resources in the system in numInstrs.                  */
   status = viFindRsrc (defaultRM, "USB?*INSTR", &findList, &numInstrs, instrResourceString);
   
   if (status < VI_SUCCESS)
   {
      printf ("An error occurred while finding resources.\nHit enter to continue.");
      fflush(stdin);
      getchar();
      viClose (defaultRM);
      //return status;
   }

      /*
    * Now we will open VISA sessions to all USB TMC instruments.
    * We must use the handle from viOpenDefaultRM and we must   
    * also use a string that indicates which instrument to open.  This
    * is called the instrument descriptor.  The format for this string
    * can be found in the function panel by right clicking on the 
    * descriptor parameter. After opening a session to the
    * device, we will get a handle to the instrument which we 
    * will use in later VISA functions.  The AccessMode and Timeout
    * parameters in this function are reserved for future
    * functionality.  These two parameters are given the value VI_NULL.
    */

   
   
   
	
}
visa32Controller::~visa32Controller()
{
	// Close the port again

	status = viClose (instr);  

}

std::string visa32Controller::queryDevice(std::string commandString, int bufferLength /* = 100 */)
{
	//strcpy (stringinput, commandString);
	std::ostringstream oss;
	std::string result = "Error Code 1: Operation Failed.";
	//unsigned char * bufferNew = new unsigned char[bufferLength + 1];

	unsigned char buffer[200]; // = *bufferNew;

	//	unsigned char buffer[bufferLength];


	if(numInstrs > 0)
   {
		//viFindNext (findList, instrResourceString);

		status = viOpen (defaultRM, instrResourceString, VI_NULL, VI_NULL, &instr);

		if (status < VI_SUCCESS) 
		{
			printf ("Cannot open a session to the device.\n");
			//continue;
		}
   }
	

	strcpy (stringinput,commandString.c_str());
	status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
    if (status < VI_SUCCESS)    
    {
		printf ("Error writing to the device.\n");
        status = viClose (instr);
        //continue;
    }
    
    /*
     * Now we will attempt to read back a response from the device to
     * the identification query that was sent.  We will use the viRead
     * function to acquire the data.  We will try to read back 100 bytes.
     * This function will stop reading if it finds the termination character
     * before it reads 100 bytes.
     * After the data has been read the response is displayed.
     */
    status = viRead (instr, buffer, 200, &retCount);
    if (status < VI_SUCCESS) 
    {
		printf ("Error reading a response from the device.\n");
    }
    else
    {
		oss << buffer;
		result = oss.str();
		size_t length = result.size();
		size_t found;
		size_t found2;
		bool foundValid = false;
		bool found2Valid = false;
		found=result.find("Í");
		found2=result.find("Ì");
		if (found != std::string::npos)
			foundValid = true;
		if (found2 != std::string::npos)
			found2Valid = true;
		if (foundValid && found2Valid)
			if(found < found2)
				result.erase(found, length);
			else
				result.erase(found2, length);
		else if (foundValid && !found2Valid)
			result.erase(found, length);
		else if (found2Valid && !foundValid)
			result.erase(found2, length);

		std::cerr << "Device: " << result << std::endl;
    }

	
	status = viClose (instr);  

	//delete[] bufferNew;

	return result;
}
std::string visa32Controller::readData(std::string commandString)
{
	ViStatus retErr;
    ViUInt32 retCnt, ii;
    ViChar cmd[256], rcv[256], idn[256];
    int i;
	
	//strcpy (stringinput, commandString);
	std::ostringstream oss;
	std::string result = "Error Code 1: Operation Failed.";
	//unsigned char * bufferNew = new unsigned char[bufferLength + 1];

	unsigned char curve[20000]; // = *bufferNew;

	//	unsigned char buffer[bufferLength];


	if(numInstrs > 0)
   {
		//viFindNext (findList, instrResourceString);

		status = viOpen (defaultRM, instrResourceString, VI_NULL, VI_NULL, &instr);

		if (status < VI_SUCCESS) 
		{
			printf ("Cannot open a session to the device.\n");
			//continue;
		}
   }
	

	strcpy (stringinput,commandString.c_str());   
	
    retErr = viWrite(instr, (ViPBuf)stringinput, strlen(stringinput), &retCnt);
    if(retErr < VI_SUCCESS) {
          viStatusDesc(instr, retErr, rcv);rcv[strlen(rcv)] = '\0';
          printf("%s\n", rcv);
          getchar();
       }
       //Get the next 5 buffers of data...
       viSetAttribute(instr, VI_ATTR_TMO_VALUE, 30000L);
       retErr = viRead(instr, (ViPBuf)curve, 200000, &retCnt);
       if(retErr < VI_SUCCESS) {
          viStatusDesc(instr, retErr, rcv);rcv[strlen(rcv)] = '\0';
          printf("%s\n", rcv);
          getchar();
		  }
       else
    {
		oss << curve;
		result = oss.str();
		size_t length = result.size();
		size_t found;
		size_t found2;
		bool foundValid = false;
		bool found2Valid = false;
		found=result.find("Í");
		found2=result.find("Ì");
		if (found != std::string::npos)
			foundValid = true;
		if (found2 != std::string::npos)
			found2Valid = true;
		if (foundValid && found2Valid)
			if(found < found2)
				result.erase(found, length);
			else
				result.erase(found2, length);
		else if (foundValid && !found2Valid)
			result.erase(found, length);
		else if (found2Valid && !foundValid)
			result.erase(found2, length);

		//std::cerr << "Device: " << result << std::endl;
    }
	   
       viSetAttribute(instr, VI_ATTR_TMO_VALUE, 10000L);
       //Get the instrument back to a talker/listener...
       retErr = viClear(instr);
       
       //Check for errors (use error_query in the driver..or whatever the call is.
       sprintf(cmd, "*esr?");
       retErr = viWrite(instr, (ViPBuf)cmd, strlen(cmd), &retCnt);
       if(retErr < VI_SUCCESS) {
          viStatusDesc(instr, retErr, rcv);rcv[strlen(rcv)] = '\0';
          printf("%s\n", rcv);
          getchar();
       }
       
       retErr = viRead(instr, (ViPBuf)curve, 10000, &retCnt);
       if(retErr < VI_SUCCESS) {
          viStatusDesc(instr, retErr, rcv);rcv[strlen(rcv)] = '\0';
          printf("%s\n", rcv);
          getchar();
       }
       else if(rcv[0] != '0')
       {
          sprintf(cmd, "allev?");
          retErr = viWrite(instr, (ViPBuf)cmd, strlen(cmd), &retCnt);
          if(retErr < VI_SUCCESS) {
             viStatusDesc(instr, retErr, rcv);rcv[strlen(rcv)] = '\0';
             printf("%s\n", rcv);
             getchar();
          }
       
          retErr = viRead(instr, (ViPBuf)rcv, 255, &retCnt);
          if(retErr < VI_SUCCESS) {
             viStatusDesc(instr, retErr, rcv);rcv[strlen(rcv)] = '\0';
             printf("%s\n", rcv);
             getchar();
          }
       
       }
       //Get the ID again to be sure all is well again...
       sprintf(cmd, "*idn?");
       retErr = viWrite(instr, (ViPBuf)cmd, strlen(cmd), &retCnt);
       if(retErr < VI_SUCCESS) {
          viStatusDesc(instr, retErr, rcv);rcv[strlen(rcv)] = '\0';
          printf("%s\n", rcv);
          getchar();
       }
       
       retErr = viRead(instr, (ViPBuf)idn, 255, &retCnt);
       if(retErr < VI_SUCCESS) {
          viStatusDesc(instr, retErr, rcv);rcv[strlen(rcv)] = '\0';
          printf("%s\n", rcv);
          getchar();
       }
       else
       {
          rcv[retCnt] = '\0';
          printf("%s\nAll is ok now again...\n", idn);
       }

	status = viClose (instr);  

	//delete[] bufferNew;

	return result;
}
void visa32Controller::commandDevice(std::string commandString)
{
	//strcpy (stringinput, commandString);
	std::ostringstream oss;
	std::string result = "Error Code 1: Operation Failed.";
	unsigned char buffer[100];


	if(numInstrs > 0)
	{
		//viFindNext (findList, instrResourceString);

		status = viOpen (defaultRM, instrResourceString, VI_NULL, VI_NULL, &instr);

		if (status < VI_SUCCESS) 
		{
			printf ("Cannot open a session to the device.\n");
			//continue;
		}
	}
	

	strcpy (stringinput,commandString.c_str());
	status = viWrite (instr, (ViBuf)stringinput, (ViUInt32)strlen(stringinput), &writeCount);
    if (status < VI_SUCCESS)    
    {
		printf ("Error writing to the device.\n");
        status = viClose (instr);
        //continue;
    }
	status = viClose (instr);
}
int visa32Controller::ShowError (int error, std::string errorMessage)
{
	std::cerr << "Error Code: " << error << " with error message: " << errorMessage << std::endl;
	return 1;
}
