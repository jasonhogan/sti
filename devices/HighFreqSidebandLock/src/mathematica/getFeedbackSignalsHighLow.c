#include "math.h"

#include "WolframRTL.h"

static WolframCompileLibrary_Functions funStructCompile;

static LibraryFunctionPointer FP0;

static LibraryFunctionPointer FP1;

static MArgument FPA[3];


static mint I0_0;

static mint I0_1;

static mint I0_2;

static mbool initialize = 1;

#include "getFeedbackSignalsHighLow.h"

DLLEXPORT int Initialize_getFeedbackSignalsHighLow(WolframLibraryData libData)
{
if( initialize)
{
funStructCompile = libData->compileLibraryFunctions;
I0_0 = (mint) 1;
I0_1 = (mint) 2;
I0_2 = (mint) 4;
FP0 = funStructCompile->getFunctionCallPointer("CopyTensor");
if( FP0 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP1 = funStructCompile->getFunctionCallPointer("TotalAll");
if( FP1 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
initialize = 0;
}
return 0;
}

DLLEXPORT void Uninitialize_getFeedbackSignalsHighLow(WolframLibraryData libData)
{
if( !initialize)
{
initialize = 1;
}
}

DLLEXPORT int getFeedbackSignalsHighLow(WolframLibraryData libData, MTensor A1, MTensor A2, MTensor *Res)
{
mint I0_3;
mreal R0_0;
mreal R0_1;
mreal R0_2;
mreal R0_3;
MTensor* T0_0;
MTensor* T0_1;
MTensor* T0_2;
MTensor* T0_3;
MTensor* T0_4;
MTensor* T0_5;
MTensorInitializationData Tinit;
mreal *P2;
mint *D2;
mint *D4;
mreal *P5;
MArgument FPA[3];
int err = 0;
Tinit = funStructCompile->GetInitializedMTensors(libData, 4);
T0_2 = MTensorInitializationData_getTensor(Tinit, 0);
T0_3 = MTensorInitializationData_getTensor(Tinit, 1);
T0_4 = MTensorInitializationData_getTensor(Tinit, 2);
T0_5 = MTensorInitializationData_getTensor(Tinit, 3);
T0_0 = &A1;
T0_1 = &A2;
err = funStructCompile->MTensor_getMTensorInitialized(T0_2, *T0_1, &I0_0, 1);
if( err)
{
goto error_label;
}
P2 = MTensor_getRealDataMacro(*T0_2);
D2 = MTensor_getDimensionsMacro(*T0_2);
MArgument_getMTensorAddress(FPA[0]) = T0_0;
MArgument_getMTensorAddress(FPA[1]) = T0_3;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
{
mint S0[2];
S0[0] = I0_0;
S0[1] = I0_1;
err = libData->MTensor_getReal(*T0_3, S0, &R0_0);
if( err)
{
goto error_label;
}
}
{
mint S0[2];
S0[0] = I0_1;
S0[1] = I0_1;
err = libData->MTensor_getReal(*T0_3, S0, &R0_1);
if( err)
{
goto error_label;
}
}
R0_2 = -R0_1;
R0_0 = R0_0 + R0_2;
{
int S0[2];
void* S1[2];
S0[0] = 2;
S1[0] = 0;
S0[1] = 0;
S1[1] = (void*) (&I0_1);
err = funStructCompile->MTensor_getPart(T0_5, *T0_3, 2, S0, S1);
if( err)
{
goto error_label;
}
}
D4 = MTensor_getDimensionsMacro(*T0_5);
MArgument_getMTensorAddress(FPA[0]) = T0_5;
MArgument_getIntegerAddress(FPA[1]) = &I0_2;
MArgument_getRealAddress(FPA[2]) = &R0_2;
err = FP1(libData, 2, FPA, FPA[2]);/*  TotalAll  */
if( err)
{
goto error_label;
}
I0_3 = D4[0];
R0_1 = (mreal) I0_3;
R0_3 = 1 / R0_1;
R0_2 = R0_2 * R0_3;
R0_3 = 1 / R0_2;
R0_0 = R0_0 * R0_3;
{
int S0[2];
void* S1[2];
S0[0] = 2;
S1[0] = 0;
S0[1] = 0;
S1[1] = (void*) (&I0_1);
err = funStructCompile->MTensor_getPart(T0_5, *T0_3, 2, S0, S1);
if( err)
{
goto error_label;
}
}
D4 = MTensor_getDimensionsMacro(*T0_5);
MArgument_getMTensorAddress(FPA[0]) = T0_5;
MArgument_getIntegerAddress(FPA[1]) = &I0_2;
MArgument_getRealAddress(FPA[2]) = &R0_3;
err = FP1(libData, 2, FPA, FPA[2]);/*  TotalAll  */
if( err)
{
goto error_label;
}
I0_3 = D4[0];
R0_2 = (mreal) I0_3;
R0_1 = 1 / R0_2;
R0_3 = R0_3 * R0_1;
{
mint S0 = D2[0];
if( I0_1 > 0)
{
if( I0_1 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_1 - 1;
}
else
{
if( I0_1 == 0 || I0_1 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_1;
}
R0_1 = P2[S0];
}
R0_2 = 1 / R0_1;
R0_3 = R0_3 * R0_2;
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_4, 3, 1, &S0);
if( err)
{
goto error_label;
}
P5 = MTensor_getRealDataMacro(*T0_4);
P5[0] = R0_0;
P5[1] = R0_3;
}
funStructCompile->MTensor_copy(Res, *T0_4);
error_label:
funStructCompile->ReleaseInitializedMTensors(Tinit);
funStructCompile->WolframLibraryData_cleanUp(libData, 1);
return err;
}

