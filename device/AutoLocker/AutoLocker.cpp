/*************************************************************************
 *
 * Name:   AutoLocker.cpp
 *
 * C++ Windows source code for Stanford Timing Interface to auto-lock the master laser to Rb
 *
 * David Johnson 7/8/2008
 * Kasevich Group - Stanford University
 *
 *
 **************************************************************************/

// Include files 

#include "AutoLocker.h"

//===========================================================================

AUTOLOCKER::AUTOLOCKER()
{
	//stuff	


}

//===========================================================================

void AUTOLOCKER::enable_lock() 
{

	system("putty -load \"ep-timing1\" -m enable_lock.txt");

}

//===========================================================================

void AUTOLOCKER::disable_lock() 
{

	system("putty -load \"ep-timing1\" -m disable_lock.txt");

}
