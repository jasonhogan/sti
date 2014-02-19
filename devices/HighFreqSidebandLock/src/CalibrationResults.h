
#ifndef CALIBRATIONRESULTS_H
#define CALIBRATIONRESULTS_H


#include "WolframRTL.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "MixedData.h"

class CalibrationResults;
typedef boost::shared_ptr<CalibrationResults> CalibrationResults_ptr;

class CalibrationResults
{
public:
	CalibrationResults();

	bool savePeaks(const WolframLibraryData& libData, MTensor* calPeaks);
	bool getPeaks(const WolframLibraryData& libData, MTensor* calPeaks);
	
	void getPeakValues(MixedData& calPeaks);

	double calibrationFSR();
	void overrideFSR(double fsr_s);

	double calibrationHeight();
	void overrideCalibrationHeight(double height);

private:

	bool setMTensorPeakValue(const WolframLibraryData& libData, MTensor* calPeaks, 
									  int peakNumber, int pos, double value);
	bool getMTensorPeakValue(const WolframLibraryData& libData, MTensor* calPeaks, 
									  int peakNumber, int pos, double* value);

	void setLeftPeak(double x, double y);
	void setRightPeak(double x, double y);

	//Peak values in (seconds, volts)
	double left[2];
	double right[2];

	mutable boost::shared_mutex calMutex;

};

#endif
