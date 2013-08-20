#include "WolframLibrary.h"

EXTERN_C DLLEXPORT int Initialize_findCalibration(WolframLibraryData libData);

EXTERN_C DLLEXPORT void Uninitialize_findCalibration(WolframLibraryData libData);

EXTERN_C DLLEXPORT int findCalibration(WolframLibraryData libData, MTensor A1, mreal A2, mreal A3, MTensor *Res);

