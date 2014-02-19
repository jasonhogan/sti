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

static MArgument FPA[4];


static mbool B0_2;

static mint I0_0;

static mint I0_6;

static mint I0_8;

static mint I0_9;

static mint I0_11;

static mreal R0_8;

static mreal R0_9;

static mreal R0_13;

static mreal R0_14;

static MTensor T0_9B = 0;

static MTensor* T0_9 = &T0_9B;

static mint *P2;

static mbool initialize = 1;

#include "findFirstSidebands.h"

DLLEXPORT int Initialize_findFirstSidebands(WolframLibraryData libData)
{
int err = 0;
if( initialize)
{
funStructCompile = libData->compileLibraryFunctions;
I0_0 = (mint) 1;
R0_8 = (mreal) 1.;
R0_9 = (mreal) 7.;
I0_9 = (mint) 2;
I0_8 = (mint) -1;
R0_14 = (mreal) 0.5;
B0_2 = (mbool) 0;
R0_13 = (mreal) 0.;
I0_6 = (mint) 0;
I0_11 = (mint) 100;
{
mint S0[2];
S0[0] = 1;
S0[1] = 1;
err = funStructCompile->MTensor_allocate(T0_9, 2, 2, S0);
if( err)
{
goto error_label;
}
P2 = MTensor_getIntegerDataMacro(*T0_9);
P2[0] = 0;
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
initialize = 0;
}
error_label:
return err;
}

DLLEXPORT void Uninitialize_findFirstSidebands(WolframLibraryData libData)
{
if( !initialize)
{
initialize = 1;
}
}

DLLEXPORT int findFirstSidebands(WolframLibraryData libData, MTensor A1, MTensor A2, mreal A3, mreal A4, mreal A5, mreal A6, MTensor *Res)
{
mbool B0_0;
mbool B0_1;
mbool B0_3;
mbool B0_4;
mbool B0_5;
mbool B0_6;
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
mreal R0_5;
mreal R0_6;
mreal R0_7;
mreal R0_10;
mreal R0_11;
mreal R0_12;
mreal R0_15;
mreal R0_16;
mreal R0_17;
mreal R0_18;
MTensor* T0_0;
MTensor* T0_1;
MTensor* T0_2;
MTensor* T0_3;
MTensor* T0_4;
MTensor* T0_5;
MTensor* T0_6;
MTensor* T0_7;
MTensor* T0_8;
MTensor* T0_10;
MTensor* T0_11;
MTensorInitializationData Tinit;
mint dims[3];
mint *D3;
mreal *P4;
mint *D4;
mreal *P5;
mint *D5;
mreal *P6;
mint *D6;
mint *D8;
mreal *P9;
mint *D9;
mreal *P11;
mint *D11;
mint *P12;
mint *D14;
mint *P15;
mint *D15;
mint *P16;
mint *D16;
mint *P18;
mreal *P22;
mint *D25;
mreal *P26;
mint *D26;
MArgument FPA[4];
int err = 0;
Tinit = funStructCompile->GetInitializedMTensors(libData, 9);
T0_10 = MTensorInitializationData_getTensor(Tinit, 0);
T0_11 = MTensorInitializationData_getTensor(Tinit, 1);
T0_2 = MTensorInitializationData_getTensor(Tinit, 2);
T0_3 = MTensorInitializationData_getTensor(Tinit, 3);
T0_4 = MTensorInitializationData_getTensor(Tinit, 4);
T0_5 = MTensorInitializationData_getTensor(Tinit, 5);
T0_6 = MTensorInitializationData_getTensor(Tinit, 6);
T0_7 = MTensorInitializationData_getTensor(Tinit, 7);
T0_8 = MTensorInitializationData_getTensor(Tinit, 8);
T0_0 = &A1;
T0_1 = &A2;
R0_0 = A3;
R0_1 = A4;
R0_2 = A5;
R0_3 = A6;
MArgument_getMTensorAddress(FPA[0]) = T0_0;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
D3 = MTensor_getDimensionsMacro(*T0_2);
I0_2 = D3[0];
I0_5 = I0_6;
dims[0] = I0_2;
err = funStructCompile->MTensor_allocate(T0_3, 3, 1, dims);
if( err)
{
goto error_label;
}
P4 = MTensor_getRealDataMacro(*T0_3);
D4 = MTensor_getDimensionsMacro(*T0_3);
I0_4 = I0_6;
goto lab15;
lab7:
{
mint S0[2];
S0[0] = I0_4;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_2, S0, &R0_6);
if( err)
{
goto error_label;
}
}
I0_3 = D3[0];
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
err = libData->MTensor_getReal(*T0_2, S0, &R0_5);
if( err)
{
goto error_label;
}
}
R0_7 = -R0_5;
R0_6 = R0_6 + R0_7;
P4[I0_5++] = R0_6;
lab15:
if( ++I0_4 <= I0_2)
{
goto lab7;
}
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = T0_4;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
P5 = MTensor_getRealDataMacro(*T0_4);
D5 = MTensor_getDimensionsMacro(*T0_4);
MArgument_getMTensorAddress(FPA[0]) = T0_4;
MArgument_getMTensorAddress(FPA[1]) = T0_5;
err = FP3(libData, 1, FPA, FPA[1]);/*  Union  */
if( err)
{
goto error_label;
}
P6 = MTensor_getRealDataMacro(*T0_5);
D6 = MTensor_getDimensionsMacro(*T0_5);
I0_2 = D6[0];
I0_3 = I0_8;
dims[0] = I0_2;
dims[1] = I0_3;
err = funStructCompile->MTensor_allocate(T0_7, 3, 2, dims);
if( err)
{
goto error_label;
}
I0_4 = I0_6;
goto lab30;
lab23:
R0_4 = P6[I0_4 - 1];
{
MTensor S0 = funStructCompile->getRankZeroMTensor((void*) (&R0_4), 3, 0);
MArgument_getMTensorAddress(FPA[0]) = T0_4;
MArgument_getMTensorAddress(FPA[1]) = &S0;
MArgument_getRealAddress(FPA[2]) = &R0_8;
MArgument_getMTensorAddress(FPA[3]) = T0_8;
err = FP4(libData, 3, FPA, FPA[3]);/*  Position  */
if( err)
{
goto error_label;
}
}
D8 = MTensor_getDimensionsMacro(*T0_8);
I0_7 = D8[0];
R0_4 = P6[I0_4 - 1];
R0_7 = (mreal) I0_7;
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_8, 3, 1, &S0);
if( err)
{
goto error_label;
}
P9 = MTensor_getRealDataMacro(*T0_8);
D9 = MTensor_getDimensionsMacro(*T0_8);
P9[0] = R0_7;
P9[1] = R0_4;
}
err = funStructCompile->MTensor_insertMTensor(*T0_7, *T0_8, &I0_3);
if( err)
{
goto error_label;
}
lab30:
if( ++I0_4 <= I0_2)
{
goto lab23;
}
MArgument_getMTensorAddress(FPA[0]) = T0_7;
MArgument_getMTensorAddress(FPA[1]) = T0_5;
err = FP5(libData, 1, FPA, FPA[1]);/*  Sort  */
if( err)
{
goto error_label;
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_7, *T0_5, &I0_8, 1);
if( err)
{
goto error_label;
}
P11 = MTensor_getRealDataMacro(*T0_7);
D11 = MTensor_getDimensionsMacro(*T0_7);
{
mint S0 = D11[0];
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
R0_4 = P11[S0];
}
{
mint S0 = 1;
err = funStructCompile->MTensor_allocate(T0_7, 3, 1, &S0);
if( err)
{
goto error_label;
}
P11 = MTensor_getRealDataMacro(*T0_7);
D11 = MTensor_getDimensionsMacro(*T0_7);
P11[0] = R0_4;
}
{
mint S0 = D11[0];
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
R0_4 = P11[S0];
}
R0_7 = R0_4 < 0 ? -R0_4 : R0_4;
I0_3 = D4[0];
I0_4 = I0_6;
dims[0] = I0_3;
err = funStructCompile->MTensor_allocate(T0_4, 2, 1, dims);
if( err)
{
goto error_label;
}
P12 = MTensor_getIntegerDataMacro(*T0_4);
I0_2 = I0_6;
goto lab52;
lab42:
R0_5 = P4[I0_2 - 1];
R0_4 = R0_5 < 0 ? -R0_5 : R0_5;
R0_5 = (mreal) I0_9;
R0_5 = R0_5 * R0_7;
{
mreal S0[2];
S0[0] = R0_4;
S0[1] = R0_5;
B0_0 = funStructCompile->Compare_R(7, R0_9, 2, S0);
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
P12[I0_4++] = I0_10;
lab52:
if( ++I0_2 <= I0_3)
{
goto lab42;
}
{
MTensor S0 = funStructCompile->getRankZeroMTensor((void*) (&I0_0), 2, 0);
MArgument_getMTensorAddress(FPA[0]) = T0_4;
MArgument_getMTensorAddress(FPA[1]) = &S0;
MArgument_getMTensorAddress(FPA[2]) = T0_5;
err = FP4(libData, 2, FPA, FPA[2]);/*  Position  */
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_9;
MArgument_getMTensorAddress(FPA[1]) = T0_5;
MArgument_getMTensorAddress(FPA[2]) = T0_4;
err = FP6(libData, 2, FPA, FPA[2]);/*  Join  */
if( err)
{
goto error_label;
}
D14 = MTensor_getDimensionsMacro(*T0_4);
I0_5 = D14[0];
I0_10 = I0_6;
dims[0] = I0_5;
err = funStructCompile->MTensor_allocate(T0_5, 2, 1, dims);
if( err)
{
goto error_label;
}
P15 = MTensor_getIntegerDataMacro(*T0_5);
D15 = MTensor_getDimensionsMacro(*T0_5);
I0_7 = I0_6;
goto lab63;
lab60:
funStructCompile->MTensor_getMTensorInitialized(T0_6, *T0_4, &I0_7, 1);
P16 = MTensor_getIntegerDataMacro(*T0_6);
D16 = MTensor_getDimensionsMacro(*T0_6);
{
mint S0 = D16[0];
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
I0_3 = P16[S0];
}
P15[I0_10++] = I0_3;
lab63:
if( ++I0_7 <= I0_5)
{
goto lab60;
}
I0_2 = D15[0];
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
err = funStructCompile->MTensor_allocate(T0_6, 3, 3, dims);
if( err)
{
goto error_label;
}
I0_10 = I0_6;
goto lab87;
lab70:
{
mint S0 = D15[0];
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
I0_3 = P15[S0];
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
mint S0 = D15[0];
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
I0_5 = P15[S0];
}
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_8, 2, 1, &S0);
if( err)
{
goto error_label;
}
P18 = MTensor_getIntegerDataMacro(*T0_8);
P18[0] = I0_4;
P18[1] = I0_5;
}
MArgument_getMTensorAddress(FPA[0]) = T0_2;
MArgument_getIntegerAddress(FPA[1]) = &I0_0;
MArgument_getMTensorAddress(FPA[2]) = T0_8;
MArgument_getMTensorAddress(FPA[3]) = T0_4;
err = FP7(libData, 3, FPA, FPA[3]);/*  Take  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_4;
MArgument_getMTensorAddress(FPA[1]) = T0_8;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_8;
MArgument_getRealAddress(FPA[1]) = &R0_4;
err = FP8(libData, 1, FPA, FPA[1]);/*  MaxRT  */
if( err)
{
goto error_label;
}
{
MTensor S0 = funStructCompile->getRankZeroMTensor((void*) (&R0_4), 3, 0);
MArgument_getMTensorAddress(FPA[0]) = T0_8;
MArgument_getMTensorAddress(FPA[1]) = &S0;
MArgument_getRealAddress(FPA[2]) = &R0_8;
MArgument_getMTensorAddress(FPA[3]) = T0_10;
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
err = libData->MTensor_getInteger(*T0_10, S0, &I0_4);
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
err = funStructCompile->MTensor_getPart(T0_8, *T0_4, 2, S0, S1);
if( err)
{
goto error_label;
}
}
P9 = MTensor_getRealDataMacro(*T0_8);
D9 = MTensor_getDimensionsMacro(*T0_8);
MArgument_getMTensorAddress(FPA[0]) = T0_8;
MArgument_getRealAddress(FPA[1]) = &R0_4;
err = FP9(libData, 1, FPA, FPA[1]);/*  MinRT  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_8;
MArgument_getRealAddress(FPA[1]) = &R0_5;
err = FP8(libData, 1, FPA, FPA[1]);/*  MaxRT  */
if( err)
{
goto error_label;
}
{
mint S0 = 2;
err = funStructCompile->MTensor_allocate(T0_10, 3, 1, &S0);
if( err)
{
goto error_label;
}
P22 = MTensor_getRealDataMacro(*T0_10);
P22[0] = R0_4;
P22[1] = R0_5;
}
err = funStructCompile->MTensor_getMTensorInitialized(T0_8, *T0_4, &I0_4, 1);
if( err)
{
goto error_label;
}
P9 = MTensor_getRealDataMacro(*T0_8);
D9 = MTensor_getDimensionsMacro(*T0_8);
{
mint S0;
mint S1[2];
S1[0] = 2;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D9[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_11, 3, 2, S1);
if( err)
{
goto error_label;
}
S0 = 1;
err = libData->MTensor_setMTensor(*T0_11, *T0_8, &S0, 1);
if( err)
{
goto error_label;
}
S0 = 2;
err = libData->MTensor_setMTensor(*T0_11, *T0_10, &S0, 1);
if( err)
{
goto error_label;
}
}
err = funStructCompile->MTensor_insertMTensor(*T0_6, *T0_11, &I0_1);
if( err)
{
goto error_label;
}
lab87:
if( ++I0_10 <= I0_2)
{
goto lab70;
}
MArgument_getMTensorAddress(FPA[0]) = T0_6;
MArgument_getMTensorAddress(FPA[1]) = T0_2;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
MArgument_getMTensorAddress(FPA[0]) = T0_1;
MArgument_getMTensorAddress(FPA[1]) = T0_5;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
R0_7 = R0_0;
R0_4 = R0_2;
R0_5 = R0_1;
R0_6 = R0_3;
MArgument_getMTensorAddress(FPA[0]) = T0_2;
MArgument_getMTensorAddress(FPA[1]) = T0_3;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
D25 = MTensor_getDimensionsMacro(*T0_3);
{
mint S0[2];
S0[0] = I0_0;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_5, S0, &R0_10);
if( err)
{
goto error_label;
}
}
R0_10 = R0_10 + R0_7 + R0_6;
R0_11 = R0_4;
R0_12 = R0_5;
I0_1 = I0_8;
R0_15 = R0_13;
R0_16 = R0_14 * R0_11;
lab102:
B0_3 = I0_1 < I0_6;
if( !B0_3)
{
goto lab109;
}
R0_17 = (mreal) I0_11;
R0_17 = R0_17 * R0_11;
{
mreal S0[2];
S0[0] = R0_16;
S0[1] = R0_17;
B0_0 = funStructCompile->Compare_R(3, R0_9, 2, S0);
}
B0_1 = B0_0;
goto lab110;
lab109:
B0_1 = B0_2;
lab110:
if( !B0_1)
{
goto lab143;
}
I0_2 = I0_0;
lab112:
I0_10 = D25[0];
B0_3 = I0_2 <= I0_10;
if( !B0_3)
{
goto lab139;
}
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_17);
if( err)
{
goto error_label;
}
}
R0_18 = -R0_10;
R0_17 = R0_17 + R0_18;
R0_18 = R0_17 < 0 ? -R0_17 : R0_17;
{
mreal S0[2];
S0[0] = R0_18;
S0[1] = R0_16;
B0_0 = funStructCompile->Compare_R(3, R0_9, 2, S0);
}
if( !B0_0)
{
goto lab135;
}
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_3, S0, &R0_18);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_18;
S0[1] = R0_15;
B0_6 = funStructCompile->Compare_R(7, R0_9, 2, S0);
}
if( !B0_6)
{
goto lab128;
}
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_18);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_18;
S0[1] = R0_12;
B0_4 = funStructCompile->Compare_R(7, R0_9, 2, S0);
}
B0_5 = B0_4;
goto lab129;
lab128:
B0_5 = B0_2;
lab129:
if( !B0_5)
{
goto lab134;
}
I0_1 = I0_2;
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_3, S0, &R0_18);
if( err)
{
goto error_label;
}
}
R0_15 = R0_18;
goto lab134;
lab134:
goto lab135;
lab135:
I0_10 = I0_2;
{
mint S0 = FP2((void*) (&I0_4), (void*) (&I0_10), (void*) (&I0_0), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
I0_2 = I0_4;
goto lab112;
lab139:
R0_18 = R0_14 * R0_11;
R0_17 = R0_16 + R0_18;
R0_16 = R0_17;
goto lab102;
lab143:
{
mint S0[2];
S0[0] = I0_1;
S0[1] = I0_0;
err = funStructCompile->MTensor_getMTensorInitialized(T0_11, *T0_3, S0, 2);
if( err)
{
goto error_label;
}
}
P26 = MTensor_getRealDataMacro(*T0_11);
D26 = MTensor_getDimensionsMacro(*T0_11);
MArgument_getMTensorAddress(FPA[0]) = T0_2;
MArgument_getMTensorAddress(FPA[1]) = T0_3;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
D25 = MTensor_getDimensionsMacro(*T0_3);
{
mint S0[2];
S0[0] = I0_9;
S0[1] = I0_0;
err = libData->MTensor_getReal(*T0_5, S0, &R0_12);
if( err)
{
goto error_label;
}
}
R0_11 = -R0_7;
R0_12 = R0_12 + R0_11 + R0_6;
R0_11 = R0_4;
R0_10 = R0_5;
I0_1 = I0_8;
R0_16 = R0_13;
R0_15 = R0_14 * R0_11;
lab153:
B0_0 = I0_1 < I0_6;
if( !B0_0)
{
goto lab160;
}
R0_17 = (mreal) I0_11;
R0_17 = R0_17 * R0_11;
{
mreal S0[2];
S0[0] = R0_15;
S0[1] = R0_17;
B0_1 = funStructCompile->Compare_R(3, R0_9, 2, S0);
}
B0_3 = B0_1;
goto lab161;
lab160:
B0_3 = B0_2;
lab161:
if( !B0_3)
{
goto lab194;
}
I0_2 = I0_0;
lab163:
I0_10 = D25[0];
B0_0 = I0_2 <= I0_10;
if( !B0_0)
{
goto lab190;
}
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_17);
if( err)
{
goto error_label;
}
}
R0_18 = -R0_12;
R0_17 = R0_17 + R0_18;
R0_18 = R0_17 < 0 ? -R0_17 : R0_17;
{
mreal S0[2];
S0[0] = R0_18;
S0[1] = R0_15;
B0_1 = funStructCompile->Compare_R(3, R0_9, 2, S0);
}
if( !B0_1)
{
goto lab186;
}
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_3, S0, &R0_18);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_18;
S0[1] = R0_16;
B0_4 = funStructCompile->Compare_R(7, R0_9, 2, S0);
}
if( !B0_4)
{
goto lab179;
}
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_0;
err = libData->MTensor_getReal(*T0_3, S0, &R0_18);
if( err)
{
goto error_label;
}
}
{
mreal S0[2];
S0[0] = R0_18;
S0[1] = R0_10;
B0_5 = funStructCompile->Compare_R(7, R0_9, 2, S0);
}
B0_6 = B0_5;
goto lab180;
lab179:
B0_6 = B0_2;
lab180:
if( !B0_6)
{
goto lab185;
}
I0_1 = I0_2;
{
mint S0[3];
S0[0] = I0_2;
S0[1] = I0_0;
S0[2] = I0_9;
err = libData->MTensor_getReal(*T0_3, S0, &R0_18);
if( err)
{
goto error_label;
}
}
R0_16 = R0_18;
goto lab185;
lab185:
goto lab186;
lab186:
I0_10 = I0_2;
{
mint S0 = FP2((void*) (&I0_4), (void*) (&I0_10), (void*) (&I0_0), 1, UnitIncrements, 4);/*  Plus  */
err = S0 == 0 ? 0 : LIBRARY_NUMERICAL_ERROR;
if( err)
{
goto error_label;
}
}
I0_2 = I0_4;
goto lab163;
lab190:
R0_18 = R0_14 * R0_11;
R0_17 = R0_15 + R0_18;
R0_15 = R0_17;
goto lab153;
lab194:
{
mint S0[2];
S0[0] = I0_1;
S0[1] = I0_0;
err = funStructCompile->MTensor_getMTensorInitialized(T0_4, *T0_3, S0, 2);
if( err)
{
goto error_label;
}
}
P5 = MTensor_getRealDataMacro(*T0_4);
D5 = MTensor_getDimensionsMacro(*T0_4);
{
mint S0 = D26[0];
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
R0_10 = P26[S0];
}
{
mint S0 = D5[0];
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
R0_11 = P5[S0];
}
{
mreal S0[2];
S0[0] = R0_10;
S0[1] = R0_11;
B0_3 = funStructCompile->Compare_R(3, R0_9, 2, S0);
}
if( !B0_3)
{
goto lab202;
}
{
mint S0;
mint S1[2];
S1[0] = 2;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D26[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_3, 3, 2, S1);
if( err)
{
goto error_label;
}
S0 = 1;
err = libData->MTensor_setMTensor(*T0_3, *T0_11, &S0, 1);
if( err)
{
goto error_label;
}
S0 = 2;
err = libData->MTensor_setMTensor(*T0_3, *T0_4, &S0, 1);
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_3;
MArgument_getMTensorAddress(FPA[1]) = T0_10;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
goto lab204;
lab202:
{
mint S0;
mint S1[2];
S1[0] = 2;
for( S0 = 1; S0 < 2; S0++)
{
S1[S0] = D5[S0 - 1];
}
err = funStructCompile->MTensor_allocate(T0_7, 3, 2, S1);
if( err)
{
goto error_label;
}
S0 = 1;
err = libData->MTensor_setMTensor(*T0_7, *T0_4, &S0, 1);
if( err)
{
goto error_label;
}
S0 = 2;
err = libData->MTensor_setMTensor(*T0_7, *T0_11, &S0, 1);
if( err)
{
goto error_label;
}
}
MArgument_getMTensorAddress(FPA[0]) = T0_7;
MArgument_getMTensorAddress(FPA[1]) = T0_10;
err = FP0(libData, 1, FPA, FPA[1]);/*  CopyTensor  */
if( err)
{
goto error_label;
}
lab204:
funStructCompile->MTensor_copy(Res, *T0_10);
error_label:
funStructCompile->ReleaseInitializedMTensors(Tinit);
funStructCompile->WolframLibraryData_cleanUp(libData, 1);
return err;
}

