/*! \file GPIB_device.cpp
 *  \author David M.S. Johnson
 *  \brief Source-file for controlling the NI GPIB dongle
 *  \section license License
 *
 *  Copyright (C) 2009 David Johnson <david.m.johnson@stanford.edu>\n
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

#include "GPIB_device.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector> //needed to be able to use vectors for data storage

//#include <winnls.h>

// #defs
//#define BD_PAD 0
#define MAX_RETRIES 100


//===========================================================================

GPIB_device::GPIB_device(std::string gpibHost, int controllerNumber)
{
	//set variable values	
	num_controllers = controllerNumber;
	BD_PAD = controllerNumber;
	
	//create handle for controller nambed gpibHost (usually "GPIB0" or "GPIB1" - no longer assumed to be in position 0 (as specified by BD_PAD), which is now set by controllerNumber
	GPIB_interface = Initialize_GPIB_Controller(gpibHost.c_str(), controllerNumber);

 
}

int GPIB_device::Initialize_GPIB_Controller(const char* interface_name, int padd)
{
	int retries=0, success=0;

	//LPWSTR interfaceNameWide; 
	//int interfaceNameWideSize = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, interface_name.c_str(), -1, interfaceNameWide, 0);

   while ( (retries<=MAX_RETRIES) && (!success) )
   {
	
	   

	   GPIB_controller[num_controllers] = ibfind (interface_name);
		GPIB_Error ("ibfind");
 
		// Ensure that GPIB-ENET/100 is configured to be system controller.
		ibrsc (GPIB_controller[num_controllers], 1);
		GPIB_Error ("ibrsc");
	
		// Ensure that GPIB-ENET/100 is configured to be at primary address padd.
		ibpad (GPIB_controller[num_controllers], padd);
		GPIB_Error ("ibpad");

		// Clear the GPIB bus and make the GPIB ENET/100 the Controller in Charge.
		
		ibsic (GPIB_controller[num_controllers]);
		GPIB_Error ("ibsic");
		

		// Raise the Remote Enable line on the GPIB bus
		ibsre (GPIB_controller[num_controllers], 1);
		GPIB_Error ("ibsre");
		
		if (ibsta&ERR)
      {
         retries++;

         // May want to put delay here before retrying.

		 Sleep(30);
      }
      else
      {
         success=1;
      }
   }
	
   return GPIB_controller[num_controllers];
}  //end Initialize_GPIBGPIB_controller

void GPIB_device::Query_Device (int padd, int sadd, char *command_string, 
									 std::string& strBuffer, int read_size)
{
   int bdhandle = GPIB_interface;
   char * buffer = new char[101];

	int retries=0, success=0;

   while ( (retries<=MAX_RETRIES) && (!success) )
   {
      // Lock only the interface we're accessing. This allows only
      // this descriptor to access the interface.  For NI-488.2 software that
      // has not added support for iblck, use iblock and ibunlock instead.
      // For example, 
      // iblock (bdhandle);
      // ibunlock (bdhandle);
      iblck(bdhandle, 1, 0, NULL);
      GPIB_Error("iblck");

      if (ibsta&ERR)
      {
         retries++;

         // May want to put delay here before retrying.
		 Sleep(30);
      }
      else
      {
         success=1;
      }
   }

   if (success)
   {
      char CmdBytes[8];
      int CmdCnt = 4;

      // Make interface a talker, requested device a listener
      CmdBytes[0] = '?';   // Unlisten
      CmdBytes[1] = '_';   // Untalk
      CmdBytes[2] = BD_PAD|0x40; // MTA GPIB-ENET/100
      CmdBytes[3] = padd|0x20;   // MLA device
      if (sadd)
      {
         CmdBytes[4] = sadd|0x60;   // MSA device
         CmdCnt++;
      }

      // Issue command sequence to set up device to listen and GPIB-ENET/100
      // to talk.

	  ibcmd(bdhandle, CmdBytes, CmdCnt);
      GPIB_Error ("ibcmd for WRITE");

      // Write requested command string to specified device.
      ibwrt (bdhandle, command_string, strlen(command_string));
      GPIB_Error ("ibwrt");

      // Make interface a listener, requested device a talker
      // CmdCnt is still setup from above.
      CmdBytes[0] = '?';   // Unlisten
      CmdBytes[1] = '_';   // Untalk
      CmdBytes[2] = BD_PAD|0x20; // MLA GPIB-ENET/100
      CmdBytes[3] = padd|0x40;   // MTA device
      if (sadd)
      {
         CmdBytes[4] = sadd|0x60;   // MSA device
      }

      // Issue command sequence to set up device to talk and GPIB-ENET/100
      // to listen.
      ibcmd(bdhandle, CmdBytes, CmdCnt);
      GPIB_Error ("ibcmd for READ");

      // Read response from specified device.
      
	  ibrd (bdhandle, buffer, read_size);
	  std::cerr << std::endl << buffer << std::endl;
      GPIB_Error ("ibrd");

      // NULL terminate response
      buffer[ibcnt]=0;

	  strBuffer = std::string(buffer); // plays nice with C++ partner objects

	  delete[] buffer;

      // Unlock the interface we've accessed, thus allowing others to
      // use the interface. For NI-488.2 software that has not added 
      // support for iblck, use iblock and ibunlock instead.
      // For example, 
      // iblock (bdhandle);
      // ibunlock (bdhandle);
      iblck(bdhandle, 0, 0, NULL);
      GPIB_Error("iblck end");
   }
   else
   {
      printf ("Unable to lock instrument. Write and Read aborted.\n");
      buffer[0]=0;
   }
}  // end Query_Device

void GPIB_device::Command_Device (int padd, int sadd, char *command_string, 
									   std::string& strBuffer, int read_size)
{
   int bdhandle = GPIB_interface;
   char * buffer = new char[101];
	int retries=0, success=0;

   while ( (retries<=MAX_RETRIES) && (!success) )
   {
      // Lock only the interface we're accessing. This allows only
      // this descriptor to access the interface.  For NI-488.2 software that
      // has not added support for iblck, use iblock and ibunlock instead.
      // For example, 
      // iblock (bdhandle);
      // ibunlock (bdhandle);
      iblck(bdhandle, 1, 0, NULL);
      GPIB_Error("iblck");

      if (ibsta&ERR)
      {
         retries++;

         // May want to put delay here before retrying.
      }
      else
      {
         success=1;
      }
   }

   if (success)
   {
      char CmdBytes[8];
      int CmdCnt = 4;

      // Make interface a talker, requested device a listener
      CmdBytes[0] = '?';   // Unlisten
      CmdBytes[1] = '_';   // Untalk
      CmdBytes[2] = BD_PAD|0x40; // MTA GPIB-ENET/100
      CmdBytes[3] = padd|0x20;   // MLA device
      if (sadd)
      {
         CmdBytes[4] = sadd|0x60;   // MSA device
         CmdCnt++;
      }

      // Issue command sequence to set up device to listen and GPIB-ENET/100
      // to talk.
      ibcmd(bdhandle, CmdBytes, CmdCnt);
      GPIB_Error ("ibcmd for WRITE");

      // Write requested command string to specified device.
      ibwrt (bdhandle, command_string, strlen(command_string));
      GPIB_Error ("ibwrt");

      
      // Unlock the interface we've accessed, thus allowing others to
      // use the interface. For NI-488.2 software that has not added 
      // support for iblck, use iblock and ibunlock instead.
      // For example, 
      // iblock (bdhandle);
      // ibunlock (bdhandle);
      iblck(bdhandle, 0, 0, NULL);
      GPIB_Error("iblck end");
   }
   else
   {
      printf ("Unable to lock instrument. Write and Read aborted.\n");
      buffer[0]=0;
   }

   strBuffer = std::string(buffer); // plays nice with C++ partner objects

	delete[] buffer;

}  // end Command_Device

void GPIB_device::Close_Handles (void)
{
   int index;

   for (index=0; index<num_controllers; index++)
   {
      // In case we exit while locked.  For NI-488.2 software that
      // has not added support for iblck, use iblock and ibunlock instead.
      // For example, 
      // iblock (bdhandle);
      // ibunlock (bdhandle);
      iblck(GPIB_controller[index],0, 0, NULL);

      // Close the driver handle to the interface.
      ibonl (GPIB_controller[index], 0);
   }
}  // end Close_Handles


void GPIB_device::GPIB_Error (char *source)
{
   // If a non-lock related error occurred in the last call to the GPIB
   // driver, print to screen and exit gracefully.
   if ( (ibsta&ERR) && (!(iberr==ELCK)) )
   {
      printf ("error %d occurred during", iberr);
      printf(" %s.\n", source);
      printf("ibsta = %x, ibcnt = %d\n", ibsta, ibcnt);
      printf("num_controllers: %d\n", num_controllers);
      //Close_Handles();
      //exit(1);                         // Exits program
   }
}  // end GPIB_Error
bool GPIB_device::readUntilTerminationCharacter (int padd, int sadd, char *command_string, std::string & result)
{
   int retries=0, success=0;
   int bdhandle = GPIB_interface;
   while ( (retries<=MAX_RETRIES) && (!success) )
   {
      // Lock only the interface we're accessing. This allows only this descriptor to access the interface.  
      iblck(bdhandle, 1, 0, NULL);
      GPIB_Error("iblck");
      if (ibsta&ERR)
      {
         retries++;
		 Sleep(30); //delay here before retrying.
      }
      else
		success=1;
   }
   if (success)
   {
      char CmdBytes[8];
      int CmdCnt = 4;
      // Make interface a talker, requested device a listener
      CmdBytes[0] = '?';   // Unlisten
      CmdBytes[1] = '_';   // Untalk
      CmdBytes[2] = BD_PAD|0x40; // MTA GPIB-ENET/100
      CmdBytes[3] = padd|0x20;   // MLA device
      if (sadd)
      {
         CmdBytes[4] = sadd|0x60;   // MSA device
         CmdCnt++;
      }
      // Issue command sequence to set up device to listen and GPIB-ENET/100 to talk.
	  ibcmd(bdhandle, CmdBytes, CmdCnt);
      GPIB_Error ("ibcmd for WRITE");
      // Write requested command string to specified device.
      ibwrt (bdhandle, command_string, strlen(command_string));
      GPIB_Error ("ibwrt");
      // Make interface a listener, requested device a talker
      // CmdCnt is still setup from above.
      CmdBytes[0] = '?';   // Unlisten
      CmdBytes[1] = '_';   // Untalk
      CmdBytes[2] = BD_PAD|0x20; // MLA GPIB-ENET/100
      CmdBytes[3] = padd|0x40;   // MTA device
      if (sadd)
      {
         CmdBytes[4] = sadd|0x60;   // MSA device
      }
      // Issue command sequence to set up device to talk and GPIB-ENET/100
      // to listen.
      ibcmd(bdhandle, CmdBytes, CmdCnt);
      GPIB_Error ("ibcmd for READ");

      // Read response from specified device.
      //Look for the # character 
	
	  char cData = '0'; // single character read
	  while (cData != '#')
	  {
		  ibrd (bdhandle, &cData, 1);
	  }
	  //Read in the character following the # character
	  ibrd (bdhandle, &cData, 1);
	  std::stringstream convertcData;
	  convertcData << std::string(&cData);

	  int bytesToRead = 0;
	  convertcData >> bytesToRead;
	  
	  //int bytesToRead = atoi(&cData); //Convert the character into a long int
	  
	  //figure out how much data to read
	  char * header = new char[bytesToRead + 1];
	  ibrd (bdhandle, header, bytesToRead);
	  header[bytesToRead] = '\0';/*Add null end of string character*/

	  std::stringstream convertHeader;
	  convertHeader << std::string(header);
	  convertHeader >> bytesToRead;//Extract # of bytes to read from header

	  ibrd (bdhandle, bigBuffer, bytesToRead + 1);
	  GPIB_Error ("ibrd");

      //NULL terminate response
      bigBuffer[ibcnt]=0;

	 
	  

      // Unlock the interface we've accessed, thus allowing others to use the interface. 
      iblck(bdhandle, 0, 0, NULL);
      GPIB_Error("iblck end");

	  delete[] header; //cleanup data
   }
   else
   {
      printf ("Unable to lock instrument. Write and Read aborted.\n");
      buffer[0]=0;
   }

    result = std::string(bigBuffer); // plays nice with C++ partner objects

	
	

	return true;
}

bool GPIB_device::readUntilNewLine (int padd, int sadd, char *command_string, std::string & result)
{
   int retries=0, success=0;
   int bdhandle = GPIB_interface;
   while ( (retries<=MAX_RETRIES) && (!success) )
   {
      // Lock only the interface we're accessing. This allows only this descriptor to access the interface.  
      iblck(bdhandle, 1, 0, NULL);
      GPIB_Error("iblck");
      if (ibsta&ERR)
      {
         retries++;
		 Sleep(30); //delay here before retrying.
      }
      else
		success=1;
   }
   if (success)
   {
      char CmdBytes[8];
      int CmdCnt = 4;
      // Make interface a talker, requested device a listener
      CmdBytes[0] = '?';   // Unlisten
      CmdBytes[1] = '_';   // Untalk
      CmdBytes[2] = BD_PAD|0x40; // MTA GPIB-ENET/100
      CmdBytes[3] = padd|0x20;   // MLA device
      if (sadd)
      {
         CmdBytes[4] = sadd|0x60;   // MSA device
         CmdCnt++;
      }
      // Issue command sequence to set up device to listen and GPIB-ENET/100 to talk.
	  ibcmd(bdhandle, CmdBytes, CmdCnt);
      GPIB_Error ("ibcmd for WRITE");
      // Write requested command string to specified device.
      ibwrt (bdhandle, command_string, strlen(command_string));
      GPIB_Error ("ibwrt");
      // Make interface a listener, requested device a talker
      // CmdCnt is still setup from above.
      CmdBytes[0] = '?';   // Unlisten
      CmdBytes[1] = '_';   // Untalk
      CmdBytes[2] = BD_PAD|0x20; // MLA GPIB-ENET/100
      CmdBytes[3] = padd|0x40;   // MTA device
      if (sadd)
      {
         CmdBytes[4] = sadd|0x60;   // MSA device
      }
      // Issue command sequence to set up device to talk and GPIB-ENET/100
      // to listen.
      ibcmd(bdhandle, CmdBytes, CmdCnt);
      GPIB_Error ("ibcmd for READ");

      // Read response from specified device.
      //Look for the # character 
	
	  char cData = '0'; // single character read
	  std::stringstream convertStream;

	  const unsigned int dataLength = 10000; 
	  char readData[dataLength];

	  /*
	  while (cData != '\n')
	  {
			ibrd (bdhandle, &cData, 1);
			GPIB_Error ("ibrd");
			testStream << cData;
			//terminatedData[0] = cData;
			//terminatedData[1] = '\0';
	  }
	  */
	    ibrd (bdhandle, readData, dataLength);
		GPIB_Error ("ibrd");
		//convertStream << readData;

	  //testStream >> result;
	  //result = convertStream.str();
		result = std::string(readData);
		result.erase(result.find_first_of('\n'));

      // Unlock the interface we've accessed, thus allowing others to use the interface. 
      iblck(bdhandle, 0, 0, NULL);
      GPIB_Error("iblck end");


   }
   else
   {
      printf ("Unable to lock instrument. Write and Read aborted.\n");
   }	

	return true;
}