//---------------------------------------------------------------------------
#ifndef nmccomH
#define nmccomH
//---------------------------------------------------------------------------
#endif
typedef unsigned char byte;

typedef struct _NMCMOD {
	byte	modtype;		//module type
    byte	modver;			//module version number
	byte	statusitems;	//definition of items to be returned
	byte	stat;  			//status byte
    byte	groupaddr;		//current group address
    BOOL	groupleader;	//TRUE if group leader
    void *	p;				//pointer to specific module's data structure
    } NMCMOD;

#define MAXSIOERROR 2

//Define PIC baud rate divisors
#define	PB19200		63
#define	PB57600		20
#define	PB115200	10

//Module type definitions:
#define	SERVOMODTYPE	0
#define	ADCMODTYPE		1
#define	IOMODTYPE		2
#define	STEPMODTYPE		3
//The following must be created for each new module type:
//		data structure XXXMOD
//		Initializer function NewXXXMod
//		Status reading function GetXXXStat
//		NMCInit and SendNmcCmd must be modified to include calls
//			to the two functions above

#define CKSUM_ERROR		0x02	//Checksum error bit in status byte

//--------------------- ADC Module specific stuff --------------------------
typedef struct _ADCMOD {
    //******  Move all this data to the NMCMOD structure *******
	short int ad0;	//definition of items to be returned
	short int ad1;
	short int ad2;
	short int ad3;
	short int ad4;
	short int ad5;
    } ADCMOD;


#define MAXNUMMOD	33

//Function prototypes:
extern "C" ADCMOD * WINAPI __declspec(dllexport) AdcNewMod();
extern "C" WINAPI __declspec(dllexport) BOOL AdcGetStat(byte addr);

//Initialization and shutdown
extern "C" WINAPI __declspec(dllexport) int NmcInit(char *portname, unsigned int baudrate);
extern "C" WINAPI __declspec(dllexport) void InitVars(void);
extern "C" WINAPI __declspec(dllexport) BOOL NmcSendCmd(byte addr, byte cmd, char *datastr, byte n, byte stataddr);
extern "C" WINAPI __declspec(dllexport) void FixSioError(byte addr);
extern "C" WINAPI __declspec(dllexport) void NmcShutdown(void);

//Module type independant commands (supported by all module types)
extern "C" WINAPI __declspec(dllexport) BOOL NmcSetGroupAddr(byte addr, byte groupaddr, BOOL leader);
extern "C" WINAPI __declspec(dllexport) BOOL NmcDefineStatus(byte addr, byte statusitems);
extern "C" WINAPI __declspec(dllexport) BOOL NmcReadStatus(byte addr, byte statusitems);
extern "C" WINAPI __declspec(dllexport) BOOL NmcSynchOutput(byte groupaddr, byte leaderaddr);
extern "C" WINAPI __declspec(dllexport) BOOL NmcChangeBaud(byte groupaddr, unsigned int baudrate);
extern "C" WINAPI __declspec(dllexport) BOOL NmcSynchInput(byte groupaddr, byte leaderaddr);
extern "C" WINAPI __declspec(dllexport) BOOL NmcNoOp(byte addr);
extern "C" WINAPI __declspec(dllexport) BOOL NmcHardReset(byte addr);

//Retrieve module type independant data from a module's data structure
extern "C" WINAPI __declspec(dllexport) byte NmcGetStat(byte addr);
extern "C" WINAPI __declspec(dllexport) byte NmcGetStatItems(byte addr);
extern "C" WINAPI __declspec(dllexport) byte NmcGetModType(byte addr);
extern "C" WINAPI __declspec(dllexport) byte NmcGetModVer(byte addr);
extern "C" WINAPI __declspec(dllexport) byte NmcGetGroupAddr(byte addr);
extern "C" WINAPI __declspec(dllexport) BOOL NmcGroupLeader(byte addr);




