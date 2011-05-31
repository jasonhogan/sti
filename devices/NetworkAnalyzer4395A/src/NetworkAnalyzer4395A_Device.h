/*! \file NetworkAnalyzer4395A_Device.h
 *  \author Jason Hogan
 *  \brief header file for NetworkAnalyzer4395A_Device class
 *  \section license License
 *
 *  Copyright (C) 2011 Jason Hogan <hogan@stanford.edu>\n
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


#ifndef NetworkAnalyzer4395A_Device_H
#define NetworkAnalyzer4395A_Device_H


#include "GPIB_Device.h"
#include <map>
#include <string>


class NetworkAnalyzer4395A_Device : public GPIB_Device
{
public:
	
	NetworkAnalyzer4395A_Device(ORBManager* orb_manager, 
							std::string    DeviceName, 
							std::string    Address, 
							unsigned short ModuleNumber,
							std::string    logDirectory = "//atomsrv1/EP/Data/deviceLogFiles",
							std::string    GCipAddress = "eplittletable.stanford.edu",
							unsigned short GCmoduleNumber = 0);
	~NetworkAnalyzer4395A_Device() {};

private:

// Device main()
	bool deviceMain(int argc, char** argv) {return false;};    //called in a loop while it returns true

    // Device Attributes
	void defineGpibAttributes();
	void defineAttributes() {};
	void refreshAttributes() {};
	bool updateAttribute(std::string key, std::string value) {return false;};

    // Device Channels
    void defineChannels();
	bool readChannel(unsigned short channel, const MixedValue& valueIn, MixedData& dataOut);
	bool writeChannel(unsigned short channel, const MixedValue& value);

	void parseDeviceEvents(const RawEventMap& eventsIn, SynchronousEventVector& eventsOut) throw(std::exception);

	void checkSweepSegmentParamters(double fStart, double fStop, double segmentTime, const RawEvent& evt) throw(std::exception);

	// Device Command line interface setup
	std::string execute(int argc, char** argv);


	unsigned short primaryAddress;
	unsigned short secondaryAddress;
	std::string gpibID;

	double frequency;
	double power;

	double minimumEventSpacing;
	double minimumAbsoluteStartTime;

private:
	
	class SweepSegment
	{
	public:
		SweepSegment() : startFreq_l(0), stopFreq_l(0), points_l(0) {}
		SweepSegment(double startFreq, double stopFreq, unsigned points) 
			: startFreq_l(startFreq), stopFreq_l(stopFreq), points_l(points) {}

		double getStart() { return startFreq_l; }
		double getStop() { return stopFreq_l; }
		unsigned getPoints() { return points_l; }

	private:
		double startFreq_l;
		double stopFreq_l;
		unsigned points_l;
	};


	class NetworkAnalyzerEvent : public SynchronousEvent
	{
	public:
		NetworkAnalyzerEvent(double time, double totalSweepTime, NetworkAnalyzer4395A_Device* device) 
			: SynchronousEvent(time, device), sweepTime(totalSweepTime), networkAnalyzer(device), device4395A_(device) {}

		void setupEvent() { };
		void loadEvent();
		void playEvent();
		void collectMeasurementData() { };

		void addSegment(double startFreq, double stopFreq, unsigned points)
		{
			SweepSegment segment(startFreq, stopFreq, points);
			segments.push_back(segment);
		}
		void setTotalSweepTime(double time)
		{
			sweepTime = time;
		}

	private:
		
		bool sendGPIBcommand(std::string command)
		{
			return device4395A_->commandDevice(command);
		}

		NetworkAnalyzer4395A_Device* networkAnalyzer;
		std::vector<SweepSegment> segments;
		double sweepTime;

		NetworkAnalyzer4395A_Device* device4395A_;
	};

};

#endif

