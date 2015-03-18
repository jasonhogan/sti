//-----------------------------------------------------------------//
// Name        | camlib.cpp                  | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | SensiCam                    |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | WINDOWS XP/W7                                     //
//-----------------------------------------------------------------//
// Environment | Microsoft VisualStudio                            //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | explizit linking of DLL sen_cam.dll               //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 0.01 rel. 0.00                               //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//             |                                                   //
//             |                                                   // 
//-----------------------------------------------------------------//
// (c) 2014 PCO AG * Donaupark 11 *                                //
// D-93309      Kelheim / Germany * Phone: +49 (0)9441 / 2005-0 *  //
// Fax: +49 (0)9441 / 2005-20 * Email: info@pco.de                 //
//-----------------------------------------------------------------//

#include "stdafx.h"

#define PCO_SENCAMLIB_H_CREATE_OBJECT
#include "camlib.h"
#undef PCO_SENCAMLIB_H_CREATE_OBJECT

void (*msg)(PSTR sz,...)=NULL;

void Enable_Messages(int level,void (*func)(PSTR sz,...))
{
 if(func)
  msg=func;
}

//get functions out of sen_cam.dll
//not for all functions of the library

int getsencamfunc(HINSTANCE *Lib)
{
  HINSTANCE sencamLib;
  int err=FALSE;

  if((sencamLib=LoadLibrary(CAMLIB_NAME))==NULL)
  {
   if(msg)
    msg("Cannot load Library %s.DLL\n",CAMLIB_NAME);
   return TRUE;
  }

  initboard=(int(WINAPI *)(int,HANDLE*))
               GetProcAddress(sencamLib,"INITBOARD");
  if(initboard==NULL)
  {
   if(msg)
    msg("Cannot get function INITBOARD\n");
   err=TRUE;
  }

  closeboard=(int(WINAPI *)(HANDLE*))
               GetProcAddress(sencamLib,"CLOSEBOARD");
  if(closeboard==NULL)
  {
   if(msg)
    msg("Cannot get function CLOSEBOARD\n");
   err=TRUE;
  }

  setup_camera=(int (WINAPI *)(HANDLE hdriver))
               GetProcAddress(sencamLib,"SETUP_CAMERA");
  if(setup_camera==NULL)
  {
   if(msg)
    msg("Cannot get function SETUP_CAMERA\n");
   err=TRUE;
  }

  run_coc=(int (WINAPI *)(HANDLE hdriver,int mode))
               GetProcAddress(sencamLib,"RUN_COC");
  if(run_coc==NULL)
  {
   if(msg)
    msg("Cannot get function RUN_COC\n");
   err=TRUE;
  }

  stop_coc=(int (WINAPI *)(HANDLE hdriver,int mode))
               GetProcAddress(sencamLib,"STOP_COC");
  if(stop_coc==NULL)
  {
   if(msg)
    msg("Cannot get function STOP_COC\n");
   err=TRUE;
  }

  set_coc=(int (WINAPI *)(HANDLE hdriver,int mode,int trig,int roixmin, int roixmax,
                        int roiymin, int roiymax,int hbin,int vbin,char *timevalues))
               GetProcAddress(sencamLib,"SET_COC");
  if(set_coc==NULL)
  {
   if(msg)
    msg("Cannot get function SET_COC\n");
   err=TRUE;
  }


  test_coc=(int (WINAPI *)(HANDLE hdriver,int *mode,int *trig,int *roixmin, int *roixmax,
                        int *roiymin, int *roiymax,int *hbin,int *vbin,
                        char *tab,int *tablength))
               GetProcAddress(sencamLib,"TEST_COC");
  if(test_coc==NULL)
  {
   if(msg)
    msg("Cannot get function TEST_COC\n");
   err=TRUE;
  }

  get_coc=(int (WINAPI *)(HANDLE hdriver,int *mode,int *trig,int *roixmin, int *roixmax,
                        int *roiymin, int *roiymax,int *hbin,int *vbin,
                        char *tab,int len))
               GetProcAddress(sencamLib,"GET_COC_SETTING");
  if(get_coc==NULL)
  {
   if(msg)
    msg("Cannot get function GET_COC_SETTING\n");
   err=TRUE;
  }

  GetSizes=(int(WINAPI *)(HANDLE,int*,int*,int*,int*,int*))
               GetProcAddress(sencamLib,"GETSIZES");
  if(GetSizes==NULL)
  {
   if(msg)
    msg("Cannot get function GETSIZES");
   err=TRUE;
  }

  get_cam_param=(int (WINAPI *)(HANDLE hdriver, struct cam_param *param))
               GetProcAddress(sencamLib,"GET_CAM_PARAM");
  if(get_cam_param==NULL)
  {
   if(msg)
    msg("Cannot get function GET_CAM_PARAM");
   err=TRUE;
  }

  get_cam_values=(int (WINAPI *)(HANDLE hdriver, struct cam_values *val))
               GetProcAddress(sencamLib,"GET_CAM_VALUES");
  if(get_cam_values==NULL)
  {
   if(msg)
    msg("Cannot get function GET_CAM_VALUES");
   err=TRUE;
  }

  get_cam_settings=(int (WINAPI *)(HANDLE hdriver, struct cam_settings *set))
               GetProcAddress(sencamLib,"GET_CAM_SETTINGS");
  if(get_cam_settings==NULL)
  {
   if(msg)
    msg("Cannot get function GET_CAM_SETIINGS");
   err=TRUE;
  }

  get_status=(int (WINAPI *)(HANDLE hdriver,int *camtype,int *eletemp,int *ccdtemp))
               GetProcAddress(sencamLib,"GET_STATUS");
  if(get_status==NULL)
  {
   if(msg)
    msg("Cannot get function GET_STATUS");
   err=TRUE;
  }

  clear_board_buffer=(int (WINAPI *)(HANDLE hdriver))
               GetProcAddress(sencamLib,"CLEAR_BOARD_BUFFER");
  if(clear_board_buffer==NULL)
  {
   if(msg)
    msg("Cannot get function CLEAR_BOARD_BUFFER");
   err=TRUE;
  }

  read_image_12bit=(int (WINAPI *)(HANDLE hdriver,int mode,int width,int height,unsigned short *b12))
               GetProcAddress(sencamLib,"READ_IMAGE_12BIT");
  if(read_image_12bit==NULL)
  {
   if(msg)
    msg("Cannot get function READ_IMAGE_12BIT");
   err=TRUE;
  }


  wait_for_image=(int (WINAPI *)(HANDLE hdriver,int timeout))
               GetProcAddress(sencamLib,"WAIT_FOR_IMAGE");
  if(wait_for_image==NULL)
  {
   if(msg)
    msg("Cannot get function WAIT_FOR_IMAGE");
   err=TRUE;
  }

  get_dialog_dllname=(int (WINAPI *)(HANDLE hdriver,char* name,int len))
               GetProcAddress(sencamLib,"GET_DIALOG_DLLNAME");
  if(get_dialog_dllname==NULL)
  {
   if(msg)
    msg("Cannot get function GET_DIALOG_DLLNAME");
   err=TRUE;
  }

  get_camera_desc=(int (WINAPI *)(HANDLE hdriver,SC_Camera_Description *strCamDesc))
               GetProcAddress(sencamLib,"GET_CAMERA_DESC");
  if(get_camera_desc==NULL)
  {
   if(msg)
    msg("Cannot get function GET_DIALOG_DLLNAME");
   err=TRUE;
  }

  AllocateBufferEx=(int(WINAPI *)(HANDLE hdriver,int *bufnr,int size,HANDLE *hPicEvent,void** adr))
               GetProcAddress(sencamLib,"ALLOCATE_BUFFER_EX");
  if(AllocateBufferEx==NULL)
  {
   if(msg)
    msg("Cannot get function ALLOCATE_BUFFER_EX");
   err=TRUE;
  }

  FreeBuffer=(int(WINAPI *)(HANDLE,int))
               GetProcAddress(sencamLib,"FREE_BUFFER");
  if(FreeBuffer==NULL)
  {
   if(msg)
    msg("Cannot get function FREE_BUFFER");
   err=TRUE;
  }



  GetBufferStatus=(int(WINAPI *)(HANDLE,int,int,int*,int))
               GetProcAddress(sencamLib,"GETBUFFER_STATUS");
  if(GetBufferStatus==NULL)
  {
   if(msg)
    msg("Cannot get function GETBUFFER_STATUS");
   err=TRUE;
  }

  AddBuffer=(int(WINAPI *)(HANDLE,int,int,int,int))
               GetProcAddress(sencamLib,"ADD_BUFFER_TO_LIST");
  if(AddBuffer==NULL)
  {
   if(msg)
    msg("Cannot get function ADD_BUFFER_TO_LIST");
   err=TRUE;
  }

  RemoveBuffer=(int(WINAPI *)(HANDLE,int))
               GetProcAddress(sencamLib,"REMOVE_BUFFER_FROM_LIST");
  if(RemoveBuffer==NULL)
  {
   if(msg)
    msg("Cannot get function REMOVE_BUFFER_FROM_LIST");
   err=TRUE;
  }

  RemoveAllBuffers=(int(WINAPI *)(HANDLE))
               GetProcAddress(sencamLib,"REMOVE_ALL_BUFFERS_FROM_LIST");
  if(RemoveAllBuffers==NULL)
  {
   if(msg)
    msg("Cannot get function REMOVE_BUFFER_FROM_LIST");
   err=TRUE;
  }

/*
  allocate_buffer=(int(WINAPI *)(HANDLE,int*,int*))
               GetProcAddress(sencamLib,"ALLOCATE_BUFFER");
  if(allocate_buffer==NULL)
  {
   if(msg)
    msg("Cannot get function ALLOCATE_BUFFER");
   err=TRUE;
  }


  getbuffer_status=(int(WINAPI *)(HANDLE,int,int,int*,int))
               GetProcAddress(sencamLib,"GETBUFFER_STATUS");
  if(getbuffer_status==NULL)
  {
   if(msg)
    msg("Cannot get function GETBUFFER_STATUS");
   err=TRUE;
  }

  add_buffer_to_list=(int(WINAPI *)(HANDLE,int,int,int,int))
               GetProcAddress(sencamLib,"ADD_BUFFER_TO_LIST");
  if(add_buffer_to_list==NULL)
  {
   if(msg)
    msg("Cannot get function ADD_BUFFER_TO_LIST");
   err=TRUE;
  }

  remove_buffer_from_list=(int(WINAPI *)(HANDLE,int))
               GetProcAddress(sencamLib,"REMOVE_BUFFER_FROM_LIST");
  if(remove_buffer_from_list==NULL)
  {
   if(msg)
    msg("Cannot get function REMOVE_BUFFER_FROM_LIST");
   err=TRUE;
  }

  setbuffer_event=(int(WINAPI *)(HANDLE,int,HANDLE*))
               GetProcAddress(sencamLib,"SETBUFFER_EVENT");
  if(setbuffer_event==NULL)
  {
   if(msg)
    msg("Cannot get function SET_BUFFER_EVENT");
   err=TRUE;
  }

  map_buffer=(int(WINAPI *)(HANDLE,int,int,int,void**))
               GetProcAddress(sencamLib,"MAP_BUFFER");
  if(map_buffer==NULL)
  {
   if(msg)
    msg("Cannot get function MAP_BUFFER");
  }

  unmap_buffer=(int(WINAPI *)(HANDLE,int))
               GetProcAddress(sencamLib,"UNMAP_BUFFER");
  if(unmap_buffer==NULL)
  {
   if(msg)
    msg("Cannot get function UNMAP_BUFFER");
   err=TRUE;
  }


  add_buffer=(int (WINAPI *)(HANDLE hdriver,int size,void *adr,HANDLE hevent,DWORD* Status))
               GetProcAddress(sencamLib,"ADD_BUFFER");
  if(add_buffer==NULL)
  {
   if(msg)
    msg("Cannot get function ADD_BUFFER");
   err=TRUE;
  }

  remove_buffer=(int (WINAPI *)(HANDLE hdriver,void *adr))
               GetProcAddress(sencamLib,"REMOVE_BUFFER");
  if(remove_buffer==NULL)
  {
   if(msg)
    msg("Cannot get function REMOVE_BUFFER");
   err=TRUE;
  }
*/

  if(err)
  {
   FreeLibrary(sencamLib);
   sencamLib=NULL;
  }
  *Lib=sencamLib;

  return err;
}

int freesencamlib(HINSTANCE *sencamLib)
{
  FreeLibrary(*sencamLib);
  *sencamLib=NULL;

  return 0;
}


BOOL getdialoglib(HINSTANCE *senDialogLib,char *name)
{
  BOOL err=FALSE;
  HINSTANCE DialogLib;

  if((DialogLib=LoadLibrary(name))==NULL)
  {
   return TRUE;
  }

  end_dialog_cam=(int (WINAPI *)(int))
               GetProcAddress(DialogLib,"END_DIALOG_CAM");
  if(end_dialog_cam==NULL)
  {
   err=TRUE;
  }

  init_dialog_cam_ext=(int (WINAPI *)(int,HINSTANCE))
               GetProcAddress(DialogLib,"INIT_DIALOG_CAM_EXT");
  if(init_dialog_cam_ext==NULL)
  {
   err=TRUE;
  }

  open_dialog_cam=(int (WINAPI *)(int,HWND,int,char*))
               GetProcAddress(DialogLib,"OPEN_DIALOG_CAM");
  if(open_dialog_cam==NULL)
  {
   err=TRUE;
  }

  lock_dialog_cam=(int (WINAPI *)(int,int))
               GetProcAddress(DialogLib,"LOCK_DIALOG_CAM");
  if(lock_dialog_cam==NULL)
  {
   err=TRUE;
  }

  close_dialog_cam=(int (WINAPI *)(int))
               GetProcAddress(DialogLib,"CLOSE_DIALOG_CAM");
  if(close_dialog_cam==NULL)
  {
   err=TRUE;
  }

  status_dialog_cam=(int (WINAPI *)(int,int*,int*))
               GetProcAddress(DialogLib,"STATUS_DIALOG_CAM");
  if(status_dialog_cam==NULL)
  {
   err=TRUE;
  }

  set_dialog_cam=(int (WINAPI *)(int board,int mode, int trig,
                                int ca, int cb,int cc, int cd,
                                int hbin, int vbin,
                                char *linevalues))
               GetProcAddress(DialogLib,"SET_DIALOG_CAM");
  if(set_dialog_cam==NULL)
  {
   err=TRUE;
  }

  get_dialog_cam=(int (WINAPI *)(int board,int *mode, int *trig,
                                 int *ca, int *cb,int *cc, int *cd,
                                 int *hbin, int *vbin,
                                 char *tab,int len))
               GetProcAddress(DialogLib,"GET_DIALOG_SETTINGS");
  if(get_dialog_cam==NULL)
  {
   err=TRUE;
  }

  if(err)
  {
   FreeLibrary(DialogLib);
   DialogLib=NULL;
  }
  *senDialogLib=DialogLib;

  return err;
}

void freedialoglib(HINSTANCE *DialogLib)
{
  FreeLibrary(*DialogLib);
  *DialogLib=NULL;
  return;
}

