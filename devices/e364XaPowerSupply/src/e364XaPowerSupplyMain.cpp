/*! \file usb1408fsMain.cpp
 *  \author David M.S. Johnson
 *  \brief main()
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

#include <string>
#include <iostream>

#include <ORBManager.h>
#include "e364XaPowerSupplyDevice.h"


using namespace std;


ORBManager* orbManager;

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);    

	//unsigned short module = 17;
	//unsigned short comPort = 3;

	e364XaPowerSupplyDevice supply3dX(orbManager, "e364Xa 3DX", "168.0.0.1", 17, 3);
	//e364XaPowerSupplyDevice supply3dY(orbManager, "e364Xa 3DY", "168.0.0.1", 18, 4);
	//e364XaPowerSupplyDevice supply3dZ(orbManager, "e364Xa 3DZ", "168.0.0.1", 19, 5);
	//e364XaPowerSupplyDevice supply2dTrim(orbManager, "e364Xa 2D Trim", "168.0.0.1", 20, 6);
	//e364XaPowerSupplyDevice supply2dIoffe1(orbManager, "e364Xa 2D Ioffe 1", "168.0.0.1", 21, 7);
	//e364XaPowerSupplyDevice supply2dIoffe2(orbManager, "e364Xa 2D Ioffe 2", "168.0.0.1", 22, 8);
	
	orbManager->run();
	
	return 0;
}

