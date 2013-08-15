#include "WolframLibrary.h"

EXTERN_C DLLEXPORT int Initialize_getFeedbackSignals(WolframLibraryData libData);

EXTERN_C DLLEXPORT void Uninitialize_getFeedbackSignals(WolframLibraryData libData);

EXTERN_C DLLEXPORT int getFeedbackSignals(WolframLibraryData libData, MTensor A1, MTensor *Res);

