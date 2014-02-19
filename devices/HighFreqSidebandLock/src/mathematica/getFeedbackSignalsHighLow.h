#include "WolframLibrary.h"

EXTERN_C DLLEXPORT int Initialize_getFeedbackSignalsHighLow(WolframLibraryData libData);

EXTERN_C DLLEXPORT void Uninitialize_getFeedbackSignalsHighLow(WolframLibraryData libData);

EXTERN_C DLLEXPORT int getFeedbackSignalsHighLow(WolframLibraryData libData, MTensor A1, MTensor A2, MTensor *Res);

