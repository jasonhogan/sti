
#ifndef MATHEMATICAPEAKFINDER_H
#define MATHEMATICAPEAKFINDER_H

#include "CalibrationResults.h"

#include "orbTypes.h"
#include "WolframRTL.h"
#include "MixedData.h"

class MathematicaPeakFinder
{
public:
	MathematicaPeakFinder(double minNumPoints);

	bool findCalibrationPeaks(const STI::Types::TDataMixedSeq& rawCalData, double FSR_s, double minimumX, const CalibrationResults_ptr& calibration);
	
	bool findCarrierAndSidebandPeaks(const STI::Types::TDataMixedSeq& rawSidebandData, 
		const CalibrationResults_ptr& calibration, double sidebandSpacing, double minimumX, double targetRange, MixedData& peaks);
	
	bool calculateFeedbackSignals(const MixedData& peaks, MixedData& feedback);

	//Using first and second order sidebands:
	bool findFirstAndSecondOrderSidebandPeaks(const STI::Types::TDataMixedSeq& rawSidebandData, 
														const CalibrationResults_ptr& calibration, 
														double firstOrderSidebandSpacing, 
														double secondOrderSidebandSpacing, 
														double minimumX,
														double targetRange,
														MixedData& peaks,
														double carrierOffset);
	
	bool calculateFeedbackSignalsFromFirstAndSecondSideband(const MixedData& peaks, MixedData& feedback);


private:
	
	bool convertRawScopeData(WolframLibraryData& libData, const STI::Types::TDataMixedSeq& rawScopeData, MTensor& formatedScopeData);
	double minNumPointsPerSection;

};

#endif
