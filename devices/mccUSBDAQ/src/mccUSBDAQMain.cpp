/*! \file mccUSBDAQMain.cpp
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
#include "mccUSBDAQDevice.h"
#include "TaggedConfigFile.h"


using namespace std;


ORBManager* orbManager;

void getInstalledBoards(int &numBoards, std::vector <int> &boardNums);
void getCommaColonMap(std::vector<std::string> &field, std::map<std::string, std::string> &fieldMap);

int main(int argc, char **argv)
{
	orbManager = new ORBManager(argc, argv);    	

	int numBoards = 0;
	std::vector<int> boardNums;
	int module = 0;

	//Get path of config file
	boost::filesystem::path abs_path = boost::filesystem::complete("../mccUSBDAQ/src/mccUSBDAQ.ini");
	abs_path = abs_path.normalize();
	std::string configFilePath = abs_path.native_file_string();
	
	std::string deviceName;

	getInstalledBoards(numBoards, boardNums);

	TaggedConfigFile taggedConfigFile(configFilePath);

	//Get device names; mostly a legacy for usb1408fs
	std::vector <std::string> field;
	std::map<std::string, std::string> nameMap;
	taggedConfigFile.getField("DAQDeviceName", field);
	getCommaColonMap(field, nameMap);

	std::vector <std::string> ipAddressField;
	taggedConfigFile.getField("ipAddress", ipAddressField);
	if (ipAddressField.empty())
		ipAddressField.push_back("eplittletable.stanford.edu");


	MccUSBDAQDevice *mccUSBDAQDevice;
	for (unsigned int i = 0; i < boardNums.size(); i++)
	{
		if (nameMap.find(STI::Utils::valueToString(boardNums.at(i))) == nameMap.end())
		{
			int  BoardType;
			cbGetConfig (BOARDINFO, boardNums.at(i), 0, BISERIALNUM, &BoardType);
			deviceName = "MCC USB DAQ Module " + STI::Utils::valueToString(BoardType);
		} else {
			deviceName = nameMap.find(STI::Utils::valueToString(boardNums.at(i)))->second;
		}

		cbGetConfig(BOARDINFO, boardNums.at(i), 0, BISERIALNUM, &module);

		mccUSBDAQDevice = new MccUSBDAQDevice(orbManager, deviceName, ipAddressField.at(0), module, boardNums.at(i));
	}
	orbManager->run();
	
	return 0;
}

void getInstalledBoards(int &numBoards, std::vector <int> &boardNums)
{
	int  ULStat = 0;
    int  BoardNum;
    char BoardNameStr[BOARDNAMELEN];
    float    RevLevel = (float)CURRENTREVNUM;
    int  BoardType;

    /* Get the number of boards installed in system */
    cbGetConfig (GLOBALINFO, 0, 0, GINUMBOARDS, &numBoards);

    /* Print out board type name of each installed board */
    for (BoardNum=0; BoardNum<numBoards; BoardNum++)
    {
        /* Get board type of each board */
        cbGetConfig (BOARDINFO, BoardNum, 0, BIBOARDTYPE, &BoardType);
        
        /* If a board is installed, select it only if it's a USB type */
        if (BoardType > 0)
        {
            cbGetBoardName (BoardNum, BoardNameStr);
			if (BoardNameStr[0] == 'U' && BoardNameStr[1] == 'S' &&
				BoardNameStr[2] == 'B')
				boardNums.push_back(BoardNum);
        }
    }
}

void getCommaColonMap(std::vector<std::string> &field, std::map<std::string, std::string> &fieldMap)
{
	std::size_t channelBegin;
	std::size_t channelEnd;
	std::string first;
	std::string second;

	std::vector<std::string>::iterator it;
	for (it = field.begin(); it != field.end(); it++)
	{
		channelBegin = it->find_first_not_of(" ");
		
		if (channelBegin == std::string::npos)
			continue;

		channelEnd = it->find_first_of(",", channelBegin + 1);

		if (channelEnd == std::string::npos)
			continue;

		first = it->substr(channelBegin, channelEnd - channelBegin);

		channelBegin = it->find_first_not_of(" ", channelEnd + 1);
//		channelBegin = channelEnd + 1;

		if (channelBegin == std::string::npos)
			continue;

		channelEnd = it->find_first_of(";", channelBegin + 1);

		second = it->substr(channelBegin, channelEnd-channelBegin);

		fieldMap[first] = second;
	}

	return;
}