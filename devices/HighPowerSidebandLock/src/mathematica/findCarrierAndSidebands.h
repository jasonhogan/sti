#include "WolframLibrary.h"

EXTERN_C DLLEXPORT int Initialize_findCarrierAndSidebands(WolframLibraryData libData);

EXTERN_C DLLEXPORT void Uninitialize_findCarrierAndSidebands(WolframLibraryData libData);

EXTERN_C DLLEXPORT int findCarrierAndSidebands(WolframLibraryData libData, MTensor A1, MTensor A2, mreal A3, mreal A4, MTensor *Res);

