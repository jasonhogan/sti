/*************************************************************************
 *
 * Name:   ENET_GPIB_device.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to control a generic GPIB device connected via the NI ENET-100 hub 
 *
 * David Johnson 6/4/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 
#include <iostream>
#include <cstring>

#include "ni4882.h"


#include "ENET_GPIB_device.h"

// #defs
#define BD_PAD 0
#define MAX_RETRIES 100


//===========================================================================

ENET_GPIB_device::ENET_GPIB_device()
{
	//set variable values	
	num_controllers=0;
	
	//create handle for ENET/100 controller nambed "GPIB0" - assumed to be in position 0 (as specified by BD_PAD)
	GPIBinterface = Initialize_GPIBENET_Controller ("GPIB0", BD_PAD);

 
}

//===========================================================================

int ENET_GPIB_device::Initialize_GPIBENET_Controller(char *interface_name, int padd)
{
	int retries=0, success=0;

   while ( (retries<=MAX_RETRIES) && (!success) )
   {
	
		enet_controller[num_controllers] = ibfind (interface_name);
		GPIB_Error ("ibfind");
 
		// Ensure that GPIB-ENET/100 is configured to be system controller.
		ibrsc (enet_controller[num_controllers], 1);
		GPIB_Error ("ibrsc");
	
		// Ensure that GPIB-ENET/100 is configured to be at primary address padd.
		ibpad (enet_controller[num_controllers], padd);
		GPIB_Error ("ibpad");

		// Clear the GPIB bus and make the GPIB ENET/100 the Controller in Charge.
		
		ibsic (enet_controller[num_controllers]);
		GPIB_Error ("ibsic");
		

		// Raise the Remote Enable line on the GPIB bus
		ibsre (enet_controller[num_controllers], 1);
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
	
   return enet_controller[num_controllers];
}  //end Initialize_GPIBENET_Controller
//===========================================================================

void ENET_GPIB_device::Query_Device (int bdhandle, int padd, int sadd, char *command_string, 
		   char *buffer, int read_size)
{
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
      GPIB_Error ("ibrd");

      // NULL terminate response
      buffer[ibcnt]=0;

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

//===========================================================================



void ENET_GPIB_device::Command_Device (int bdhandle, int padd, int sadd, char *command_string, 
		   char *buffer, int read_size)
{
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
}  // end Command_Device

//===========================================================================


void ENET_GPIB_device::Close_Handles (void)
{
   int index;

   for (index=0; index<num_controllers; index++)
   {
      // In case we exit while locked.  For NI-488.2 software that
      // has not added support for iblck, use iblock and ibunlock instead.
      // For example, 
      // iblock (bdhandle);
      // ibunlock (bdhandle);
      iblck(enet_controller[index],0, 0, NULL);

      // Close the driver handle to the interface.
      ibonl (enet_controller[index], 0);
   }
}  // end Close_Handles

//===========================================================================


void ENET_GPIB_device::GPIB_Error (char *source)
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