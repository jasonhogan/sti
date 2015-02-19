/*! \file CompositeEvent.cpp
 *  \author Jason Michael Hogan
 *  \brief Source-file for the class CompositeEvent
 *  \section license License
 *
 *  Copyright (C) 2009 Jason Hogan <hogan@stanford.edu>\n
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

#include "CompositeEvent.h"
#include "utils.h"
#include <string>

CompositeEvent::CompositeEvent(const STI::Types::TEvent& tEvent) : 
tEvent_ptr(&tEvent)
{
}

CompositeEvent::CompositeEvent(STI::Types::TDeviceEvent& tDeviceEvent, const STI::Types::TEvent& tEvent) : 
tDeviceEvent_(tDeviceEvent), tEvent_ptr(&tEvent)
{
}

CompositeEvent::~CompositeEvent()
{
}

CompositeEvent& CompositeEvent::operator= (const CompositeEvent& other)
{
	tDeviceEvent_ = other.tDeviceEvent_;
	tEvent_ptr = other.tEvent_ptr;
	return (*this);
}


//struct TDeviceEvent {
//	unsigned short channel;   //== STI::Types::TChannel.channel
//	double         time;
//	unsigned long  eventNum;
//	boolean        isMeasurementEvent;

//	string         description;
//	string         channelName;
//	TValMixed      value;
//	TDevicePosition      pos;

bool CompositeEvent::operator<(const CompositeEvent& other) const
{
	if( tDeviceEvent_.time < other.tDeviceEvent_.time ) return true;
	
	if( tDeviceEvent_.time == other.tDeviceEvent_.time ) {
		return ( tDeviceEvent_.channel < other.tDeviceEvent_.channel );
	}

	return false;
}

bool CompositeEvent::operator==(const CompositeEvent& other) const
{
	if(tEvent_ptr == 0 || other.tEvent_ptr == 0) {
		return false;
	}
	//tDeviceEvent_
	if( tDeviceEvent_.time != other.tDeviceEvent_.time ) return false;
	if( tDeviceEvent_.channel != other.tDeviceEvent_.channel ) return false;
	if(STI::Utils::compareTValMixed(tDeviceEvent_.value, other.tDeviceEvent_.value) == false) return false;
	
	if( tDeviceEvent_.isMeasurementEvent != other.tDeviceEvent_.isMeasurementEvent ) return false;
	if( tDeviceEvent_.eventNum != other.tDeviceEvent_.eventNum ) return false;


	std::string temp = tDeviceEvent_.description;
	if(temp.compare( other.tDeviceEvent_.description ) != 0) return false;
	temp = tDeviceEvent_.channelName;
	if(temp.compare( other.tDeviceEvent_.channelName ) != 0) return false;

	//TDevicePosition
	temp = tDeviceEvent_.pos.file;
	if(temp.compare( other.tDeviceEvent_.pos.file ) != 0) return false;
	if( tDeviceEvent_.pos.line != other.tDeviceEvent_.pos.line ) return false;

	if( tDeviceEvent_.useCallback != other.tDeviceEvent_.useCallback ) return false;
	if( tDeviceEvent_.hasDynamicValue != other.tDeviceEvent_.hasDynamicValue ) return false;

	//Should be safe to check TDeviceEvent only

	////tEvent_ptr
	//if( tEvent_ptr->time != other.tEvent_ptr->time ) return false;
	//if( tEvent_ptr->channel != other.tEvent_ptr->channel ) return false;
	//if( tEvent_ptr->isMeasurementEvent != other.tEvent_ptr->isMeasurementEvent ) return false;

	return true;
}


STI::Types::TDeviceEvent& CompositeEvent::getTDeviceEvent()
{
	return tDeviceEvent_;
}


const STI::Types::TEvent& CompositeEvent::getTEvent()
{
	return (*tEvent_ptr);
}


