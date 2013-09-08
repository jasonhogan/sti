#pragma pack(push)
#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	unsigned short AcquireMode;
	unsigned short AcquisitionMode;
	unsigned short Acquirer;
	unsigned short TransferChans;
	double AMaxScale;
	double AMinScale;
	double BMaxScale;
	double BMinScale;
	unsigned short AProbe;
	unsigned short BProbe;
	unsigned short ACoupling;
	unsigned short BCoupling;
	unsigned short ABandwidth;
	unsigned short BBandwidth;
	unsigned long TriggerSource;
	double TriggerAmplitude;
	double ATriggerAmplitude;
	double BTriggerAmplitude;
	unsigned short TriggerFilter;
	LVBoolean TrigSlope;
	double TriggerHoldoff;
	LVBoolean DigPatternRqd;
	unsigned long DigPattern;
	double ExtTrigThreshold;
	double DigInputThreshold;
	double StartTime;
	double StopTime;
	double PreTrigTime;
	unsigned short Port;
	short NumDivisions;
	short NumSeqFrames;
	long NumBuffers;
	double SigGenFreq;
	double SigGenAmp;
	double SigGenOffset;
	unsigned short SigGenWaveform;
	unsigned short SigGenSweep;
	unsigned short SigGenFunc;
	double SigGenFreq2;
	double SigGenPhase;
	unsigned short Trig2Function;
	double MinTriggerPeriod;
	double MaxTriggerPeriod;
	unsigned long TriggerCount;
	LVBoolean Trig2Slope;
	unsigned long Trig2SourceChan;
	double Trig2Level;
	LVBoolean DigPattern2Rqd;
	unsigned long DigPattern2;
	unsigned short Trigger2Source;
	long WaveformAverages;
	long ValueChanged;
	double FreqSpan;
	double FreqRes;
	double Duration;
	double Resolution;
	unsigned char LinkPort;
	LVBoolean ExtSampleClock;
	LVBoolean FSpare2;
	LVBoolean FSpare3;
	LVBoolean FSpare4;
	unsigned short SamplerResolution;
	unsigned short IntfSource;
	unsigned short UpdateRate;
	unsigned short TransferSize;
	double SigGenFreqStep;
	unsigned long TCPAdr;
	unsigned long TCPPort;
	unsigned long CAUSerNumHi;
	unsigned long CAUSerNumLo;
	double FunctionNumber;
	double FunctionParameter;
	double FunctionResult;
	unsigned long LinkStart;
	unsigned long LinkTimebase;
	unsigned long LinkTimer;
	unsigned long LinkSetup;
	double Spare1;
	double Spare2;
	double Spare3;
	double Spare4;
	} acquire_defn;

typedef struct {
	LVBoolean status;
	long code;
	LStrHandle source;
	} TD2;


void __stdcall CscopeControlDriver(long AcquisitionUnit, 
	unsigned short Command, double ReplayStartTime, double ReplayStopTime, 
	long SamplesInReplay, long FrameNumber, acquire_defn *AcquireDefinition, 
	LVBoolean *GotSamples, double *T0, double *dT, unsigned long *NumSamples, 
	unsigned long *NumFrames, float ChanAData[], long ChanAAllocSpace, 
	float ChanBData[], long ChanBAllocSpace, unsigned short DigitalInputData[], 
	long DigInpAllocSpace, unsigned short *CAUStatus, TD2 *errorOut);
void __stdcall CscopeFunction(long AcquisitionUnit, 
	unsigned short FunctionCommand, double Parameter, 
	unsigned char LinkDataSend[], long LinkDataSendAllocSpace, 
	double *FunctionResult, unsigned char LinkDataReceived[], 
	long LinkDataReceivedAllocSpace, TD2 *errorOut);

long __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

#ifdef __cplusplus
} // extern "C"
#endif

#pragma pack(pop)

