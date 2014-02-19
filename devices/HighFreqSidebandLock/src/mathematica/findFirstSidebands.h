#include "WolframLibrary.h"

EXTERN_C DLLEXPORT int Initialize_findFirstSidebands(WolframLibraryData libData);

EXTERN_C DLLEXPORT void Uninitialize_findFirstSidebands(WolframLibraryData libData);

EXTERN_C DLLEXPORT int findFirstSidebands(WolframLibraryData libData, MTensor A1, MTensor A2, mreal A3, mreal A4, mreal A5, mreal A6, MTensor *Res);

