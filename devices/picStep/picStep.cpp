//                     PIC-STEP Demonstration Program
//This program, compiled using Borland's C++ Builder, demonstrates the
//basic operation of the PIC-STEP controller.  It assumes that one PIC-STEP
//controller module is connected to your PC's COM1: port through a Z232-485
//serial port adapter converter.  It assumes the PIC-STEP controller is
//connected to the example driver circuit displayed in the PIC-STEP data sheet.
//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop

#include "main.h"
#include "sio_util.h"
#include "nmccom.h"
#include "picstep.h"
//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::InitButtonClick(TObject *Sender)
{
int nummod;

InitButton->Enabled = false;

//Supress printing of error messages
ErrorPrinting(0);

//Initialize NMC controllers on COM1: using 19200 Baud
nummod = NmcInit("COM1:", 19200);

if (nummod < 1)
  {
  SimpleMsgBox("No NMC Modules found!");
  Close();
  }

NmcReadStatus(1, SEND_ID);
if (NmcGetModType(1) != STEPMODTYPE)
  {
  SimpleMsgBox("Module 1 is not a PIC-STEP!");
  Close();
  }
else SimpleMsgBox("PIC-STEP Module found");

SetParamButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetParamButtonClick(TObject *Sender)
{
SetParamButton->Enabled = false;

//Set Motion parameters:
//	SPEED_1X = speed increments of 25 steps/second
//	Limit switches, E-Stop igonred
//  Minimum speed = 20 (x 25 steps/sec)
//  Holding current = 25 (out of 255 max)
//	Running current = 100 (out of 255 max)
StepSetParam(1, SPEED_1X | IGNORE_LIMITS | IGNORE_ESTOP, 20, 100, 25, 0);

//Set assorted output bits to configure the driver
StepSetOutputs(1, 0x02 + 0x04 + 0x08);

OnButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnButtonClick(TObject *Sender)
{
OnButton->Enabled = false;

StepStopMotor(1, AMP_ENABLE);

MoveButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::MoveButtonClick(TObject *Sender)
{
MoveButton->Enabled = false;

//Load motion parameters and execute motion:
//	Load position, speed & acc. time to specify a trap. pos. motion
//	Start now bit set = begin motion immediately
//	Goal position = 5000
//	Max velocity = 100 (x 25 steps/sec.)
//	Accel. time = 10
//	(Raw speed value ignored, set to 0)
StepLoadTraj(1, LOAD_POS | LOAD_SPEED | LOAD_ACC | START_NOW, 5000, 100, 10, 0);

do   //poll PIC-STEP while the motor is moving
  {
  NmcNoOp(1);   //poll with No-Op command to get status back
  }
while ( NmcGetStat(1) & MOTOR_MOVING );

ReadButton->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
//Reset the network of controllers
NmcHardReset(1);

//Clean-up and close the COM port
NmcShutdown();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ExitButtonClick(TObject *Sender)
{
Close();	
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ReadButtonClick(TObject *Sender)
{
char msgstr[80];

//Read the position and then display it
NmcReadStatus(1, SEND_POS);

sprintf(msgstr,"Motor Position = %ld", StepGetPos(1));
SimpleMsgBox(msgstr);
}
//---------------------------------------------------------------------------