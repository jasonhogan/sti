
// Copied from the PCO SDK
int (__stdcall *GetGeneral)(HANDLE ph,PCO_General *strGeneral);
int (__stdcall *GetCameraType)(HANDLE ph,PCO_CameraType *strCamType);
int (__stdcall *GetCameraHealthStatus)(HANDLE ph,DWORD* dwWarn,DWORD* dwErr,DWORD* dwStatus);
int (__stdcall *ResetSettingsToDefault)(HANDLE ph);
int (__stdcall *InitiateSelftestProcedure)(HANDLE ph);
int (__stdcall *GetTemperature)(HANDLE ph,SHORT* sCCDTemp,SHORT* sCamTemp,SHORT* sPowTemp);
int (__stdcall *GetSensorStruct)(HANDLE ph,PCO_Sensor *strSensor);
int (__stdcall *SetSensorStruct)(HANDLE ph,PCO_Sensor *strSensor);
int (__stdcall *GetCameraDescription)(HANDLE ph,PCO_Description *strDescription);
int (__stdcall *GetSensorFormat)(HANDLE ph,WORD* wSensor);
int (__stdcall *SetSensorFormat)(HANDLE ph,WORD wSensor);
int (__stdcall *GetSizes)(HANDLE ph,WORD *wXResAct,WORD *wYResAct,WORD *wXResMax,WORD *wYResMax);
int (__stdcall *GetROI)(HANDLE ph,WORD *wRoiX0,WORD *wRoiY0,WORD *wRoiX1,WORD *wRoiY1);
int (__stdcall *SetROI)(HANDLE ph,WORD wRoiX0,WORD wRoiY0,WORD wRoiX1,WORD wRoiY1);
int (__stdcall *GetBinning)(HANDLE ph,WORD *wBinHorz,WORD *wBinVert);
int (__stdcall *SetBinning)(HANDLE ph,WORD wBinHorz,WORD wBinVert);
int (__stdcall *GetPixelRate)(HANDLE ph,DWORD *dwPixelRate);
int (__stdcall *SetPixelRate)(HANDLE ph,DWORD dwPixelRate);
int (__stdcall *GetConversionFactor)(HANDLE ph,WORD *wConvFact);
int (__stdcall *SetConversionFactor)(HANDLE ph,WORD wConvFact);
int (__stdcall *GetDoubleImageMode)(HANDLE ph,WORD *wDoubleImage);
int (__stdcall *SetDoubleImageMode)(HANDLE ph,WORD wDoubleImage);
int (__stdcall *GetADCOperation)(HANDLE ph,WORD *wADCOperation);
int (__stdcall *SetADCOperation)(HANDLE ph,WORD wADCOperation);
int (__stdcall *GetIRSensitivity)(HANDLE ph,WORD *wIR);
int (__stdcall *SetIRSensitivity)(HANDLE ph,WORD wIR);
int (__stdcall *GetCoolingSetpointTemperature)(HANDLE ph,SHORT *sCoolSet);
int (__stdcall *SetCoolingSetpointTemperature)(HANDLE ph,SHORT sCoolSet);
int (__stdcall *GetOffsetMode)(HANDLE ph,WORD *wOffsetRegulation);
int (__stdcall *SetOffsetMode)(HANDLE ph,WORD wOffsetRegulation);
int (__stdcall *StartSelfCalibration)(HANDLE ph,WORD *wCalibrated);
int (__stdcall *GetDSNUCorrectionMode)(HANDLE ph,WORD *wDSNUCorrectionMode);
int (__stdcall *SetDSNUCorrectionMode)(HANDLE ph,WORD wDSNUCorrectionMode);
int (__stdcall *GetTimingStruct)(HANDLE ph,PCO_Timing *strTiming);
int (__stdcall *SetTimingStruct)(HANDLE ph,PCO_Timing *strTiming);
int (__stdcall *GetDelayExposureTime)(HANDLE ph,DWORD* dwDelay,DWORD* dwExposure,WORD* wTimeBaseDelay,WORD* wTimeBaseExposure);
int (__stdcall *SetDelayExposureTime)(HANDLE ph,DWORD dwDelay,DWORD dwExposure,WORD wTimeBaseDelay,WORD wTimeBaseExposure);
int (__stdcall *GetDelayExposureTimeTable)(HANDLE ph,DWORD* dwDelay,DWORD* dwExposure,WORD* wTimeBaseDelay,WORD* wTimeBaseExposure,WORD wCount);
int (__stdcall *SetDelayExposureTimeTable)(HANDLE ph,DWORD* dwDelay,DWORD* dwExposure,WORD wTimeBaseDelay,WORD wTimeBaseExposure,WORD wCount);
int (__stdcall *GetTriggerMode)(HANDLE ph,WORD* wTriggerMode);
int (__stdcall *SetTriggerMode)(HANDLE ph,WORD wTriggerMode);
int (__stdcall *ForceTrigger)(HANDLE ph,WORD *wTriggered);
int (__stdcall *GetCameraBusyStatus)(HANDLE ph,WORD* wCameraBusyState);
int (__stdcall *GetPowerDownMode)(HANDLE ph,WORD* wPowerDownMode);
int (__stdcall *SetPowerDownMode)(HANDLE ph,WORD wPowerDownMode);
int (__stdcall *GetUserPowerDownTime)(HANDLE ph,DWORD* dwPowerDownTime);
int (__stdcall *SetUserPowerDownTime)(HANDLE ph,DWORD dwPowerDownTime);
int (__stdcall *GetExpTrigSignalStatus)(HANDLE ph,WORD* wExpTrgSignal);
int (__stdcall *GetCOCRuntime)(HANDLE ph,DWORD* dwTime_s,DWORD* dwTime_us);
int (__stdcall *GetStorageStruct)(HANDLE ph,PCO_Storage *strStorage);
int (__stdcall *SetStorageStruct)(HANDLE ph,PCO_Storage *strStorage);
int (__stdcall *GetCameraRamSize)(HANDLE ph,DWORD* dwRamSize,WORD* wPageSize);
int (__stdcall *GetCameraRamSegmentSize)(HANDLE ph,DWORD* dwRamSegSize);
int (__stdcall *SetCameraRamSegmentSize)(HANDLE ph,DWORD* dwRamSegSize);
int (__stdcall *ClearRamSegment)(HANDLE ph);
int (__stdcall *GetActiveRamSegment)(HANDLE ph,WORD* wActSeg);
int (__stdcall *SetActiveRamSegment)(HANDLE ph,WORD wActSeg);
int (__stdcall *GetRecordingStruct)(HANDLE ph,PCO_Recording *strRecording);
int (__stdcall *SetRecordingStruct)(HANDLE ph,PCO_Recording *strRecording);
int (__stdcall *GetStorageMode)(HANDLE ph,WORD* wStorageMode);
int (__stdcall *SetStorageMode)(HANDLE ph,WORD wStorageMode);
int (__stdcall *GetRecorderSubmode)(HANDLE ph,WORD* wRecSubmode);
int (__stdcall *SetRecorderSubmode)(HANDLE ph,WORD wRecSubmode);
int (__stdcall *GetRecordingState)(HANDLE ph,WORD* wRecState);
int (__stdcall *SetRecordingState)(HANDLE ph,WORD wRecState);
int (__stdcall *ArmCamera)(HANDLE ph);
int (__stdcall *GetAcquireMode)(HANDLE ph,WORD* wAcquMode);
int (__stdcall *SetAcquireMode)(HANDLE ph,WORD wAcquMode);
int (__stdcall *GetAcqEnblSignalStatus)(HANDLE ph,WORD* wAcquEnableState);
int (__stdcall *SetDateTime)(HANDLE ph,BYTE ucDay,BYTE ucMonth,WORD wYear,WORD wHour,BYTE ucMin,BYTE ucSec);
int (__stdcall *GetTimestampMode)(HANDLE ph,WORD* wTimeStampMode);
int (__stdcall *SetTimestampMode)(HANDLE ph,WORD wTimeStampMode);
int (__stdcall *GetImageStruct)(HANDLE ph,PCO_Image *strImage);
int (__stdcall *GetSegmentStruct)(HANDLE ph,WORD wSegment,PCO_Segment *strSegment);
int (__stdcall *GetSegmentImageSettings)(HANDLE ph,WORD wSegment,WORD* wXRes,WORD* wYRes,WORD* wBinHorz,WORD* wBinVert,WORD* wRoiX0,WORD* wRoiY0,WORD* wRoiX1,WORD* wRoiY1);
int (__stdcall *GetNumberOfImagesInSegment)(HANDLE ph,WORD wSegment,DWORD* dwValidImageCnt,DWORD* dwMaxImageCnt);
int (__stdcall *OpenCamera)(HANDLE *ph,WORD wCamNum);
int (__stdcall *CloseCamera)(HANDLE ph);
int (__stdcall *AllocateBuffer)(HANDLE ph,SHORT* sBufNr,DWORD size,WORD** wBuf,HANDLE *hEvent);
int (__stdcall *FreeBuffer)(HANDLE ph,SHORT sBufNr);
int (__stdcall *AddBuffer)(HANDLE ph,DWORD dw1stImage,DWORD dwLastImage,SHORT sBufNr);
int (__stdcall *AddBufferEx)(HANDLE ph,DWORD dw1stImage,DWORD dwLastImage,SHORT sBufNr, WORD wXRes, WORD wYRes, WORD wBitRes);
int (__stdcall *GetBufferStatus)(HANDLE ph,SHORT sBufNr,DWORD *dwStatusDll,DWORD *dwStatusDrv);
int (__stdcall *RemoveBuffer)(HANDLE ph);
int (__stdcall *GetImage)(HANDLE ph,WORD dwSegment,DWORD dw1stImage,DWORD dwLastImage,SHORT sBufNr);
int (__stdcall *GetPendingBuffer)(HANDLE ph,int *count);
int (__stdcall *CancelImages)(HANDLE ph);
int (__stdcall *CheckDeviceAvailability)(HANDLE ph,WORD wNum);
int (__stdcall *CamLinkSetImageParameters)(HANDLE ph, WORD wXResAct, WORD wYResAct);
