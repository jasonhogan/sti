
#ifndef MAHTEMATICAPEAKFINDERHF_H
#define MAHTEMATICAPEAKFINDERHF_H

#include "CalibrationResults.h"

#include "orbTypes.h"
#include "WolframRTL.h"
#include "MixedData.h"

class MathematicaPeakFinderHF
{
public:
	MathematicaPeakFinderHF(double minNumPoints);

	bool findCalibrationPeaks(const STI::Types::TDataMixedSeq& rawCalData, double FSR_s, double minimumX, const CalibrationResults_ptr& calibration);
	
//	bool calculateFeedbackSignals(const MixedData& peaks, MixedData& feedback);

	bool findFirstSidebandPeaks(const STI::Types::TDataMixedSeq& rawSidebandData, 
							const CalibrationResults_ptr& calibration, 
							double sidebandSpacing, 
							double minimumX, 
							double targetRange, 
							double carrierOffset,
							MixedData& peaks);

		
	bool findCarrierPeaks(const STI::Types::TDataMixedSeq& rawSidebandData, 
							const CalibrationResults_ptr& calibration, 
							double minimumX, 
							double targetRange, 
							double carrierOffset,
							MixedData& peaks);

	bool calculateFeedbackSignalsHighLow(const MixedData& sidebandPeaks, const MixedData& carrierPeaks, 
		MixedData& feedback);


private:
	
	bool convertRawScopeData(WolframLibraryData& libData, const STI::Types::TDataMixedSeq& rawScopeData, MTensor& formatedScopeData);
	double minNumPointsPerSection;

};

#endif
