/*! \file main.cpp
 *  \author Alexander Franz Sugarbaker
 *  \brief main() source-file for the stf_da_slow program
 *  \section license License
 *
 *  Copyright (C) 2008 Alex Sugarbaker <sugarbak@stanford.edu>
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

//#include <iostream>
#include "stf_da_slow.h"

using namespace std;

int main(int argc, char* argv[]){
	STF_DA_SLOW::da_slow test_slow(0x90000054);

	int chan;
	double val;

	std::cout << "Enter -1 for channel to reset all to 0V" << std::endl;

	while(1){
		cout << "Channel: ";
		cin >> chan;
		cout << "Value: ";
		cin >> val;
		
		if (chan == -1 ) 
			test_slow.reset();
		else if ((chan>-1) && (chan<40) && (val>=-10) && (val<=10))
			test_slow.set_value(chan, val);
		else {
			std::cerr << "Your channel or value is out of range." << std::endl;
			return 1;
		}
	}

	return 0;
}

