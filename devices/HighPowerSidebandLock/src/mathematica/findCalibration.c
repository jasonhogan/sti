#include "math.h"

#include "WolframRTL.h"

static WolframCompileLibrary_Functions funStructCompile;

static const mint UnitIncrements[3] = {1, 1, 1};

static LibraryFunctionPointer FP0;

static BinaryMathFunctionPointer FP1;

static BinaryMathFunctionPointer FP2;

static LibraryFunctionPointer FP3;

static LibraryFunctionPointer FP4;

static LibraryFunctionPointer FP5;

static LibraryFunctionPointer FP6;

static LibraryFunctionPointer FP7;

static LibraryFunctionPointer FP8;

static LibraryFunctionPointer FP9;

static LibraryFunctionPointer FP10;

static LibraryFunctionPointer FP11;

static MArgument FPA[4];


static mbool B0_2;

static mint I0_0;

static mint I0_6;

static mint I0_8;

static mint I0_9;

static mint I0_11;

static mint I0_12;

static mreal R0_5;

static mreal R0_6;

static mreal R0_7;

static mreal R0_8;

static mreal R0_9;

static MTensor T0_8B = 0;

static MTensor* T0_8 = &T0_8B;

static MTensor T0_11B = 0;

static MTensor* T0_11 = &T0_11B;

static MTensor T0_12B = 0;

static MTensor* T0_12 = &T0_12B;

static MTensor T0_13B = 0;

static MTensor* T0_13 = &T0_13B;

static MTensor T0_14B = 0;

static MTensor* T0_14 = &T0_14B;

static mint *P1;

static mint *P2;

static mint *P3;

static mint *P4;

static mint *P5;

static mbool initialize = 1;

#include "findCalibration.h"

DLLEXPORT int Initialize_findCalibration(WolframLibraryData libData)
{
int err = 0;
if( initialize)
{
funStructCompile = libData->compileLibraryFunctions;
I0_0 = (mint) 1;
R0_5 = (mreal) 1.;
R0_6 = (mreal) 7.;
I0_9 = (mint) 2;
{
mint S0[1];
S0[0] = 3;
err = funStructCompile->MTensor_allocate(T0_14, 2, 1, S0);
if( err)
{
goto error_label;
}
P1 = MTensor_getIntegerDataMacro(*T0_14);
P1[0] = 1;
P1[1] = 2;
P1[2] = 3;
}
I0_8 = (mint) -1;
R0_9 = (mreal) 0.5;
{
mint S0[1];
S0[0] = 2;
err = funStructCompile->MTensor_allocate(T0_13, 2, 1, S0);
if( err)
{
goto error_label;
}
P2 = MTensor_getIntegerDataMacro(*T0_13);
P2[0] = 3;
P2[1] = 2;
}
{
mint S0[1];
S0[0] = 2;
err = funStructCompile->MTensor_allocate(T0_12, 2, 1, S0);
if( err)
{
goto error_label;
}
P3 = MTensor_getIntegerDataMacro(*T0_12);
P3[0] = 1;
P3[1] = 3;
}
B0_2 = (mbool) 0;
R0_7 = (mreal) 0.1;
R0_8 = (mreal) 0.;
I0_6 = (mint) 0;
I0_11 = (mint) 100;
I0_12 = (mint) 3;
{
mint S0[2];
S0[0] = 1;
S0[1] = 1;
err = funStructCompile->MTensor_allocate(T0_8, 2, 2, S0);
if( err)
{
goto error_label;
}
P4 = MTensor_getIntegerDataMacro(*T0_8);
P4[0] = 0;
}
{
mint S0[1];
S0[0] = 2;
err = funStructCompile->MTensor_allocate(T0_11, 2, 1, S0);
if( err)
{
goto error_label;
}
P5 = MTensor_getIntegerDataMacro(*T0_11);
P5[0] = 1;
P5[1] = 2;
}
FP0 = funStructCompile->getFunctionCallPointer("CopyTensor");
if( FP0 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP1 = funStructCompile->getBinaryMathFunction(261, 2, 2);/*  Mod  */
if( FP1 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP2 = funStructCompile->getBinaryMathFunction(257, 2, 2);/*  Plus  */
if( FP2 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP3 = funStructCompile->getFunctionCallPointer("Union");
if( FP3 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP4 = funStructCompile->getFunctionCallPointer("Position");
if( FP4 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP5 = funStructCompile->getFunctionCallPointer("Sort");
if( FP5 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP6 = funStructCompile->getFunctionCallPointer("Join");
if( FP6 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP7 = funStructCompile->getFunctionCallPointer("Take");
if( FP7 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP8 = funStructCompile->getFunctionCallPointer("MaxRT");
if( FP8 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP9 = funStructCompile->getFunctionCallPointer("MinRT");
if( FP9 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP10 = funStructCompile->getFunctionCallPointer("Insert");
if( FP10 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
FP11 = funStructCompile->getFunctionCallPointer("Complement");
if( FP11 == 0)
{
return LIBRARY_FUNCTION_ERROR;
}
initialize = 0;
}
error_label:
return err;
}

DLLEXPORT void Uninitialize_findCalibration(WolframLibraryData libData)
{
if( !initialize)
{
initialize = 1;
}
}

DLLEXPORT int findCalibration(WolframLibraryData libData, MTensor A1, mreal A2, MTensor *Res)
{
mbool B0_0;
mbool B0_1;
mbool B0_3;
mbool B0_4;
mint I0_1;
mint I0_2;
mint I0_3;
mint I0_4;
mint I0_5;
mint I0_7;
mint I0_10;
mreal R0_0;
mreal R0_1;
mreal R0_2;
mreal R0_3;
mreal R0_4;
mreal R0_10;
mreal R0_11;
mreal R0_12;
mreal R0_13;
MTensor* T0_0;
MTensor* T0_1;
MTensor* T0_2;
MTensor* T0_3;
MTensor* T0_4;
MTensor* T0_5;
MTensor* T0_6;
MTensor* T0_7;
MTensor* T0_9;
MTensor* T0_10;
MTensor* T0_15;
MTensorInitializationData Tinit;
mint dims[3];
mint *D6;
mreal *P7;
mint *D7;
mreal *P9;
mint *D9;
mint *D11;
mreal *P12;
mint *D12;
mreal *P14;
mint *D14;
mint *P15;
mint *D17;
mint *P18;
mint *D18;
mint *P19;
mint *D19;
mint *P21;
mreal *P25;
mint *D25;
mint *D29;
mint *D31;
mint *P32;
mint *D32;
mint *D34;
mint *P35;
mint *D35;
mint *P37;
mint *D37;
MArgument FPA[4];
int err = 0;
Tinit = funStructCompile->GetInitializedMTensors(libData, 10);
T0_1 = MTensorInitializationData_getTensor(Tinit, 0);
T0_10 = MTensorInitializationData_getTensor(Tinit, 1);
T0_15 = MTensorInitializationData_getTensor(Tinit, 2);
T0_2 = MTensorInitializationData_getTensor(Tinit, 3);
T0_3 = MTensorInitializationData_getTensor(Tinit, 4);
T0_4 = MTensorInitializationData_getTensor(Tinit, 5);
T0_5 = MTensorInitializationData_getTensor(Tinit, 6);
T0_6 = MTensorInitializationData_getTensor(Tinit, 7);
T0_7 = MTensorInitializationData_getTensor(Tinit, 8);
T0_9 = MTensorInitializationData_getTensor(Tinit, 9);
T0_0 = &A1;
R0_0 = A2;
MArgument_getMTensorAddress(FPA[0]) = T0_0;
MArgument_getMTensorAddress(FPA[1]) = T0_1;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
D6 = MTensor_getDimensionsMacro(*T0_1);
I0_2 = D6[0];
I0_5 = I0_6;
dims[0] = I0_2;
err = funStructCompile->MTensor_allocate(T0_2, 3, 1, dims);
if( err)
{
goto error_label;
}
P7 = MTensor_getRealDataMacro(*T0_2);
D7 = MTensor_getDimensionsMacro(*T0_2);
I0_4 = I0_6;
goto lab15;
lab7:
{
mint S0[2];
S0[0] = I0_4;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_1, S0, &R0_3);
if( err)
{
goto error_label;
}
}
I0_3 = D6[0];
{
mint S0 = FP1((void*) (&I0_7), (void*) (&I0_4), (void*) (&I0_3), 1, UnitIncrements, 4);/*  Mod  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
{
mint S0 = FP2((void*) (&I0_7), (void*) (&I0_7), (void*) (&I0_0), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
{
mint S0[2];
S0[0] = I0_7;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_1, S0, &R0_2);
if( err)
{
goto error_label;
}
}
R0_4 = -R0_2;
R0_3 = R0_3 + R0_4;
P7[I0_5++] = R0_3;
lab15:
if( ++I0_4 <= I0_2)
{
goto lab7;
}
MArgument_getMTensorAddress(FPA[0]) = T0_2;
MArgument_getMTensorAddress(FPA[1]) = T0_3;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = T0_4;
err = FP3(libData, 1, FPA, FPA[1]);/*  Union  */
if( err)
{
goto error_label;
}
P9 = MTensor_getRealDataMacro(*T0_4);
D9 = MTensor_getDimensionsMacro(*T0_4);
I0_2 = D9[0];
I0_3 = I0_8;
dims[0] = I0_2;
dims[1] = I0_3;
err = funStructCompile->MTensor_allocate(T0_6, 3, 2, dims);
if( err)
{
goto error_label;
}
I0_4 = I0_6;
goto lab30;
lab23:
R0_1 = P9[I0_4 - 1];
{
MTensor S0 = funStructCompile->getRankZeroMTensor((void*) (&R0_1), 3, 0);
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = &S0;
MArgument_getRealAddress(FPA[2]) = &R0_5;
MArgument_getMTensorAddress(FPA[3]) = T0_7;
err = FP4(libData, 3, FPA, FPA[3]);/*  Position  */
if( err)
{
goto error_label;
}
}
D11 = MTensor_getDimensionsMacro(*T0_7);
I0_7 = D11[0];
R0_1 = P9[I0_4 - 1];
R0_4 = (mreal) I0_7;
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_7, 3, 1, &S0);
if( err)
{
goto error_label;
}
P12 = MTensor_getRealDataMacro(*T0_7);
D12 = MTensor_getDimensionsMacro(*T0_7);
P12[0] = R0_4;
P12[1] = R0_1;
}
err = funStructCompile->MTensor_insertMTensor(*T0_6, *T0_7, &I0_3);
if( err)
{
goto error_label;
}
lab30:
if( ++I0_4 <= I0_2)
{
goto lab23;
}
MArgument_getMTensorAddress(FPA[0]) = T0_6;
MArgument_getMTensorAddress(FPA[1]) = T0_4;
err = FP5(libData, 1, FPA, FPA[1]);/*  Sort  */
if( err)
{
goto error_label;
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_6, *T0_4, &I0_8, 1);
if( err)
{
goto error_label;
}
P14 = MTensor_getRealDataMacro(*T0_6);
D14 = MTensor_getDimensionsMacro(*T0_6);
{
mint S0 = D14[0];
if( I0_9 > 0)
{
if( I0_9 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_9 - 1;
}
else
{
if( I0_9 == 0 || I0_9 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_9;
}
R0_1 = P14[S0];
}
{
mint S0 = 1;
err = funStructCompile->MTensor_allocate(T0_6, 3, 1, &S0);
if( err)
{
goto error_label;
}
P14 = MTensor_getRealDataMacro(*T0_6);
D14 = MTensor_getDimensionsMacro(*T0_6);
P14[0] = R0_1;
}
{
mint S0 = D14[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
R0_1 = P14[S0];
}
R0_4 = R0_1 < 0 ? -R0_1 : R0_1;
I0_3 = D7[0];
I0_4 = I0_6;
dims[0] = I0_3;
err = funStructCompile->MTensor_allocate(T0_3, 2, 1, dims);
if( err)
{
goto error_label;
}
P15 = MTensor_getIntegerDataMacro(*T0_3);
I0_2 = I0_6;
goto lab52;
lab42:
R0_2 = P7[I0_2 - 1];
R0_1 = R0_2 < 0 ? -R0_2 : R0_2;
R0_2 = (mreal) I0_9;
R0_2 = R0_2 * R0_4;
{
mreal S0[2];
S0[0] = R0_1;
S0[1] = R0_2;
B0_0 = funStructCompile->Compare_R(7, R0_6, 2, S0);
}
if( !B0_0)
{
goto lab50;
}
I0_10 = I0_0;
goto lab51;
lab50:
I0_10 = I0_6;
lab51:
P15[I0_4++] = I0_10;
lab52:
if( ++I0_2 <= I0_3)
{
goto lab42;
}
{
MTensor S0 = funStructCompile->getRankZeroMTensor((void*) (&I0_0), 2, 0);
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = &S0;
MArgument_getMTensorAddress(FPA[2]) = T0_4;
err = FP4(libData, 2, FPA, FPA[2]);/*  Position  */
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_8;
MArgument_getMTensorAddress(FPA[1]) = T0_4;
MArgument_getMTensorAddress(FPA[2]) = T0_3;
err = FP6(libData, 2, FPA, FPA[2]);/*  Join  */
if( err)
{
goto error_label;
}
D17 = MTensor_getDimensionsMacro(*T0_3);
I0_5 = D17[0];
I0_10 = I0_6;
dims[0] = I0_5;
err = funStructCompile->MTensor_allocate(T0_4, 2, 1, dims);
if( err)
{
goto error_label;
}
P18 = MTensor_getIntegerDataMacro(*T0_4);
D18 = MTensor_getDimensionsMacro(*T0_4);
I0_7 = I0_6;
goto lab63;
lab60:
funStructCompile->MTensor_getMTensorInitialized(T0_5, *T0_3, &I0_7, 1);
P19 = MTensor_getIntegerDataMacro(*T0_5);
D19 = MTensor_getDimensionsMacro(*T0_5);
{
mint S0 = D19[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
I0_3 = P19[S0];
}
P18[I0_10++] = I0_3;
lab63:
if( ++I0_7 <= I0_5)
{
goto lab60;
}
I0_2 = D18[0];
{
mint S0 = FP2((void*) (&I0_2), (void*) (&I0_2), (void*) (&I0_8), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
I0_1 = I0_8;
dims[0] = I0_2;
dims[1] = I0_0;
dims[2] = I0_1;
err = funStructCompile->MTensor_allocate(T0_5, 3, 3, dims);
if( err)
{
goto error_label;
}
I0_10 = I0_6;
goto lab87;
lab70:
{
mint S0 = D18[0];
if( I0_10 > 0)
{
if( I0_10 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_10 - 1;
}
else
{
if( I0_10 == 0 || I0_10 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_10;
}
I0_3 = P18[S0];
}
{
mint S0 = FP2((void*) (&I0_4), (void*) (&I0_0), (void*) (&I0_3), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
{
mint S0 = FP2((void*) (&I0_3), (void*) (&I0_10), (void*) (&I0_0), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
{
mint S0 = D18[0];
if( I0_3 > 0)
{
if( I0_3 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_3 - 1;
}
else
{
if( I0_3 == 0 || I0_3 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_3;
}
I0_5 = P18[S0];
}
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_7, 2, 1, &S0);
if( err)
{
goto error_label;
}
P21 = MTensor_getIntegerDataMacro(*T0_7);
P21[0] = I0_4;
P21[1] = I0_5;
}
MArgument_getMTensorAddress(FPA[0]) = T0_1;
MArgument_getIntegerAddress(FPA[1]) = &I0_0;
MArgument_getMTensorAddress(FPA[2]) = T0_7;
MArgument_getMTensorAddress(FPA[3]) = T0_3;
err = FP7(libData, 3, FPA, FPA[3]);/*  Take  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = T0_7;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_7;
MArgument_getRealAddress(FPA[1]) = &R0_1;
err = FP8(libData, 1, FPA, FPA[1]);/*  MaxRT  */
if( err)
{
goto error_label;
}
{
MTensor S0 = funStructCompile->getRankZeroMTensor((void*) (&R0_1), 3, 0);
MArgument_getMTensorAddress(FPA[0]) = T0_7;
MArgument_getMTensorAddress(FPA[1]) = &S0;
MArgument_getRealAddress(FPA[2]) = &R0_5;
MArgument_getMTensorAddress(FPA[3]) = T0_9;
err = FP4(libData, 3, FPA, FPA[3]);/*  Position  */
if( err)
{
goto error_label;
}
}
{
mint S0[2];
S0[0] = I0_0;
S0[1] = I0_0;
err = libData->MTensor_getInteger(*T0_9, S0, &I0_4);
if( err)
{
goto error_label;
}
}
{
int S0[2];
void* S1[2];
S0[0] = 2;
S1[0] = 0;
S0[1] = 0;
S1[1] = (void*) (&I0_0);
err = funStructCompile->MTensor_getPart(T0_7, *T0_3, 2, S0, S1);
if( err)
{
goto error_label;
}
}
P12 = MTensor_getRealDataMacro(*T0_7);
D12 = MTensor_getDimensionsMacro(*T0_7);
MArgument_getMTensorAddress(FPA[0]) = T0_7;
MArgument_getRealAddress(FPA[1]) = &R0_1;
err = FP9(libData, 1, FPA, FPA[1]);/*  MinRT  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_7;
MArgument_getRealAddress(FPA[1]) = &R0_2;
err = FP8(libData, 1, FPA, FPA[1]);/*  MaxRT  */
if( err)
{
goto error_label;
}
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_9, 3, 1, &S0);
if( err)
{
goto error_label;
}
P25 = MTensor_getRealDataMacro(*T0_9);
D25 = MTensor_getDimensionsMacro(*T0_9);
P25[0] = R0_1;
P25[1] = R0_2;
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_7, *T0_3, &I0_4, 1);
if( err)
{
goto error_label;
}
P12 = MTensor_getRealDataMacro(*T0_7);
D12 = MTensor_getDimensionsMacro(*T0_7);
{
mint S0;
mint S1[2];
S1[0] = 2;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D12[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_10, 3, 2, S1);
if( err)
{
goto error_label;
}
S0 = 1;
err = libData->MTensor_setMTensor(*T0_10, *T0_7, &S0, 1);
if( err)
{
goto error_label;
}
S0 = 2;
err = libData->MTensor_setMTensor(*T0_10, *T0_9, &S0, 1);
if( err)
{
goto error_label;
}
}
err = funStructCompile->MTensor_insertMTensor(*T0_5, *T0_10, &I0_1);
if( err)
{
goto error_label;
}
lab87:
if( ++I0_10 <= I0_2)
{
goto lab70;
}
MArgument_getMTensorAddress(FPA[0]) = T0_5;
MArgument_getMTensorAddress(FPA[1]) = T0_1;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
R0_4 = R0_0;
MArgument_getMTensorAddress(FPA[0]) = T0_1;
MArgument_getMTensorAddress(FPA[1]) = T0_3;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = T0_4;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
D29 = MTensor_getDimensionsMacro(*T0_4);
I0_2 = I0_0;
I0_10 = I0_0;
lab94:
I0_4 = D29[0];
B0_0 = I0_10 <= I0_4;
if( !B0_0)
{
goto lab107;
}
{
mint S0[3];
S0[0] = I0_10;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_4, S0, &R0_11);
if( err)
{
goto error_label;
}
}
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_4, S0, &R0_10);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_11;
S0[1] = R0_10;
B0_1 = funStructCompile->Compare_R(7, R0_6, 2, S0);
}
if( !B0_1)
{
goto lab103;
}
I0_2 = I0_10;
goto lab103;
lab103:
I0_4 = I0_10;
{
mint S0 = FP2((void*) (&I0_5), (void*) (&I0_4), (void*) (&I0_0), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
I0_10 = I0_5;
goto lab94;
lab107:
err = funStructCompile->MTensor_getMTensorInitialized(T0_9, *T0_4, &I0_2, 1);
if( err)
{
goto error_label;
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_4, *T0_9, &I0_0, 1);
if( err)
{
goto error_label;
}
P9 = MTensor_getRealDataMacro(*T0_4);
D9 = MTensor_getDimensionsMacro(*T0_4);
{
mint S0 = D9[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
R0_11 = P9[S0];
}
{
mint S0;
mint S1[2];
S1[0] = 1;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D9[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_3, 3, 2, S1);
if( err)
{
goto error_label;
}
S0 = 1;
err = libData->MTensor_setMTensor(*T0_3, *T0_4, &S0, 1);
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_1;
MArgument_getMTensorAddress(FPA[1]) = T0_9;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
D31 = MTensor_getDimensionsMacro(*T0_9);
R0_10 = -R0_4;
R0_3 = R0_11 + R0_10;
R0_10 = R0_7 * R0_4;
I0_2 = I0_8;
R0_1 = R0_8;
R0_2 = R0_9 * R0_10;
lab118:
B0_3 = I0_2 < I0_6;
if( !B0_3)
{
goto lab125;
}
R0_13 = (mreal) I0_11;
R0_13 = R0_13 * R0_10;
{
mreal S0[2];
S0[0] = R0_2;
S0[1] = R0_13;
B0_0 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
B0_1 = B0_0;
goto lab126;
lab125:
B0_1 = B0_2;
lab126:
if( !B0_1)
{
goto lab153;
}
I0_10 = I0_0;
lab128:
I0_4 = D31[0];
B0_3 = I0_10 <= I0_4;
if( !B0_3)
{
goto lab149;
}
{
mint S0[3];
S0[0] = I0_10;
S0[1] = I0_0;
S0[2] = I0_0;
err = libData->MTensor_getReal(*T0_9, S0, &R0_13);
if( err)
{
goto error_label;
}
}
R0_12 = -R0_3;
R0_13 = R0_13 + R0_12;
R0_12 = R0_13 < 0 ? -R0_13 : R0_13;
{
mreal S0[2];
S0[0] = R0_12;
S0[1] = R0_2;
B0_0 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_0)
{
goto lab145;
}
{
mint S0[3];
S0[0] = I0_10;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_9, S0, &R0_12);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_12;
S0[1] = R0_1;
B0_4 = funStructCompile->Compare_R(7, R0_6, 2, S0);
}
if( !B0_4)
{
goto lab144;
}
I0_2 = I0_10;
{
mint S0[3];
S0[0] = I0_10;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_9, S0, &R0_12);
if( err)
{
goto error_label;
}
}
R0_1 = R0_12;
goto lab144;
lab144:
goto lab145;
lab145:
I0_4 = I0_10;
{
mint S0 = FP2((void*) (&I0_5), (void*) (&I0_4), (void*) (&I0_0), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
I0_10 = I0_5;
goto lab128;
lab149:
R0_12 = R0_9 * R0_10;
R0_13 = R0_2 + R0_12;
R0_2 = R0_13;
goto lab118;
lab153:
{
mint S0[2];
S0[0] = I0_2;
S0[1] = I0_0;
err = funStructCompile->MTensor_getMTensorInitialized(T0_7, *T0_9, S0, 2);
if( err)
{
goto error_label;
}
}
P12 = MTensor_getRealDataMacro(*T0_7);
D12 = MTensor_getDimensionsMacro(*T0_7);
{
mint S0 = 1;
err = funStructCompile->MTensor_allocate(T0_9, 2, 1, &S0);
if( err)
{
goto error_label;
}
P32 = MTensor_getIntegerDataMacro(*T0_9);
D32 = MTensor_getDimensionsMacro(*T0_9);
P32[0] = I0_8;
}
{
mint S0;
mint S1[2];
S1[0] = 1;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D32[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_2, 2, 2, S1);
if( err)
{
goto error_label;
}
S0 = 1;
err = libData->MTensor_setMTensor(*T0_2, *T0_9, &S0, 1);
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = T0_7;
MArgument_getMTensorAddress(FPA[2]) = T0_2;
MArgument_getMTensorAddress(FPA[3]) = T0_10;
err = FP10(libData, 3, FPA, FPA[3]);/*  Insert  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_10;
MArgument_getMTensorAddress(FPA[1]) = T0_3;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_1;
MArgument_getMTensorAddress(FPA[1]) = T0_10;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
D34 = MTensor_getDimensionsMacro(*T0_10);
R0_10 = R0_11 + R0_4;
R0_3 = R0_7 * R0_4;
I0_2 = I0_8;
R0_2 = R0_8;
R0_1 = R0_9 * R0_3;
lab164:
B0_0 = I0_2 < I0_6;
if( !B0_0)
{
goto lab171;
}
R0_13 = (mreal) I0_11;
R0_13 = R0_13 * R0_3;
{
mreal S0[2];
S0[0] = R0_1;
S0[1] = R0_13;
B0_1 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
B0_3 = B0_1;
goto lab172;
lab171:
B0_3 = B0_2;
lab172:
if( !B0_3)
{
goto lab199;
}
I0_10 = I0_0;
lab174:
I0_4 = D34[0];
B0_0 = I0_10 <= I0_4;
if( !B0_0)
{
goto lab195;
}
{
mint S0[3];
S0[0] = I0_10;
S0[1] = I0_0;
S0[2] = I0_0;
err = libData->MTensor_getReal(*T0_10, S0, &R0_13);
if( err)
{
goto error_label;
}
}
R0_12 = -R0_10;
R0_13 = R0_13 + R0_12;
R0_12 = R0_13 < 0 ? -R0_13 : R0_13;
{
mreal S0[2];
S0[0] = R0_12;
S0[1] = R0_1;
B0_1 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_1)
{
goto lab191;
}
{
mint S0[3];
S0[0] = I0_10;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_10, S0, &R0_12);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_12;
S0[1] = R0_2;
B0_4 = funStructCompile->Compare_R(7, R0_6, 2, S0);
}
if( !B0_4)
{
goto lab190;
}
I0_2 = I0_10;
{
mint S0[3];
S0[0] = I0_10;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_10, S0, &R0_12);
if( err)
{
goto error_label;
}
}
R0_2 = R0_12;
goto lab190;
lab190:
goto lab191;
lab191:
I0_4 = I0_10;
{
mint S0 = FP2((void*) (&I0_5), (void*) (&I0_4), (void*) (&I0_0), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
I0_10 = I0_5;
goto lab174;
lab195:
R0_12 = R0_9 * R0_3;
R0_13 = R0_1 + R0_12;
R0_1 = R0_13;
goto lab164;
lab199:
{
mint S0[2];
S0[0] = I0_2;
S0[1] = I0_0;
err = funStructCompile->MTensor_getMTensorInitialized(T0_9, *T0_10, S0, 2);
if( err)
{
goto error_label;
}
}
P25 = MTensor_getRealDataMacro(*T0_9);
D25 = MTensor_getDimensionsMacro(*T0_9);
{
mint S0 = 1;
err = funStructCompile->MTensor_allocate(T0_10, 2, 1, &S0);
if( err)
{
goto error_label;
}
P35 = MTensor_getIntegerDataMacro(*T0_10);
D35 = MTensor_getDimensionsMacro(*T0_10);
P35[0] = I0_8;
}
{
mint S0;
mint S1[2];
S1[0] = 1;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D35[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_7, 2, 2, S1);
if( err)
{
goto error_label;
}
D11 = MTensor_getDimensionsMacro(*T0_7);
S0 = 1;
err = libData->MTensor_setMTensor(*T0_7, *T0_10, &S0, 1);
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = T0_9;
MArgument_getMTensorAddress(FPA[2]) = T0_7;
MArgument_getMTensorAddress(FPA[3]) = T0_2;
err = FP10(libData, 3, FPA, FPA[3]);/*  Insert  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_2;
MArgument_getMTensorAddress(FPA[1]) = T0_3;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_2, *T0_3, &I0_0, 1);
if( err)
{
goto error_label;
}
P7 = MTensor_getRealDataMacro(*T0_2);
D7 = MTensor_getDimensionsMacro(*T0_2);
{
mint S0 = D7[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
R0_3 = P7[S0];
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_2, *T0_3, &I0_9, 1);
if( err)
{
goto error_label;
}
P7 = MTensor_getRealDataMacro(*T0_2);
D7 = MTensor_getDimensionsMacro(*T0_2);
{
mint S0 = D7[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
R0_10 = P7[S0];
}
R0_1 = -R0_10;
R0_3 = R0_3 + R0_1;
R0_1 = R0_3 < 0 ? -R0_3 : R0_3;
R0_3 = -R0_4;
R0_1 = R0_1 + R0_3;
R0_3 = R0_1 < 0 ? -R0_1 : R0_1;
err = funStructCompile->MTensor_getMTensorInitialized(T0_2, *T0_3, &I0_12, 1);
if( err)
{
goto error_label;
}
P7 = MTensor_getRealDataMacro(*T0_2);
D7 = MTensor_getDimensionsMacro(*T0_2);
{
mint S0 = D7[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
R0_1 = P7[S0];
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_2, *T0_3, &I0_9, 1);
if( err)
{
goto error_label;
}
P7 = MTensor_getRealDataMacro(*T0_2);
D7 = MTensor_getDimensionsMacro(*T0_2);
{
mint S0 = D7[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
R0_10 = P7[S0];
}
R0_2 = -R0_10;
R0_1 = R0_1 + R0_2;
R0_2 = R0_1 < 0 ? -R0_1 : R0_1;
R0_1 = -R0_4;
R0_2 = R0_2 + R0_1;
R0_1 = R0_2 < 0 ? -R0_2 : R0_2;
err = funStructCompile->MTensor_getMTensorInitialized(T0_2, *T0_3, &I0_0, 1);
if( err)
{
goto error_label;
}
P7 = MTensor_getRealDataMacro(*T0_2);
D7 = MTensor_getDimensionsMacro(*T0_2);
{
mint S0 = D7[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
R0_2 = P7[S0];
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_2, *T0_3, &I0_12, 1);
if( err)
{
goto error_label;
}
P7 = MTensor_getRealDataMacro(*T0_2);
D7 = MTensor_getDimensionsMacro(*T0_2);
{
mint S0 = D7[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
R0_10 = P7[S0];
}
R0_13 = -R0_10;
R0_2 = R0_2 + R0_13;
R0_13 = R0_2 < 0 ? -R0_2 : R0_2;
R0_2 = -R0_4;
R0_13 = R0_13 + R0_2;
R0_2 = R0_13 < 0 ? -R0_13 : R0_13;
{
mreal S0[2];
S0[0] = R0_3;
S0[1] = R0_1;
B0_3 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_3)
{
goto lab248;
}
{
mreal S0[2];
S0[0] = R0_3;
S0[1] = R0_2;
B0_0 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_0)
{
goto lab240;
}
MArgument_getMTensorAddress(FPA[0]) = T0_11;
MArgument_getMTensorAddress(FPA[1]) = T0_9;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P32 = MTensor_getIntegerDataMacro(*T0_9);
D32 = MTensor_getDimensionsMacro(*T0_9);
goto lab246;
lab240:
{
mreal S0[2];
S0[0] = R0_2;
S0[1] = R0_1;
B0_1 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_1)
{
goto lab244;
}
MArgument_getMTensorAddress(FPA[0]) = T0_12;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P37 = MTensor_getIntegerDataMacro(*T0_2);
D37 = MTensor_getDimensionsMacro(*T0_2);
goto lab245;
lab244:
MArgument_getMTensorAddress(FPA[0]) = T0_13;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P37 = MTensor_getIntegerDataMacro(*T0_2);
D37 = MTensor_getDimensionsMacro(*T0_2);
lab245:
MArgument_getMTensorAddress(FPA[0]) = T0_2;
MArgument_getMTensorAddress(FPA[1]) = T0_9;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P32 = MTensor_getIntegerDataMacro(*T0_9);
D32 = MTensor_getDimensionsMacro(*T0_9);
lab246:
MArgument_getMTensorAddress(FPA[0]) = T0_9;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P37 = MTensor_getIntegerDataMacro(*T0_2);
D37 = MTensor_getDimensionsMacro(*T0_2);
goto lab259;
lab248:
{
mreal S0[2];
S0[0] = R0_1;
S0[1] = R0_2;
B0_0 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_0)
{
goto lab252;
}
MArgument_getMTensorAddress(FPA[0]) = T0_13;
MArgument_getMTensorAddress(FPA[1]) = T0_10;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P35 = MTensor_getIntegerDataMacro(*T0_10);
D35 = MTensor_getDimensionsMacro(*T0_10);
goto lab258;
lab252:
{
mreal S0[2];
S0[0] = R0_2;
S0[1] = R0_3;
B0_1 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_1)
{
goto lab256;
}
MArgument_getMTensorAddress(FPA[0]) = T0_12;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P37 = MTensor_getIntegerDataMacro(*T0_2);
D37 = MTensor_getDimensionsMacro(*T0_2);
goto lab257;
lab256:
MArgument_getMTensorAddress(FPA[0]) = T0_11;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P37 = MTensor_getIntegerDataMacro(*T0_2);
D37 = MTensor_getDimensionsMacro(*T0_2);
lab257:
MArgument_getMTensorAddress(FPA[0]) = T0_2;
MArgument_getMTensorAddress(FPA[1]) = T0_10;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P35 = MTensor_getIntegerDataMacro(*T0_10);
D35 = MTensor_getDimensionsMacro(*T0_10);
lab258:
MArgument_getMTensorAddress(FPA[0]) = T0_10;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P37 = MTensor_getIntegerDataMacro(*T0_2);
D37 = MTensor_getDimensionsMacro(*T0_2);
lab259:
MArgument_getMTensorAddress(FPA[0]) = T0_14;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
MArgument_getMTensorAddress(FPA[2]) = T0_9;
err = FP11(libData, 2, FPA, FPA[2]);/*  Complement  */
if( err)
{
goto error_label;
}
P32 = MTensor_getIntegerDataMacro(*T0_9);
D32 = MTensor_getDimensionsMacro(*T0_9);
{
mint S0 = D32[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
I0_2 = P32[S0];
}
{
mint S0 = D37[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
I0_10 = P37[S0];
}
{
mint S0[2];
S0[0] = I0_10;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_13);
if( err)
{
goto error_label;
}
}
{
mint S0[2];
S0[0] = I0_2;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_10);
if( err)
{
goto error_label;
}
}
R0_12 = -R0_10;
R0_13 = R0_13 + R0_12;
R0_12 = R0_13 < 0 ? -R0_13 : R0_13;
R0_13 = 1 / R0_4;
R0_12 = R0_12 * R0_13;
{
mreal S0[2];
S0[0] = R0_12;
S0[1] = R0_7;
B0_3 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_3)
{
goto lab277;
}
{
mint S0 = D37[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
I0_10 = P37[S0];
}
{
mint S0[2];
S0[0] = I0_10;
S0[1] = I0_9;
err = libData->MTensor_getReal(*T0_3, S0, &R0_12);
if( err)
{
goto error_label;
}
}
{
mint S0[2];
S0[0] = I0_2;
S0[1] = I0_9;
err = libData->MTensor_getReal(*T0_3, S0, &R0_13);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_12;
S0[1] = R0_13;
B0_0 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
B0_1 = B0_0;
goto lab278;
lab277:
B0_1 = B0_2;
lab278:
if( !B0_1)
{
goto lab283;
}
{
mint S0 = D37[0];
if( I0_9 > 0)
{
if( I0_9 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_9 - 1;
}
else
{
if( I0_9 == 0 || I0_9 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_9;
}
I0_10 = P37[S0];
}
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_9, 2, 1, &S0);
if( err)
{
goto error_label;
}
P32 = MTensor_getIntegerDataMacro(*T0_9);
D32 = MTensor_getDimensionsMacro(*T0_9);
P32[0] = I0_2;
P32[1] = I0_10;
}
MArgument_getMTensorAddress(FPA[0]) = T0_9;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P37 = MTensor_getIntegerDataMacro(*T0_2);
D37 = MTensor_getDimensionsMacro(*T0_2);
goto lab283;
lab283:
{
mint S0 = D37[0];
if( I0_9 > 0)
{
if( I0_9 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_9 - 1;
}
else
{
if( I0_9 == 0 || I0_9 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_9;
}
I0_10 = P37[S0];
}
{
mint S0[2];
S0[0] = I0_10;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_12);
if( err)
{
goto error_label;
}
}
{
mint S0[2];
S0[0] = I0_2;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_13);
if( err)
{
goto error_label;
}
}
R0_10 = -R0_13;
R0_12 = R0_12 + R0_10;
R0_10 = R0_12 < 0 ? -R0_12 : R0_12;
R0_12 = 1 / R0_4;
R0_10 = R0_10 * R0_12;
{
mreal S0[2];
S0[0] = R0_10;
S0[1] = R0_7;
B0_1 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_1)
{
goto lab299;
}
{
mint S0 = D37[0];
if( I0_9 > 0)
{
if( I0_9 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_9 - 1;
}
else
{
if( I0_9 == 0 || I0_9 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_9;
}
I0_10 = P37[S0];
}
{
mint S0[2];
S0[0] = I0_10;
S0[1] = I0_9;
err = libData->MTensor_getReal(*T0_3, S0, &R0_10);
if( err)
{
goto error_label;
}
}
{
mint S0[2];
S0[0] = I0_2;
S0[1] = I0_9;
err = libData->MTensor_getReal(*T0_3, S0, &R0_12);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_10;
S0[1] = R0_12;
B0_3 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
B0_0 = B0_3;
goto lab300;
lab299:
B0_0 = B0_2;
lab300:
if( !B0_0)
{
goto lab305;
}
{
mint S0 = D37[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
I0_10 = P37[S0];
}
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_9, 2, 1, &S0);
if( err)
{
goto error_label;
}
P32 = MTensor_getIntegerDataMacro(*T0_9);
D32 = MTensor_getDimensionsMacro(*T0_9);
P32[0] = I0_10;
P32[1] = I0_2;
}
MArgument_getMTensorAddress(FPA[0]) = T0_9;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P37 = MTensor_getIntegerDataMacro(*T0_2);
D37 = MTensor_getDimensionsMacro(*T0_2);
goto lab305;
lab305:
{
mint S0 = D37[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
I0_10 = P37[S0];
}
{
mint S0[2];
S0[0] = I0_10;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_10);
if( err)
{
goto error_label;
}
}
{
mint S0 = D37[0];
if( I0_9 > 0)
{
if( I0_9 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_9 - 1;
}
else
{
if( I0_9 == 0 || I0_9 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_9;
}
I0_10 = P37[S0];
}
{
mint S0[2];
S0[0] = I0_10;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_12);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_10;
S0[1] = R0_12;
B0_0 = funStructCompile->Compare_R(3, R0_6, 2, S0);
}
if( !B0_0)
{
goto lab318;
}
{
mint S0 = D37[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
I0_10 = P37[S0];
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_9, *T0_3, &I0_10, 1);
if( err)
{
goto error_label;
}
P25 = MTensor_getRealDataMacro(*T0_9);
D25 = MTensor_getDimensionsMacro(*T0_9);
{
mint S0 = D37[0];
if( I0_9 > 0)
{
if( I0_9 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_9 - 1;
}
else
{
if( I0_9 == 0 || I0_9 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_9;
}
I0_10 = P37[S0];
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_10, *T0_3, &I0_10, 1);
if( err)
{
goto error_label;
}
{
mint S0;
mint S1[2];
S1[0] = 2;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D25[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_7, 3, 2, S1);
if( err)
{
goto error_label;
}
S0 = 1;
err = libData->MTensor_setMTensor(*T0_7, *T0_9, &S0, 1);
if( err)
{
goto error_label;
}
S0 = 2;
err = libData->MTensor_setMTensor(*T0_7, *T0_10, &S0, 1);
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_7;
MArgument_getMTensorAddress(FPA[1]) = T0_9;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
goto lab324;
lab318:
{
mint S0 = D37[0];
if( I0_9 > 0)
{
if( I0_9 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_9 - 1;
}
else
{
if( I0_9 == 0 || I0_9 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_9;
}
I0_10 = P37[S0];
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_9, *T0_3, &I0_10, 1);
if( err)
{
goto error_label;
}
P25 = MTensor_getRealDataMacro(*T0_9);
D25 = MTensor_getDimensionsMacro(*T0_9);
{
mint S0 = D37[0];
if( I0_0 > 0)
{
if( I0_0 > S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = I0_0 - 1;
}
else
{
if( I0_0 == 0 || I0_0 < -S0)
{
return LIBRARY_DIMENSION_ERROR;
}
S0 = S0 + I0_0;
}
I0_10 = P37[S0];
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_10, *T0_3, &I0_10, 1);
if( err)
{
goto error_label;
}
{
mint S0;
mint S1[2];
S1[0] = 2;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D25[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_15, 3, 2, S1);
if( err)
{
goto error_label;
}
S0 = 1;
err = libData->MTensor_setMTensor(*T0_15, *T0_9, &S0, 1);
if( err)
{
goto error_label;
}
S0 = 2;
err = libData->MTensor_setMTensor(*T0_15, *T0_10, &S0, 1);
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_15;
MArgument_getMTensorAddress(FPA[1]) = T0_9;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
lab324:
funStructCompile->MTensor_copy(Res, *T0_9);
error_label:
funStructCompile->ReleaseInitializedMTensors(Tinit);
funStructCompile->WolframLibraryData_cleanUp(libData, 1);
return err;
}

