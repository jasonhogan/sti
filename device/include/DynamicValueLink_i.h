/*! \file DynamicValueLink_i.h
 *  \author Jason Michael Hogan
 *  \brief Include-file for the class DynamicValueLink_i
 *  \section license License
 *
 *  Copyright (C) 2013 Jason Hogan <hogan@stanford.edu>\n
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

#ifndef DYNAMICVALUELINK_I_H
#define DYNAMICVALUELINK_I_H

#include "device.h"

#include "DynamicValue.h"

#include <boost/shared_ptr.hpp>

class DynamicValueLink_i;
typedef boost::shared_ptr<DynamicValueLink_i> DynamicValueLink_i_ptr;


class DynamicValueLink_i : public POA_STI::Server_Device::DynamicValueLink, public DynamicValueListener
{
public:

	DynamicValueLink_i(const DynamicValue_ptr& value);
	DynamicValueLink_i(const DynamicValue_ptr& value, 
		const STI::Server_Device::DynamicValueLink_var& dynamicValueLinkRef);
	~DynamicValueLink_i();
	
	void addLink(STI::Server_Device::DynamicValueLink_ptr link);
	void unLink();
	void refreshLinkedValue(const STI::Server_Device::TNetworkDynamicValueEvent& evt);

	//DynamicValueListener
	void refresh(const DynamicValueEvent& evt);

private:

	bool hasLinkTarget;
	bool isLinkedToRemoteSource;

	STI::Server_Device::DynamicValueLink_var dynamicValueLink;
	DynamicValue_ptr dynamicValue;

};

#endif
