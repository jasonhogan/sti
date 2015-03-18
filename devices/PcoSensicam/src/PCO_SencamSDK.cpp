#include "stdafx.h"
#include <iostream>

//namespace PCO_SENCAMSDK
//{

//#include <intsafe.h>

// #include <WinDef.h>
//#include <windows.h> // HINSTANCE
#define PCO_SC_CREATE_NAME_OBJECT
#include "..\..\include\sencam_def.h"
#undef PCO_SC_CREATE_NAME_OBJECT
#include "camlib.h"
#include "PCO_SencamSDK.h"

PCOCamera::PCOCamera()
{
  cameraHandle=NULL;
  camLib=NULL;
}

PCOCamera::~PCOCamera()
{
  if(cameraHandle)
   Close();

  if(camLib)
   freesencamlib(&camLib);
}


DWORD PCOCamera::Open(int number)
{
  DWORD err =0;
  HANDLE camHandle=NULL;

  if(getsencamfunc(&camLib)==TRUE)
  {
   errmsg.ShowError(_T("Cannot load library sencam"));
   return PCO_ERROR_APPLICATION_DLLNOTFOUND;
  }

  strcpy_s(libname,sizeof(libname),CAMLIB_NAME);

  err = initboard(number,&camHandle);
  errmsg.ShowPCOError(_T("initboard"), err);
  if(err == PCO_NOERROR)
  {
   boardnum=number;
   cameraHandle=camHandle;
  }
  err = setup_camera(camHandle);
  errmsg.ShowPCOError(_T("setup_camera"), err);
  if(err != PCO_NOERROR)
   Close();
  else
   err=get_cam_param(camHandle,&campar);
  if(err == PCO_NOERROR)
   SetDefaultValues();
  return err;
}

DWORD PCOCamera::Close()
{
  DWORD err=PCO_NOERROR;
  if(cameraHandle)
  {
   err = closeboard(&cameraHandle);
   errmsg.ShowPCOError(_T("closeboard"), err);
   if(err==PCO_NOERROR)
    cameraHandle = NULL;
  }
  return err; 
}

void PCOCamera::SetDefaultValues()
{
//camera parameters
   int exptime,delay;

   delay=0;
   switch(campar.cam_typ)
   {
    case FASTEXP:
     mode=M_FAST;
     submode=NORMALFAST;
     trig=0;
     exptime=5000000;
     sprintf_s(timetab,sizeof(timetab),"%d,%d,-1,-1",delay,exptime); 
     break;

    case LONGEXP:
    case OEM:
    case LONGEXPQE:
    case FASTEXPQE:
     mode=M_LONG;
     submode=NORMALLONG; //VIDEO; //NORMALLONG;
     trig=0;
     exptime=5;
     sprintf_s(timetab,sizeof(timetab),"%d,%d,-1,-1",delay,exptime); 
    break;

    case DICAM: 
    {
     int decay,mcpgain;
     mode=M_DICAM;
     submode=DPSINGLE;
     trig=0;
     exptime=5;
     decay=2;
     mcpgain=990;
     sprintf_s(timetab,sizeof(timetab),"%d,%d,0,1,0,%d,%d,0,-1,-1",decay,mcpgain,delay,exptime);
     break;
    }

    default:
     mode=M_LONG;
     submode=NORMALLONG; //VIDEO; //NORMALLONG;
     trig=0;
     delay=0;
     exptime=5;
     sprintf_s(timetab,sizeof(timetab),"%d,%d,-1,-1",delay,exptime); 
     break;
   }

   switch(campar.cam_ccd)
   {
    case CCD74:
    case CCD74C:
     roixmin=roiymin=1;
     roixmax=20;
     roiymax=15;
     hbin=vbin=1;
     break;

    case CCD85:
    case CCD85C:
     roixmin=roiymin=1;
     roixmax=40;
     roiymax=32;
     hbin=vbin=1;
     break;

    case CCD285QE:
    case CCD285QEF:
    case CCD285QED:
     roixmin=roiymin=1;
     roixmax=43;
     roiymax=33;
     hbin=vbin=1;
     break;

    case CCDTIEM285:
    case CCDTIUV285:
    case CCDTIUG285:
     roixmin=roiymin=1;
     roixmax=32;
     roiymax=31;
     hbin=vbin=1;
     break;

    default:
     roixmin=roiymin=1;
     roixmax=20;
     roiymax=15;
     hbin=vbin=1;
     break;
   }
   bit_pix=12;
   ima_timeout=1000;
}


DWORD PCOCamera::GetDescription(std::string *desc)
{
  DWORD err;
 /* SC_Camera_Description strCamDesc;

  memset(&strCamDesc,0,sizeof(SC_Camera_Description));
  strCamDesc.wSize=sizeof(SC_Camera_Description);
  err=get_camera_desc(cameraHandle,&strCamDesc);
  if(err!=PCO_NOERROR)
  {
   errmsg.ShowPCOError("get_camera_desc", err);
   return err;
  }
  
  desc->assign("Camera:\n");
  
  //desc->SetString(_T("Camera:\n"));
  if(err==PCO_NOERROR)
  {
   desc->Append(_T("Type "));
   desc->AppendFormat(_T("0x%04x : %s\n"),campar.cam_typ,CAMTYPE_NAMES[strCamDesc.wCameraTypeDESC]);
   desc->Append(_T("CCDType "));
   desc->AppendFormat(_T("0x%04x : %s\n"),strCamDesc.wSensorTypeDESC,CCDTYPE_NAMES[strCamDesc.wSensorTypeDESC]);
  }

  if(err==0)
  {
   if(strCamDesc.wDoubleImageDESC==1)
   {
     desc->Append(_T("Feature: Double Shutter\n"));
   }
  }

  if(err==0)
  {
   desc->AppendFormat(_T("Resolution: %04d x %04d\n"),strCamDesc.wMaxHorzResDESC,strCamDesc.wMaxVertResDESC);
  }*/
  return err;
}

DWORD PCOCamera::Start()
{
  DWORD err = run_coc(cameraHandle,0);
  errmsg.ShowPCOError("run_coc", err);
  return err;
}

DWORD PCOCamera::Stop()
{
  DWORD err = stop_coc(cameraHandle,0);
  errmsg.ShowPCOError("stop_coc", err);
  return err;
}

DWORD PCOCamera::GetCameraSettings()
{
  DWORD err= PCO_NOERROR;
  int m;
  err=get_coc(cameraHandle,&m,&trig,&roixmin,&roixmax,&roiymin,&roiymax,&hbin,&vbin, timetab,sizeof(timetab));
  errmsg.ShowPCOError("get_coc", err);
  mode=m&0xFFFF;
  submode=(m>>16);

  return err;
}

DWORD PCOCamera::GetCameraSettings(int *imode,int* itrig,int *iroixmin, int *iroixmax,
                        int *iroiymin, int *iroiymax,int *ihbin,int *ivbin,char *itab,int len)
{
  DWORD err= PCO_NOERROR;
  int m;
  err=get_coc(cameraHandle,&m,&trig,&roixmin,&roixmax,&roiymin,&roiymax,&hbin,&vbin, timetab,sizeof(timetab));
  mode=m&0xFFFF;
  submode=(m>>16);
  *imode=m;
  *itrig=trig;
  *iroixmin=roixmin;
  *iroixmax=roixmax;
  *iroiymin=roiymin;
  *iroiymax=roiymax;
  *ihbin=hbin;
  *ivbin=vbin;
  memset(itab,0,len);
  len=min(len,sizeof(timetab));
  memcpy(itab,timetab,len);
  return err;
}

DWORD PCOCamera::ArmCamera()
{
  DWORD err=PCO_NOERROR;
  int len,m;
  len=sizeof(timetab);
  m=mode|(submode<<16);
  err=test_coc(cameraHandle,&m,&trig,&roixmin,&roixmax,&roiymin,&roiymax,
               &hbin,&vbin,timetab,&len);

  if(err!=PCO_NOERROR)
  {
   if((err&0xF000FFFF)==PCO_WARNING_SDKDLL_COC_VALCHANGE)
   {
   }
   else if((err&0xF000FFFF)==PCO_WARNING_SDKDLL_COC_STR_SHORT)
   {
    return PCO_ERROR_APPLICATION|PCO_ERROR_WRONGVALUE;
   }
   else
   {
    return err;
   }
  }

//set camera values
  err=set_coc(cameraHandle,m,trig,roixmin,roixmax,roiymin,roiymax,
               hbin,vbin,timetab);
  errmsg.ShowPCOError("SetMode", err);
  return err;
}

DWORD PCOCamera::SendTrigger()
{
  DWORD err=PCO_NOERROR;
  return err;
}

DWORD PCOCamera::CancelBuffers()
{
  DWORD err = RemoveAllBuffers(cameraHandle);
  errmsg.ShowPCOError("RemoveAllBuffers", err);
  return err;
}

DWORD PCOCamera::GetImage(PCOBuffer *buf)
{
  return GetImage(buf,ima_timeout+Exptime_ms());
}

DWORD PCOCamera::GetImage(PCOBuffer *buf,int timeout)
{
  DWORD err = PCO_NOERROR;
  if (!IsRunning())
   return PCO_ERROR_APPLICATION|(PCO_ERROR_SDKDLL_RECORDINGMUSTBEON&~PCO_ERROR_LAYER_MASK);
  HANDLE picev = buf->Picevent();
  int Status;

  err = buf->Add();
  if(err==PCO_NOERROR)
  {
   err = SendTrigger();

   WaitForSingleObject(picev,timeout);
   Status=buf->Status();
   if((Status & 0x0F000) != 0)
    return PCO_ERROR_APPLICATION;
   if ((Status & 0x0002) == 0)
    return PCO_ERROR_APPLICATION_PICTURETIMEOUT;
  }
  return err;
}

bool PCOCamera::IsRunning()
{
  DWORD err = PCO_NOERROR;
  struct cam_values camval;
  camval.runcoc=CAM_STOPPED;
  err=get_cam_values(cameraHandle,&camval);
  if(camval.runcoc==CAM_STOPPED)
   return false;
  else
   return true;
}

bool PCOCamera::IsColor()
{
  if((campar.cam_ccd&0x01)==0x01)
   return false;
  else
   return true;
}

int PCOCamera::CCDSize_X()
{
  return campar.ccdwidth;
}

int PCOCamera::CCDSize_Y()
{
  return campar.ccdheight;
}

void PCOCamera::Mode(int value)
{
  switch(campar.cam_typ)
  {
   case FASTEXP:
    if((value&0xFFFF)!=M_FAST)
    {
     errmsg.ShowArgumentOutOfRange(_T("Mode"),value);
     return;
    }
    if((value>>16)<NORMALFAST)
    {
     errmsg.ShowArgumentOutOfRange(_T("Submode"),value);
     return;
    }
    break;

   case LONGEXP:
   case OEM:
   case LONGEXPQE:
   case FASTEXPQE:
    if((value&0xFF)!=M_LONG)
    {
     errmsg.ShowArgumentOutOfRange(_T("Mode"),value);
     return;
    }
    if((value>>16)<NORMALLONG)
    {
     errmsg.ShowArgumentOutOfRange(_T("Submode"),value);
     return;
    }
   break;

   case DICAM: 
    if((value&0xFFFF)!=M_DICAM)
    {
     errmsg.ShowArgumentOutOfRange(_T("Mode"),value);
     return;
    }
    if(((value>>16)<DPSINGLE)||((value>>16)>DPDOUBLE))
    {
     errmsg.ShowArgumentOutOfRange(_T("Submode"),value);
     return;
    }
   break;
  }
  mode=value&0xFFFF;
  submode=(value>>16);
}

int PCOCamera::Mode(void)
{
  return mode;
}

void PCOCamera::TriggerMode(int value)
{
  if(((value&0xF)<0)||((value&0xF)>2))
  {
   errmsg.ShowArgumentOutOfRange(_T("Trigger"),value);
   return;
  }
  trig = value;
}

int PCOCamera::TriggerMode(void)
{
  return trig;
}

//cannot handle special cases
void PCOCamera::Exptime(int value)
{
  int delay,exptime,num;
  int decay,mcpgain;

  switch(campar.cam_typ)
  {
   case FASTEXP:
   case LONGEXP:
   case OEM:
   case LONGEXPQE:
   case FASTEXPQE:
    num=sscanf_s(timetab,"%d,%d,-1,-1",&delay,&exptime);
    if(num==2)
    {
     exptime=value;
     sprintf_s(timetab,sizeof(timetab),"%d,%d,-1,-1",delay,exptime);
    }
   break;

   case DICAM: 
    decay=2;
    mcpgain=990;
    num=sscanf_s(timetab,"%d,%d,0,1,0,%d,%d,0,-1,-1",&decay,&mcpgain,&delay,&exptime);
    if(num==4)
    {
     exptime=value;
     sprintf_s(timetab,sizeof(timetab),"%d,%d,0,1,0,%d,%d,0,-1,-1",decay,mcpgain,delay,exptime);
    }
   break;
  }
}

int PCOCamera::Exptime()
{
  int delay,exptime,num;
  int decay,mcpgain;

  switch(campar.cam_typ)
  {
   case FASTEXP:
   case LONGEXP:
   case OEM:
   case LONGEXPQE:
   case FASTEXPQE:
    num=sscanf_s(timetab,"%d,%d,-1,-1",&delay,&exptime);
    if(num==2)
    {
     return exptime;
    }
   break;

   case DICAM: 
    decay=2;
    mcpgain=990;
    num=sscanf_s(timetab,"%d,%d,0,1,0,%d,%d,0,-1,-1",&decay,&mcpgain,&delay,&exptime);
    if(num==4)
    {
     return exptime;
    }
   break;
  }
  return -1;
}

void PCOCamera::Exptime_ms(int value)
{
  int delay,exptime,num;
  int decay,mcpgain;

  switch(campar.cam_typ)
  {
   case FASTEXP:
    value*=1000000;
   case LONGEXP:
   case OEM:
   case LONGEXPQE:
   case FASTEXPQE:
    num=sscanf_s(timetab,"%d,%d,-1,-1",&delay,&exptime);
    if(num==2)
    {
     exptime=value;
     sprintf_s(timetab,sizeof(timetab),"%d,%d,-1,-1",delay,exptime);
    }
   break;

   case DICAM: 
    decay=2;
    mcpgain=990;
    num=sscanf_s(timetab,"%d,%d,0,1,0,%d,%d,0,-1,-1",&decay,&mcpgain,&delay,&exptime);
    if(num==4)
    {
     exptime=value;
     sprintf_s(timetab,sizeof(timetab),"%d,%d,0,1,0,%d,%d,0,-1,-1",decay,mcpgain,delay,exptime);
    }
   break;
  }
}

int PCOCamera::Exptime_ms()
{
  int delay,exptime,num;
  int decay,mcpgain;

  switch(campar.cam_typ)
  {
   case FASTEXP:
   case LONGEXP:
   case OEM:
   case LONGEXPQE:
   case FASTEXPQE:
    num=sscanf_s(timetab,"%d,%d,-1,-1",&delay,&exptime);
    if(num==2)
    {
     if(campar.cam_typ==FASTEXP)
      return exptime/1000000;
     else
      return exptime;
    }
   break;

   case DICAM: 
    decay=2;
    mcpgain=990;
    num=sscanf_s(timetab,"%d,%d,0,1,0,%d,%d,0,-1,-1",&decay,&mcpgain,&delay,&exptime);
    if(num==4)
    {
     return exptime;
    }
   break;
  }
  return -1;
}

void PCOCamera::HorizontalBinning(int value)
{
  if(value<=8)
   hbin=value;
  else 
   errmsg.ShowArgumentOutOfRange(_T("HorizontalBinning"),value);
}

int PCOCamera::HorizontalBinning()
{
  return hbin;
}

void PCOCamera::VerticalBinning(int value)
{
  if(value<=32)
   vbin = value;
  else
   errmsg.ShowArgumentOutOfRange(_T("VerticalBinning"),value);
}

int PCOCamera::VerticalBinning()
{
  return vbin;
}


void PCOCamera::SetROI(int xmin,int xmax,int ymin,int ymax)
{
  if(xmin<1)
  {
   errmsg.ShowArgumentOutOfRange(_T("ROI xmin"),xmin);
   return;
  }
  if(ymin<1)
  {
   errmsg.ShowArgumentOutOfRange(_T("ROI ymin"),ymin);
   return;
  }

  if(xmin>xmax)
  {
   errmsg.ShowArgumentOutOfRange(_T("ROI xmin>xmax"),xmin);
   return;
  }

  if(ymin>ymax)
  {
   errmsg.ShowArgumentOutOfRange(_T("ROI ymin>ymax"),ymin);
   return;
  }

  int max;

  max=campar.ccdwidth/32;
  if(campar.ccdwidth%32)
   max++;

  if(xmax>max)
  {
   errmsg.ShowArgumentOutOfRange(_T("ROI xmax>ccdwidth"),xmax);
   return;
  }
  max=campar.ccdheight/32;
  if(campar.ccdheight%32)
   max++;
  if(ymax>max)
  {
   errmsg.ShowArgumentOutOfRange(_T("ROI ymax>ccdheight"),ymax);
   return;
  }
  roixmin=xmin;
  roixmax=xmax;
  roiymin=ymin;
  roiymax=ymax;
}

void PCOCamera::Timevalues(char* table)
{
  if(strlen(table)>sizeof(timetab))
  {
   errmsg.ShowArgumentOutOfRange(_T("Timetable size"),(int)strlen(table));
   return;
  }
  strcpy_s(timetab,sizeof(timetab),table);
}




//BufferClass
PCOBuffer::PCOBuffer(HANDLE HCam)
{
  picev=NULL;
  bufwidth=bufheight=0;
  bit_pix=0;
  bufnr = -1;
  buf=NULL;
  cameraHandle = HCam;
}

void PCOBuffer::SetCameraHandle(HANDLE HCam)
{
  cameraHandle = HCam;
}


DWORD PCOBuffer::Allocate()
{
  DWORD err = 0;
  int ccdxsize = 0;
  int ccdysize = 0;
  int width = 0;
  int height = 0;

  err = GetSizes(cameraHandle,&ccdxsize,&ccdysize,&width,&height,&bit_pix);
  if (err != PCO_NOERROR)
  {
   errmsg.ShowPCOError("GetSizes",err);
   return err;
  }

//check if buffer was already allocated
  if (bufnr > -1)
  {
//if buffer has different size allocate new one else use old
   if ((bufwidth != width) || (bufheight != height))
   {
    err = Free();
    if (err != PCO_NOERROR)
     return err;
   }
   else
    return 0;
  }

  int size = width * height * 2;

  picev = NULL;
  err = AllocateBufferEx(cameraHandle,&bufnr,size,&picev,&buf);
  errmsg.ShowPCOError("AllocateBufferEx",err);
  if(err == PCO_NOERROR)
  {
   bufwidth=width;
   bufheight=height;
  }

  return err;
}

DWORD PCOBuffer::Free()
{
  DWORD err = PCO_NOERROR;
  if ((buf != NULL) && (bufnr > -1))
  {
   err = FreeBuffer(cameraHandle, bufnr);
   errmsg.ShowPCOError("FreeBuffer",err);
   if (err == PCO_NOERROR)
   {
    bufwidth = bufheight = 0;
    buf = NULL;
    bufnr = -1;
    picev=NULL;
   }
  }
  return err;
}


DWORD PCOBuffer::Add()
{
  DWORD err = 0;
  int size = bufwidth * bufheight * 2;
  err = AddBuffer(cameraHandle, bufnr, size, 0, 0);
  errmsg.ShowPCOError("AddBuffer", err);
  return err;
}

DWORD PCOBuffer::Remove()
{
  DWORD err = 0;
  err = RemoveBuffer(cameraHandle, bufnr);
  errmsg.ShowPCOError("RemoveBuffer", err);
  return err;
}

DWORD PCOBuffer::Status()
{
  DWORD err=0; 
  int stat=0;
  err = GetBufferStatus(cameraHandle, bufnr, 0, &stat, 4);
  errmsg.ShowPCOError("GetBufferStatus", err);
  return stat;
}

DWORD PCOBuffer::Get_MinMax(int *min,int *max)
{
  DWORD err = 0;
  *max = 2000;
  *min = 100;
  if (buf != NULL)
  {
   *max = 0;
   *min = 65535;
   WORD* pw = (WORD*)buf;

   for (int i = 0; i < bufwidth * bufheight; i++)
   {
    if (*pw > *max)
     *max = *pw;
    if (*pw < *min)
     *min = *pw;
    pw++;
   }
  }
  if(*max<1)
   *max=1;
  if(*min >= *max)
   *min = *max - 1;
  return err;
}

DWORD PCOBuffer::SimpleConvert_24(void* outbuf,int pitch)
{
  DWORD err = 0;
  if(buf)
  {
   WORD* pw = (WORD*)buf;
   BYTE* po;
   int shift=16-bit_pix;

   for (int y = 0; y < bufheight; y++)
   {
    po=(BYTE*)outbuf;
    po+=y*pitch; 
    for(int x=0;x<bufwidth;x++)
    { 
     *po++=(BYTE)(*pw>>shift);
     *po++=(BYTE)(*pw>>shift);
     *po++=(BYTE)(*pw>>shift);
     pw++;
    }
   }
  }
  return err;
}

DWORD PCOBuffer::SimpleConvert_32(void* outbuf,int pitch)
{
  DWORD err = 0;
  if(buf)
  {
   WORD* pw = (WORD*)buf;
   BYTE* po;
   int shift=16-bit_pix;

   for (int y = 0; y < bufheight; y++)
   {
    po=(BYTE*)outbuf;
    po+=y*pitch; 
    for(int x=0;x<bufwidth;x++)
    { 
     *po++=(BYTE)(*pw>>shift);
     *po++=(BYTE)(*pw>>shift);
     *po++=(BYTE)(*pw>>shift);
     *po++=0;
     pw++;
    }
   }
  }
  return err;
}

/*
PCODialog::~PCODialog(void)
{
  if(DialogLib)
  {
   end_dialog_cam(b);
   freedialoglib(&DialogLib);
  }
}

DWORD PCODialog::open()
{
  return open(TRUE,FALSE);
}

DWORD PCODialog::open(BOOL modal)
{
  return open(modal,FALSE);
}

DWORD PCODialog::open(BOOL modal,BOOL last_set)
{
  char dialogname[100];
  DWORD err=PCO_NOERROR;

  if(camera==NULL)
  {
   err= PCO_ERROR_APPLICATION|PCO_ERROR_INVALIDHANDLE;
   errmsg.ShowPCOError(_T("PCODialog::open"),err);
   return err;
  }

  err=get_dialog_dllname(camera->GetHandle(),dialogname,sizeof(dialogname));
  if(err!=PCO_NOERROR)
  {
   errmsg.ShowPCOError(_T("get_dialog_dllname"),err);
   return err;
  }

  if(DialogLib==NULL)
  {
   if(getdialoglib(&DialogLib,dialogname))
   {
    err=PCO_ERROR_APPLICATION | PCO_ERROR_APPLICATION_DLLNOTFOUND;
    errmsg.ShowPCOError(_T("getdialoglib"),err);
    return err;
   }
  }

  err=init_dialog_cam_ext(camera->BoardNum(),NULL);
  if(err==PCO_NOERROR)
  {
   b=camera->BoardNum();
   hmain=parent->GetSafeHwnd();
  }
  else
  {
   freedialoglib(&DialogLib); 
   errmsg.ShowPCOError(_T("init_dialog_cam_ext"),err);
   return err;
  }

  if(!last_set)
  {
   char timetab[500];
   int mode,trig,ca,cb,cc,cd,hbin,vbin;
   camera->GetCameraSettings(&mode,&trig,&ca,&cb,&cc,&cd,&hbin,&vbin,timetab,sizeof(timetab));
   set_dialog_cam(b,mode,trig,ca,cb,cc,cd,hbin,vbin,timetab);
  }

  if(modal)
  {
   int ret;
   ret=open_dialog_cam(b,hmain,4,"CppDemo Modal Dialog");
   if((ret&0xFF)==1)
   {
    err=getdia_and_setcam(FALSE);
    if(ret&0x100)
     err=open_dialog_cam(b,hmain,((MSG_CAM_DIALOG)<<16)+3,"CppDemo Time Dialog");
   }
  }
  else
   err=open_dialog_cam(b,hmain,((MSG_CAM_DIALOG)<<16)+1,"CppDemo Dialog");

  return err;
}

DWORD PCODialog::close()
{
  DWORD err;
  if(DialogLib)
   err= close_dialog_cam(b);
  else
   err= PCO_ERROR_APPLICATION |PCO_ERROR_APPLICATION_DLLNOTFOUND;
  return err;
}

void PCODialog::Unload()
{
  close();
}


BOOL PCODialog::IsClosing(int val)
{
  if(val==2)
   return TRUE;
  else
   return FALSE;
}


DWORD PCODialog::lock()
{
  DWORD err;
  if(DialogLib)
   err=lock_dialog_cam(b,1);
  else
   err= PCO_ERROR_APPLICATION |PCO_ERROR_APPLICATION_DLLNOTFOUND;
  return err;
}

DWORD PCODialog::unlock()
{
  DWORD err;
  if(DialogLib)
   err=lock_dialog_cam(b,0);
  else
   err= PCO_ERROR_APPLICATION |PCO_ERROR_APPLICATION_DLLNOTFOUND;
  return err;
}


DWORD PCODialog::getdia_and_setcam(BOOL test)
{
  DWORD err=PCO_NOERROR;
  int mode,trig,ca,cb,cc,cd,hbin,vbin;
  char tab[500];
  int hwnd,status;

  if(test)
   err=status_dialog_cam(b,&hwnd,&status);
  else
   status=1;
  if((err==PCO_NOERROR)&&(status!=0))
  {
   err=get_dialog_cam(b,&mode,&trig,&ca,&cb,&cc,&cd,
                    &hbin,&vbin,tab,500);
   if(err==PCO_NOERROR)
   {
    camera->Mode(mode);
    camera->TriggerMode(trig);
    camera->SetROI(ca,cb,cc,cd);
    camera->HorizontalBinning(hbin);
    camera->VerticalBinning(vbin);
    camera->Timevalues(tab);
    camera->ArmCamera();
   }
  }
  return err;
}


*/

void PCOErrorMsg::ShowError(_TCHAR* msg)
{
    MessageBox(NULL,msg,_T("CppDemo ERROR information"),MB_OK|MB_APPLMODAL|MB_ICONEXCLAMATION);
}

void PCOErrorMsg::ShowPCOError(_TCHAR* func, DWORD err)
{
  if (err != PCO_NOERROR)
  {
    _TCHAR msg[200];
    sprintf_s(msg,sizeof(msg),"Call to %s() failed with errorcode 0x%x",func,err);
    MessageBox(NULL,msg,_T("CppDemo ERROR information"),MB_OK|MB_APPLMODAL|MB_ICONEXCLAMATION);
  }
}

void PCOErrorMsg::ShowArgumentOutOfRange(_TCHAR* par,int val)
{
  _TCHAR msg[200];
  sprintf_s(msg,sizeof(msg),"Parameter %s: Value %d (0x%x) is out of valid range",par,val);
  MessageBox(NULL,msg,_T("CppDemo ERROR information"),MB_OK|MB_APPLMODAL|MB_ICONEXCLAMATION);
}

//}