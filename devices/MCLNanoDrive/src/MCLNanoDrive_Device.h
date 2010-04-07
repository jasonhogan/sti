/*! \file MCLNanoDrive_Device.h
 *  \author Susannah Dickerson
 *  \brief Include-file for the class MCLNanoDrive_Device
 *  \section license License
 *
 *  Copyright (C) 2010 Susannah Dickerson <sdickers@stanford.edu>\n
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

#ifndef MCLNANODRIVE_DEVICE_H
#define MCLNANODRIVE_DEVICE_H

#include <STI_Device.h>
#include "Madlib.h"
#include "RCSTipTiltZ.h"

class MCLNanoDrive_Device : public STI_Device
{
public:

    //***************BEGIN STI_Device functions***************//

    MCLNanoDrive_Device(ORBManager* orb_manager,  std::string    DeviceName, 
             std::string Address,    unsigned short ModuleNumber);
    ~MCLNanoDrive_Device();
    
    // Device main()
    bool deviceMain(int argc, char** argv);    //called in a loop while it returns true

    // Device Attributes
    void defineAttributes();
    void refreshAttributes();
    bool updateAttribute(std::string key, std::string value);

    // Device Channels
    void defineChannels();
    bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
    bool writeChannel(unsigned short channel, const MixedValue& value);

    // Device Command line interface setup
    void definePartnerDevices();
    std::string execute(int argc, char** argv);

    // Device-specific event parsing
    void parseDeviceEvents(const RawEventMap& eventsIn, 
        SynchronousEventVector& eventsOut) throw(std::exception);

	// Event Playback control
	void stopEventPlayback();	//for devices that require non-generic stop commands
	void pauseEventPlayback() {};
	void resumeEventPlayback() {};

    //****************END STI_Device functions***************//

	class MCLEvent : public SynchronousEvent
	{
	public:

		MCLEvent(double time, MCLNanoDrive_Device* device, double t, double p, double zt) 
			: SynchronousEvent(time, device), MCLdevice_(device), theta(t), phi(p), z(zt), eventSuccess(true)  {}
		~MCLEvent() {}

		void setupEvent() { }
		void loadEvent() { }	//no need to load since they aren't on the FPGA
		void playEvent();
		void collectMeasurementData();

		//void waitBeforeCollectData();

		double theta;
		double phi;
		double z;
		bool eventSuccess;

		MCLNanoDrive_Device* MCLdevice_;

	private:
	};

	bool initialized;
    // Additional MCLNanoDrive_Device class content...
private:

	class attributeMCL {
	public:
		double value;
		std::string tag;
	};

	class axisInfo {
	public:

		attributeMCL *attribute;
		int index;

		void setInfo(attributeMCL *attr, int i) {attribute = attr; index = i;}
	};

	int handle;
	struct ProductInformation pi;

	std::vector <axisInfo> axes;
	double xRange, yRange, zRange;

	std::vector <double> angles;

	attributeMCL setX, setY, setZ, setth, setph, setz, X, Y, Z, th, ph, z;

	bool initializeDevice();
	bool getPositions();
	bool setPosition(std::string key, double positionUM);
	bool setAngle(std::string key, double angle);
	bool setAngles(double thetaIn, double phiIn, double zIn); // for playing events
	bool getAngles();
	bool getAngles(double &thetaOut, double &phiOut, double &zOut);
	bool updateSetThPhZ();
	bool printError(int error);
	bool inRange(double theta, double phi, double zt);

	/* Unused functions
	bool getPosition(std::string key);
	bool updateThPhZ();
	*/

	RCSTipTiltZ rcs;
};

#endif
