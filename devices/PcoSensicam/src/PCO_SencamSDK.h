#include "..\..\include\sencam_def.h"
#include "Csendialog.h"

#include <tchar.h>

class PCOErrorMsg
{
public:
    PCOErrorMsg(){}

    void ShowError(_TCHAR* msg);
    void ShowPCOError(_TCHAR* func, DWORD err);
    void ShowArgumentOutOfRange(_TCHAR* par,int val);
};


class PCOBuffer
{

protected:
  PCOErrorMsg errmsg;
  HANDLE cameraHandle;
  HANDLE picev;
  int bufwidth,bufheight;
  int bit_pix;
  int bufnr;
  void *buf;

public:
  PCOBuffer()
  {
   cameraHandle=NULL;
   picev=NULL;
   bufwidth=bufheight=0;
   bit_pix=0;
   bufnr = -1;
   buf=NULL;
  };

  PCOBuffer(HANDLE HCam);

  void SetCameraHandle(HANDLE HCam);
  int Width(){return bufwidth;};
  int Height(){return bufheight;};
  int Size(){return bufheight*bufwidth*((bit_pix+7)/8);};
  int Bufnr(){return bufnr;};
  HANDLE Picevent(){return picev;};
  void* bufadr(){return buf;};

  DWORD Allocate();
  DWORD Free();
  DWORD Add();
  DWORD Remove();
  DWORD Status();
  DWORD Get_MinMax(int *min,int *max);
  DWORD SimpleConvert_24(void* outbuf,int pitch);
  DWORD SimpleConvert_32(void* outbuf,int pitch);

};

class PCOCamera
{
    friend PCOBuffer;

public:
    PCOErrorMsg errmsg;

protected:
    HINSTANCE camLib ;
    HANDLE cameraHandle;
    int boardnum;

    int mode, submode,trig, hbin, vbin, bit_pix;
    int roixmin,roixmax,roiymin,roiymax;
    int ima_timeout;
    char timetab[500];

    cam_param campar;
    char libname[10];


public:
    PCOCamera();
    ~PCOCamera();

    char* GetLibname(){return libname;};
    void SetDefaultValues();
    DWORD Open(int number);
    DWORD Close();
    //DWORD GetDescription(CString *desc);
    DWORD GetDescription(std::string *desc);
    DWORD Start();
    DWORD Stop();
    DWORD GetCameraSettings();
    DWORD GetCameraSettings(int *imode,int* itrig,int *iroixmin, int *iroixmax,
                            int *iroiymin, int *iroiymax,int *ihbin,int *ivbin,char *itab,int len);
    DWORD ArmCamera();
    DWORD SendTrigger();
    DWORD CancelBuffers();
    DWORD GetImage(PCOBuffer *buf);
    DWORD GetImage(PCOBuffer *buf,int timeout);
    void SetImageTimeout(int timeout){ima_timeout=timeout;};
    bool IsRunning();
    bool IsColor();
    int CCDSize_X();
    int CCDSize_Y();
    HANDLE GetHandle(){return cameraHandle;};
    int BoardNum(){return boardnum;};
    void Mode(int value);
    int Mode();
    void TriggerMode(int value);
    int TriggerMode();
    void Exptime(int value);
    int Exptime();
    void Exptime_ms(int value);
    int Exptime_ms();
    void HorizontalBinning(int value);
    int HorizontalBinning();
    void VerticalBinning(int value);
    int VerticalBinning();
    void SetROI(int xmin,int xmax,int ymin,int ymax);
    void Timevalues(char* table);
};

/*
class PCODialog
{
protected:
  PCOErrorMsg errmsg;
  PCOCamera *camera;
  CWnd* parent;
  HINSTANCE DialogLib;
  int b;
  HWND hmain;

public:
  ~PCODialog(void);
  PCODialog()
  {
   camera=NULL;
   parent=NULL;
   DialogLib=NULL;
  };

  PCODialog(CWnd* pParent,PCOCamera *cam)
  {
   parent=pParent;
   camera=cam;
   DialogLib=NULL;
  }

  BOOL check_modal(){return TRUE;};
  BOOL check_modeless(){return TRUE;};
  BOOL IsClosing(int val);

  void Unload();
  DWORD open();
  DWORD open(BOOL modal);
  DWORD open(BOOL modal,BOOL last_set);
  DWORD close();
  DWORD lock();
  DWORD unlock();
  DWORD getdia_and_setcam(BOOL test);
};
*/
