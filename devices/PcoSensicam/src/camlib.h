//-----------------------------------------------------------------//
// Name        | camlib.h                    | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
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

#include "..\..\include\SC_SDKStructures.h"

#define CAMLIB_NAME "sen_cam"

#ifdef PCO_SENCAMLIB_H_CREATE_OBJECT
#define EXT 
#else
#define EXT extern "C"
#endif

#ifdef PCO_SENCAMLIB_H_CREATE_OBJECT
#ifdef __cplusplus
extern "C" {            //  Assume C declarations for C++
#endif  //C++
#endif

EXT  int getsencamfunc(HINSTANCE *sencamLib);
EXT  int freesencamlib(HINSTANCE *sencamLib);
EXT  BOOL getdialoglib(HINSTANCE *senDialogLib,char *name);
EXT  void freedialoglib(HINSTANCE *senDialogLib);

//pointer to sen_cam lib functions
EXT  int (WINAPI *initboard)(int board,HANDLE *hdriver);
EXT  int (WINAPI *closeboard)(HANDLE *hdriver);

EXT  int (WINAPI *setup_camera)(HANDLE hdriver);
EXT  int (WINAPI *run_coc)(HANDLE hdriver,int mode);
EXT  int (WINAPI *stop_coc)(HANDLE hdriver,int mode);

EXT  int (WINAPI *set_coc)(HANDLE hdriver,int mode,int trig,int roixmin, int roixmax,
                        int roiymin, int roiymax,int hbin,int vbin,char *timevalues);
EXT  int (WINAPI *test_coc)(HANDLE hdriver,int *mode,int *trig,int *roixmin, int *roixmax,
                        int *roiymin, int *roiymax,int *hbin,int *vbin,
                        char *tab,int *tablength);
EXT  int (WINAPI *get_coc)(HANDLE hdriver,int *mode,int *trig,int *roixmin, int *roixmax,
                        int *roiymin, int *roiymax,int *hbin,int *vbin,
                        char *tab,int len);

EXT  int (WINAPI *GetSizes)(HANDLE hdriver, int *ccdxsize, int *ccdysize,
                         int *actxsize, int *actysize,
                         int *bit_pix);


EXT  int (WINAPI *get_cam_param)(HANDLE hdriver, struct cam_param *param);
EXT  int (WINAPI *get_cam_values)(HANDLE hdriver, struct cam_values *val);
EXT  int (WINAPI *get_cam_settings)(HANDLE hdriver, struct cam_settings *set);

EXT  int (WINAPI *get_status)(HANDLE hdriver,int *camtype,int *eletemp,int *ccdtemp);
EXT  int (WINAPI *clear_board_buffer)(HANDLE hdriver);

EXT  int (WINAPI *read_image_12bit)(HANDLE hdriver,int mode,int width,int height,unsigned short *b12);

EXT  int (WINAPI *wait_for_image)(HANDLE hdriver,int timeout);

EXT  int (WINAPI *get_dialog_dllname)(HANDLE hdriver,char* name,int len);
EXT  int (WINAPI *get_camera_desc)(HANDLE hdriver,SC_Camera_Description *strCamDesc);

EXT  int (WINAPI *RemoveAllBuffers)(HANDLE hdriver);
EXT  int (WINAPI *AllocateBufferEx)(HANDLE hdriver,int *bufnr,int size,HANDLE *hPicEvent,void** adr);
EXT  int (WINAPI *FreeBuffer)(HANDLE hdriver,int bufnr);
EXT  int (WINAPI *AddBuffer)(HANDLE hdriver,int bufnr,int size,int offset,int data);
EXT  int (WINAPI *RemoveBuffer)(HANDLE hdriver,int bufnr);
EXT  int (WINAPI *GetBufferStatus)(HANDLE hdriver,int bufnr,int mode,int *stat,int len);
/*
//pointer to buffer functions
EXT  int (WINAPI *allocate_buffer)(HANDLE hdriver,int *bufnr,int *size);
EXT  int (WINAPI *allocate_buffer_ex)(HANDLE hdriver,int *bufnr,int size,HANDLE *hPicEvent,void** adr);

EXT  int (WINAPI *getbuffer_status)(HANDLE hdriver,int bufnr,int mode,int *stat,int len);
EXT  int (WINAPI *add_buffer_to_list)(HANDLE hdriver,int bufnr,int size,int offset,int data);
EXT  int (WINAPI *remove_buffer_from_list)(HANDLE hdriver,int bufnr);
EXT  int (WINAPI *setbuffer_event)(HANDLE hdriver,int bufnr,HANDLE *hPicEvent);
EXT  int (WINAPI *clearbuffer_event)(HANDLE hdriver,int bufnr,HANDLE *hPicEvent);
EXT  int (WINAPI *map_buffer)(HANDLE hdriver,int bufnr,int size,int offset,void **linadr);
EXT  int (WINAPI *unmap_buffer)(HANDLE hdriver,int bufnr);
EXT  int (WINAPI *add_buffer)(HANDLE hdriver,int size,void *adr,HANDLE hevent,DWORD* Status);
EXT  int (WINAPI *remove_buffer)(HANDLE hdriver,void *adr);
*/

//pointer to senc_xxx dialog  functions
EXT  int (WINAPI *end_dialog_cam)(int board);
EXT  int (WINAPI *init_dialog_cam_ext)(int board,HINSTANCE SencamLib);
EXT  int (WINAPI *open_dialog_cam)(int board,HWND hWnd, int mode, char *title);
EXT  int (WINAPI *lock_dialog_cam)(int board,int mode);
EXT  int (WINAPI *close_dialog_cam)(int board);
EXT  int (WINAPI *status_dialog_cam)(int board,int *hwnd,int *status);
EXT  int (WINAPI *set_dialog_cam)(int board,int mode, int trig,
                           int ca, int cb,int cc, int cd,
                           int hbin, int vbin,
                           char *linevalues);
EXT  int (WINAPI *get_dialog_cam)(int board,int *mode, int *trig,
                           int *ca, int *cb,int *cc, int *cd,
                           int *hbin, int *vbin,
                           char *tab,int len);
EXT int (WINAPI *read_registry)(int board,int *xps, int *yps);
EXT int (WINAPI *create_registry)(int board);
EXT int (WINAPI *write_registry)(int board);

#ifdef PCO_SENCAMLIB_H_CREATE_OBJECT
#ifdef __cplusplus
}            //  Assume C declarations for C++
#endif  //C++
#endif
