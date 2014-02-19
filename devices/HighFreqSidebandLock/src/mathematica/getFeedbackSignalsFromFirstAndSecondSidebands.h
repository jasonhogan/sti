#include "WolframLibrary.h"

EXTERN_C DLLEXPORT int Initialize_getFeedbackSignalsFromFirstAndSecondSidebands(WolframLibraryData libData);

EXTERN_C DLLEXPORT void Uninitialize_getFeedbackSignalsFromFirstAndSecondSidebands(WolframLibraryData libData);

EXTERN_C DLLEXPORT int getFeedbackSignalsFromFirstAndSecondSidebands(WolframLibraryData libData, MTensor A1, MTensor *Res);

