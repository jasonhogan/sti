
#include "CalibrationResults.h"
#include <math.h>

CalibrationResults::CalibrationResults()
{
	setLeftPeak(0, 0);
	setRightPeak(0, 0);
}

//FSR in seconds
double CalibrationResults::calibrationFSR()
{
//	boost::unique_lock< boost::shared_mutex > writeLock(calMutex);

	return fabs(left[0] - right[0]);
}
//Calibration peak mean height in Volts
double CalibrationResults::calibrationHeight()
{
//	boost::unique_lock< boost::shared_mutex > writeLock(calMutex);

	return fabs((left[1] + right[1]) / 2.0);
}

//Sets the FSR when we want to override the last calibration trace result
void CalibrationResults::overrideFSR(double fsr_s)
{
	right[0] = left[0] + fsr_s;
}

//Sets the Calibration peak mean height when we want to override the last calibration trace result
void CalibrationResults::overrideCalibrationHeight(double height)
{
	left[1] = height;
	right[1] = height;
}

void CalibrationResults::setLeftPeak(double x, double y)
{
//	boost::unique_lock< boost::shared_mutex > writeLock(calMutex);

	left[0] = x;
	left[1] = y;
}

void CalibrationResults::setRightPeak(double x, double y)
{
//	boost::unique_lock< boost::shared_mutex > writeLock(calMutex);

	right[0] = x;
	right[1] = y;
}

bool CalibrationResults::getPeaks(const WolframLibraryData& libData, MTensor* calPeaks)
{
//	boost::unique_lock< boost::shared_mutex > writeLock(calMutex);

	bool success = true;

	success &= setMTensorPeakValue(libData, calPeaks, 1, 1, left[0]);	
	success &= setMTensorPeakValue(libData, calPeaks, 1, 2, left[1]);

	success &= setMTensorPeakValue(libData, calPeaks, 2, 1, right[0]);	
	success &= setMTensorPeakValue(libData, calPeaks, 2, 2, right[1]);

	return success;
}

void CalibrationResults::getPeakValues(MixedData& calPeaks)
{
//	boost::unique_lock< boost::shared_mutex > writeLock(calMutex);

	MixedData leftPeak;
	leftPeak.addValue(left[0]);
	leftPeak.addValue(left[1]);
	
	MixedData rightPeak;
	rightPeak.addValue(right[0]);
	rightPeak.addValue(right[1]);

	calPeaks.addValue(leftPeak);
	calPeaks.addValue(rightPeak);
}


bool CalibrationResults::setMTensorPeakValue(const WolframLibraryData& libData, MTensor* calPeaks, 
									  int peakNumber, int pos, double value)
{
	if(calPeaks == NULL) {
		return false;
	}

	mint lens[2];
	int err;
	
	lens[0] = peakNumber;
	lens[1] = pos;
	err = libData->MTensor_setReal(*calPeaks, lens, value);

	return (err == 0);
}

bool CalibrationResults::savePeaks(const WolframLibraryData& libData, MTensor* calPeaks)
{
//	boost::unique_lock< boost::shared_mutex > writeLock(calMutex);

	bool success = true;

	double leftX;
	double leftY;
	double rightX;
	double rightY;

	success &= getMTensorPeakValue(libData, calPeaks, 1, 1, &leftX);	
	success &= getMTensorPeakValue(libData, calPeaks, 1, 2, &leftY);

	success &= getMTensorPeakValue(libData, calPeaks, 2, 1, &rightX);	
	success &= getMTensorPeakValue(libData, calPeaks, 2, 2, &rightY);

	double newFSR = fabs(leftX - rightX);
	double fractionalChangeFSR = fabs( 1 - (newFSR / calibrationFSR()) );

	double dLeftFrac = fabs( 1 - (leftY / left[1]) );
	double dRightFrac = fabs( 1 - (rightY / right[1]) );

	success &= (leftX != rightX) && (fractionalChangeFSR < 0.5) && (dLeftFrac < 0.25) && (dRightFrac < 0.25);

	if(success) {
		setLeftPeak(leftX, leftY);
		setRightPeak(rightX, rightY);
	}

	return success;
}

bool CalibrationResults::getMTensorPeakValue(const WolframLibraryData& libData, MTensor* calPeaks, 
									  int peakNumber, int pos, double* value)
{
	if(calPeaks == NULL) {
		return false;
	}

	mint lens[2];
	int err;
	
	lens[0] = peakNumber;
	lens[1] = pos;
	err = libData->MTensor_getReal(*calPeaks, lens, value);

	return (err == 0);
}
