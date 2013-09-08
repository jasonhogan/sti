#include "cscope definitions.h"

#pragma pack(push)
#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif


void __stdcall CscopeControlDriver(long AcquisitionUnit,
	unsigned short Command, double ReplayStartTime, double ReplayStopTime,
	long SamplesInReplay, long FrameNumber, TD1 *AcquireDefinition,
	bool *GotSamples, double *T0, double *dT, unsigned long *NumSamples,
	unsigned long *NumFrames, float ChanAData[], long ChanAAllocSpace,
	float ChanBData[], long ChanBAllocSpace, unsigned short DigitalInputData[],
	long DigInpAllocSpace, unsigned short *CAUStatus, TD2 *errorOut);

long __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

#ifdef __cplusplus
} // extern "C"
#endif

#pragma pack(pop)

