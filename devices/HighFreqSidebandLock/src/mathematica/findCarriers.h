#include "WolframLibrary.h"

EXTERN_C DLLEXPORT int Initialize_findCarriers(WolframLibraryData libData);

EXTERN_C DLLEXPORT void Uninitialize_findCarriers(WolframLibraryData libData);

EXTERN_C DLLEXPORT int findCarriers(WolframLibraryData libData, MTensor A1, MTensor A2, mreal A3, mreal A4, mreal A5, MTensor *Res);

