//---------------------------------------------------------------------------
#ifndef sio_utilH
#define sio_utilH
//---------------------------------------------------------------------------
#endif
extern "C" WINAPI __declspec(dllexport) void ErrorPrinting(int f);
extern "C" WINAPI __declspec(dllexport) int ErrorMsgBox(char *msgstr);
extern "C" WINAPI __declspec(dllexport) int SimpleMsgBox(char *msgstr);
extern "C" WINAPI __declspec(dllexport) HANDLE SioOpen(char *name, unsigned int baudrate);
extern "C" WINAPI __declspec(dllexport) BOOL SioPutChars(HANDLE ComPort, char *stuff, int n);
extern "C" WINAPI __declspec(dllexport) DWORD SioGetChars(HANDLE ComPort, char *stuff, int n);
extern "C" WINAPI __declspec(dllexport) DWORD SioTest(HANDLE ComPort);
extern "C" WINAPI __declspec(dllexport) BOOL SioClrInbuf(HANDLE ComPort);
extern "C" WINAPI __declspec(dllexport) BOOL SioChangeBaud(HANDLE ComPort, unsigned int baudrate);
extern "C" WINAPI __declspec(dllexport) BOOL SioClose(HANDLE ComPort);







