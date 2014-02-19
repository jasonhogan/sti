#include "WolframLibrary.h"

EXTERN_C DLLEXPORT int Initialize_findFirstAndSecondOrderSidebands(WolframLibraryData libData);

EXTERN_C DLLEXPORT void Uninitialize_findFirstAndSecondOrderSidebands(WolframLibraryData libData);

EXTERN_C DLLEXPORT int findFirstAndSecondOrderSidebands(WolframLibraryData libData, MTensor A1, MTensor A2, mreal A3, mreal A4, mreal A5, mreal A6, mreal A7, MTensor *Res);

