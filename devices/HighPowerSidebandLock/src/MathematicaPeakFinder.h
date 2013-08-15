
#ifndef MATHEMATICAPEAKFINDER_H
#define MATHEMATICAPEAKFINDER_H

#include "CalibrationResults.h"

#include "orbTypes.h"
#include "WolframRTL.h"
#include "MixedData.h"

class MathematicaPeakFinder
{
public:
	MathematicaPeakFinder();

	bool findCalibrationPeaks(const STI::Types::TDataMixedSeq& rawCalData, double FSR_s, const CalibrationResults_ptr& calibration);
	
	bool findCarrierAndSidebandPeaks(const STI::Types::TDataMixedSeq& rawSidebandData, 
		const CalibrationResults_ptr& calibration, double sidebandSpacing, MixedData& peaks);
	
	bool calculateFeedbackSignals(const MixedData& peaks, MixedData& feedback);

private:
	
	bool convertRawScopeData(WolframLibraryData& libData, const STI::Types::TDataMixedSeq& rawScopeData, MTensor& formatedScopeData);

};

#endif
