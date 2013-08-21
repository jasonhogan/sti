
#include "MathematicaPeakFinder.h"

#include "findCalibration.h"
#include "findCarrierAndSidebands.h"
#include "getFeedbackSignals.h"

#include <iostream>
using std::cout;
using std::endl;

MathematicaPeakFinder::MathematicaPeakFinder()
{
}

bool MathematicaPeakFinder::findCalibrationPeaks(const STI::Types::TDataMixedSeq& rawCalData, double FSR_s, double minimumX,
												 const CalibrationResults_ptr& calResults)
{
	WolframLibraryData libData = 0;
	WolframRTL_initialize(WolframLibraryVersion);
	libData = WolframLibraryData_new(WolframLibraryVersion);

	//Setup arguments

	MTensor formatedScopeCalibrationData;		//List of {x,y} pairs, with gaps when y is below threshold

	if(!convertRawScopeData(libData, rawCalData, formatedScopeCalibrationData)) {
		return false;
	}

	////test
	//mint lens[2];
	//int err2;
	//double value;
	//
	//lens[0] = 4;
	//for(unsigned j = 1; j < 50; j++) {
	//	lens[0] = j;

	//	lens[1] = 1;
	//	err2 = libData->MTensor_getReal(formatedScopeCalibrationData, lens, &value);
	//	cout << "(" << value <<", ";
	//	lens[1] = 2;
	//	err2 = libData->MTensor_getReal(formatedScopeCalibrationData, lens, &value);
	//	cout << value << ")" << endl;
	//}


	MTensor calibration;
	int err;
	mint type = MType_Real;
	mint rank = 2;
	mint dims[2];
	dims[0] = 2;
	dims[1] = 2;
	err = libData->MTensor_new(type, rank, dims, &calibration);
	MTensor* result = &calibration;
	
	cout << "In Peak Finder: error = " << err << endl;

	if(err == 0) {
		Initialize_findCalibration(libData);		//Begin call to Mathematica code

		mreal fsrTime = FSR_s;
		mreal minX = minimumX;

		err = findCalibration(libData, formatedScopeCalibrationData, fsrTime, minX, &calibration);

		cout << "Calibration Result:" << endl;
		mint lens[2];
		int err2;
		double value = 0;
		for(unsigned j = 1; j <= 2; j++) {
			lens[0] = j;

			lens[1] = 1;
			err2 = libData->MTensor_getReal(calibration, lens, &value);
			cout << "(" << value <<", ";
			lens[1] = 2;
			err2 = libData->MTensor_getReal(calibration, lens, &value);
			cout << value << ")" << endl;
		}


		if(err == 0) {
			calResults->savePeaks(libData, &calibration);
		}

		Uninitialize_findCalibration(libData);		//End call to Mathematica code
	}
	

	libData->MTensor_free(formatedScopeCalibrationData);
	libData->MTensor_free(calibration);
	result = 0;

	return (err == 0);
}

bool MathematicaPeakFinder::findCarrierAndSidebandPeaks(const STI::Types::TDataMixedSeq& rawSidebandData, 
														const CalibrationResults_ptr& calibration, double sidebandSpacing, double minimumX,
														MixedData& peaks)
{
	WolframLibraryData libData = 0;
	WolframRTL_initialize(WolframLibraryVersion);
	libData = WolframLibraryData_new(WolframLibraryVersion);

	//Setup arguments

	MTensor formatedSidebandData;		//List of {x,y} pairs, with gaps when y is below threshold

	if(!convertRawScopeData(libData, rawSidebandData, formatedSidebandData)) {
		return false;
	}

	//Initialize calibration tensor
	MTensor calTensor;
	int err;
	mint type = MType_Real;
	mint rank = 2;
	mint dims[2];
	dims[0] = 2;
	dims[1] = 2;
	err = libData->MTensor_new(type, rank, dims, &calTensor);

	if(err != 0)
		return false;

	if(!calibration->getPeaks(libData, &calTensor))
		return false;

	//Initialize results tensor
	MTensor peakResults;
	type = MType_Real;
	rank = 2;
	dims[2];
	dims[0] = 3;
	dims[1] = 2;
	err = libData->MTensor_new(type, rank, dims, &peakResults);

	mreal sidebandSpacingArg = sidebandSpacing;
	mreal minX = minimumX;

	if(err == 0) {
		Initialize_findCarrierAndSidebands(libData);		//Begin call to Mathematica code

		err = findCarrierAndSidebands(libData, formatedSidebandData, calTensor, sidebandSpacingArg, minX, &peakResults);

		Uninitialize_findCarrierAndSidebands(libData);		//End call to Mathematica code
	}
	
	if( err == 0) {
		//Copy results of peak search

		double value = 0;
		mint pos[2];
		peaks.clear();
		MixedData peak;
		for(int i = 1; i <= 3; i++) {
			pos[0] = i;
			peak.clear();
			for(int j = 1; j <= 2; j++) {
				pos[1] = j;
				err = libData->MTensor_getReal(peakResults, pos, &value);
				peak.addValue(value);
			}
			peaks.addValue(peak);
		}
	}

	cout << "Peak find results:" << endl;
	cout << peaks.print() << endl;
	

	libData->MTensor_free(formatedSidebandData);
	libData->MTensor_free(calTensor);
	libData->MTensor_free(peakResults);

	return (err == 0);

}

//bool copyMTensor(const MTensor& tensor, mint rank, mint* dims, MixedData& copy)
//{
//	double* value = 0;
//	mint pos[rank];
//	copy.clear();
//
//	MixedData element;
//
//	//initialize
//	int totalLength = 1;
//	for(int i = 1; i <= rank; i++) {
//		pos[i] = 1;
//		totalLength *= dims[i];
//	}
//
//	//iterate over all elements
//	for(int i = 1; i <= totalLength; i++) {
//		pos[1]++;
//		for(int j = 1; j <= rank; j++) {
//			if(pos[j] > dims[j]) {
//				pos[j] = 1;
//				pos[j+1]++;
//			} else {
//				break;
//			}
//		}
//		err = libData->MTensor_getReal(peakResults, pos, value);
//		element.addValue(*value);
//	}
//}

bool MathematicaPeakFinder::calculateFeedbackSignals(const MixedData& peaks, MixedData& feedback)
{
	WolframLibraryData libData = 0;
	WolframRTL_initialize(WolframLibraryVersion);
	libData = WolframLibraryData_new(WolframLibraryVersion);

	//Setup arguments

	//Initialize peak tensor
	MTensor peakTensor;
	int err;
	mint type = MType_Real;
	mint rank = 2;
	mint dims[2];
	dims[0] = 3;
	dims[1] = 2;
	err = libData->MTensor_new(type, rank, dims, &peakTensor);

	if(err != 0)
		return false;

	mint peakPos[2];

	for(int i = 1; i <= dims[0]; i++) {
		peakPos[0] = i;
		for(int j = 1; j <= dims[1]; j++) {
			peakPos[1] = j;
			err = libData->MTensor_setReal(peakTensor, peakPos, 
				peaks.getVector().at(i-1).getVector().at(j-1).getDouble());
		}
	}

	if(err != 0)
		return false;

	//Initialize result tensor
	MTensor feedbackResults;
	type = MType_Real;
	rank = 1;
	mint dimsRes[1];
	dimsRes[0] = 2;
	err = libData->MTensor_new(type, rank, dimsRes, &feedbackResults);

	if(err != 0)
		return false;

	Initialize_getFeedbackSignals(libData);			//Begin call to Mathematica code

	err = getFeedbackSignals(libData, peakTensor, &feedbackResults);

	Uninitialize_getFeedbackSignals(libData);		//End call to Mathematica code

		
	if( err == 0) {
		//Copy results of the feedback function

		double value = 0;
		mint pos[1];
		feedback.clear();
		
		for(int j = 1; j <= 2; j++) {
			pos[0] = j;
			err = libData->MTensor_getReal(feedbackResults, pos, &value);
			feedback.addValue(value);
		}
	}
	
	cout << "Feedback results:" << endl;
	cout << feedback.print() << endl;

	libData->MTensor_free(peakTensor);
	libData->MTensor_free(feedbackResults);

	return (err == 0);
}

bool MathematicaPeakFinder::convertRawScopeData(WolframLibraryData& libData, const STI::Types::TDataMixedSeq& rawScopeData, MTensor& formatedScopeData)
{
	int vecPos = -1;
	double timebase = 1;
	std::string timeBaseStr = "TimeBase";

	for(unsigned i = 0; i < rawScopeData.length(); i++) {
		if(rawScopeData[i]._d() == STI::Types::DataVector && rawScopeData[i].vector().length() == 2
			&& rawScopeData[i].vector()[0]._d() == STI::Types::DataString
			&& timeBaseStr.compare(rawScopeData[i].vector()[0].stringVal()) == 0
			&& rawScopeData[i].vector()[1]._d() == STI::Types::DataDouble) {
			
			timebase = rawScopeData[i].vector()[1].doubleVal();
		}
		else if(rawScopeData[i]._d() == STI::Types::DataVector && rawScopeData[i].vector().length() > 0
			&& rawScopeData[i].vector()[0]._d() == STI::Types::DataVector) {
			vecPos = i;
		}
	}

	if(vecPos == -1) {	//data not found
		return false;
	}

	//Count all the data points.
	int points = 0;
	const STI::Types::TDataMixedSeq& rawData = rawScopeData[vecPos].vector();

	for(unsigned i = 0; i < rawData.length(); i++) {
		if(rawData[i]._d() == STI::Types::DataVector 
			&& rawData[i].vector().length() == 2 
			&& rawData[i].vector()[0]._d() == STI::Types::DataDouble
			&& rawData[i].vector()[1]._d() == STI::Types::DataVector) {
			
			points += rawData[i].vector()[1].vector().length();
		}
		else {
			return false;	//error in format of data vector
		}
	}

	//Allocate space for the MTensor containing the scope data
	mint type = MType_Real;
	mint dims[2];
	mint rank = 2;
	int err;
	dims[0] = points;
	dims[1] = 2;
	err = libData->MTensor_new(type, rank, dims, &formatedScopeData);

	if(err != 0) {
		return false;
	}

	int point = 1;
	mint pos[2];
	double t0 = 0;
	const STI::Types::TDataMixedSeq* rawDataSection;
	for(unsigned i = 0; i < rawData.length(); i++) {
		t0 = rawData[i].vector()[0].doubleVal();
		rawDataSection = &(rawData[i].vector()[1].vector());
		for(unsigned j = 0; j < rawDataSection->length(); j++) {
			if((*rawDataSection)[j]._d() != STI::Types::DataDouble) {
				return false;	//data format error
			}
			pos[0] = point;
			
			//Set X
			pos[1] = 1;
			err = libData->MTensor_setReal(formatedScopeData, pos, t0 + j*timebase);
			
			//Set Y
			pos[1] = 2;
			err = libData->MTensor_setReal(formatedScopeData, pos, (*rawDataSection)[j].doubleVal());

			point++;
		}
	}

	return true;
}


