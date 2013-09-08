#using <mscorlib.dll>

using namespace System;
using namespace System::Runtime::InteropServices;
//using namespace System::Diagnostics;

#if (0)  //_MSC_VER > 1499 // Visual C++ 2008 only
#include <msclr/marshal.h>
using namespace msclr::interop;
#endif

#include "cscope interface.h"
#include "Cscope Control Driver.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//#define uInt32 unsigned long


int32		scope_err;
int32		samples_required;			//the number of samples to capture and display
uInt32		samples_returned[num_acquisition_units];  //actual number of samples returned,
uInt32		ds;							//dummy samples
float64		dt,t0, ddt;						//holds time increment and start value, dummy dt
uInt32		frames_returned;			//number of frames returned after sequential transfer
acquire_defn acquire[num_acquisition_units];		//holds the acquire definition
float32		samples[num_acquisition_units*2][max_samples];	  	//contains the channels array
float32		sample[1];					//dummy for when we are no expecting samples
uInt16		dig_samples[max_samples];	//contains teh digotal channel values
TD2	error;					//holds the error value
LVBoolean	got_samples,dgt;	//set to 1 when we have samples
int32 		frame_number;				//frame number to extract for multi-frame replay
uInt16		CAU_status;					//gives status of CAU. 0 = Runtime Closed, 1 = Closed ,
//2 = Open, 3 = Fault and open, 4 = Fault and Closed
unsigned char tx_chars[max_chars];		//contains character array to send to link
unsigned char rx_chars[max_chars];		//contains character array received from link
float64		function_result;			//result of function call

//**************************************************

uInt16 call_cscope_control_driver(int32 unit_number, uInt16 command)
//use this routine to call the control driver with a particular command
{
	if (unit_number <= num_acquisition_units-1)
	{
		if (command == c_check)
		{
			CscopeControlDriver(unit_number, command, acquire[unit_number].StartTime, acquire[unit_number].StopTime, samples_required,frame_number, &acquire[unit_number], &got_samples,
				&t0, &dt, &samples_returned[unit_number], &frames_returned, samples[unit_number*2], max_samples,  samples[unit_number*2+1], max_samples,
				dig_samples, sizeof(dig_samples), &CAU_status, &error);
			//			Debug::WriteLine(String::Concat("Check Unit Num: ",unit_number.ToString(),"  Got_sample: ",got_samples.ToString(),"  Num samples :",samples_returned[unit_number].ToString()));
		}
		else
		{
			CscopeControlDriver(unit_number, command, acquire[unit_number].StartTime, acquire[unit_number].StopTime, samples_required,frame_number, &acquire[unit_number], &dgt,
				&ddt, &ddt, &ds, &ds, sample, 1,  sample, 1,
				dig_samples, sizeof(dig_samples), &CAU_status, &error);
		}

	}
	return CAU_status;
}

//***********************************************************************
float64 get_function(int32 unit_number, uInt16 cmd_value, float64 cmd_parameter)
//use this routine to call the control driver function command

{
	CscopeFunction(unit_number, cmd_value, cmd_parameter,tx_chars, sizeof(tx_chars), 
		&function_result, rx_chars, sizeof(rx_chars),&error);
	return function_result;
}


//**************************************************************
uInt16	scope_init (int32 unit_number)
//Assumes the scope is connected and opens it.
//Sets up the default values
{
	acquire[unit_number].AcquireMode = 3;		//automatic  (0 = single, 1 = automatic, 2 = triggered, 3 = stop)
	acquire[unit_number].AcquisitionMode = 1;	//peak captured
	acquire[unit_number].Acquirer =  4;			//cleverscope is the acquirer
	acquire[unit_number].TransferChans = 2;		//transfer both channels
	acquire[unit_number].AMaxScale = 2;			// Volts range = +/-2
	acquire[unit_number].AMinScale = -2;
	acquire[unit_number].BMaxScale = 2;
	acquire[unit_number].BMinScale = -2;
	acquire[unit_number].AProbe = 0;				//x1
	acquire[unit_number].BProbe = 0;				//x1
	acquire[unit_number].ACoupling = 1;			//DC
	acquire[unit_number].BCoupling = 1;			//DC
	acquire[unit_number].ABandwidth = 1;			//100 MHz
	acquire[unit_number].BBandwidth = 1;			//100 MHz
	acquire[unit_number].TriggerSource = 0;		//A Chan trigger
	acquire[unit_number].TriggerAmplitude = 0;	//Trigger at zero volts
	acquire[unit_number].ATriggerAmplitude = 0;
	acquire[unit_number].BTriggerAmplitude = 0;
	acquire[unit_number].TriggerFilter = 0;		//No trigger filter
	acquire[unit_number].TrigSlope = 0;			//rising
	acquire[unit_number].TriggerHoldoff = 0;
	acquire[unit_number].DigPatternRqd = 0;		//not used
	acquire[unit_number].DigPattern =  0;		//not used
	acquire[unit_number].ExtTrigThreshold = 0;
	acquire[unit_number].DigInputThreshold = 2;
	acquire[unit_number].StartTime =  -0.005;	//-5 msecs
	acquire[unit_number].StopTime =   0.005;		//5 msecs
	acquire[unit_number].PreTrigTime = 0.005;
	acquire[unit_number].Port = 0;
	acquire[unit_number].NumDivisions = 10;
	acquire[unit_number].NumSeqFrames = 1;
	acquire[unit_number].NumBuffers = 2;
	acquire[unit_number].SigGenFreq = 1000;		//1kHz output
	acquire[unit_number].SigGenAmp = 1;			//1V amplitude
	acquire[unit_number].SigGenOffset = 0;
	acquire[unit_number].SigGenWaveform = 0;		//sine
	acquire[unit_number].SigGenSweep = 0;
	acquire[unit_number].SigGenFunc = 0;
	acquire[unit_number].SigGenFreq2 = 0;
	acquire[unit_number].SigGenPhase = 0;
	acquire[unit_number].Trig2Function = 0;		//not used
	acquire[unit_number].MinTriggerPeriod = 0.0000001;
	acquire[unit_number].MaxTriggerPeriod = 1;
	acquire[unit_number].TriggerCount = 1;
	acquire[unit_number].Trig2Slope = 0;
	acquire[unit_number].Trig2SourceChan = 0;
	acquire[unit_number].Trig2Level = 0;
	acquire[unit_number].DigPattern2Rqd = 0;
	acquire[unit_number].DigPattern2 = 0;
	acquire[unit_number].Trigger2Source = 0;
	acquire[unit_number].WaveformAverages = 1;
	acquire[unit_number].ValueChanged = 1;
	acquire[unit_number].SamplerResolution = 0;	//0 = 10 bit (1 = 12 bit, 2 = 14 bit).
	acquire[unit_number].IntfSource = 0;			//0 = USB, 1 = Ethernet - open given IP address,
	//2 = Ethernet - open next free CAU, 3 = Ethernet - open  given Serial number
	acquire[unit_number].TCPPort = 53270;
	acquire[unit_number].TCPAdr = 0xC0A80169;     //192.168.1.105

	acquire[unit_number].ExtSampleClock = 0;
	acquire[unit_number].UpdateRate =0;
	acquire[unit_number].TransferSize =0;

	frame_number = 0;
	samples_required = 1000;
	return call_cscope_control_driver(unit_number,c_init);
}

//**************************************************
uInt16	cscope_interface(int32 unit_number, uInt16 Intf_source, uInt32 TCP_adr)
//Updates the interface spec. If the CAU_statrus indicates the scope is not open, attempts to open it.
//Call this after changing the IP address etc.
{
	acquire[unit_number].ValueChanged++;
	acquire[unit_number].IntfSource = Intf_source;
	acquire[unit_number].TCPAdr = TCP_adr;
	CAU_status = call_cscope_control_driver(unit_number,c_idle);
	if (CAU_status != c_open)
	{
		return call_cscope_control_driver(unit_number,c_init);
	}
	else
	{
		return 0;
	}
}

//*************************************************
uInt16 scope_close (int32 unit_number)
//closes the scope
{
	return call_cscope_control_driver(unit_number,c_close);
}

//*************************************************
uInt16 scope_finish (void)
//finishes up - the unit_number is not important
{
	return call_cscope_control_driver(0,c_finish);

}


//****************************************************
void update_values(int32 unit_number, float64 a_div, float64 b_div, float64 t_div,
				   float64 a_div_center, float64 b_div_center, float64 t_div_center, int32 number_of_points,
				   float64 freq, float64 sigvolts, float64 trigvolts, int32 trig_chan,
				   uInt16 trigger_action, uInt32 dig_pattern, uInt32 clock_source, uInt32 unit_usage)
				   //updates the acquire variable only. If triggered = 1 then we do triggered capture, otherwise auto capture
{
	acquire[unit_number].AMaxScale = a_div_center + (v_divisions * a_div / 2);
	acquire[unit_number].AMinScale = a_div_center - (v_divisions * a_div / 2);
	acquire[unit_number].BMaxScale = b_div_center + (v_divisions * b_div / 2);
	acquire[unit_number].BMinScale = b_div_center - (v_divisions * b_div / 2);
	acquire[unit_number].StopTime = t_div_center + (t_divisions * t_div /2);
	acquire[unit_number].StartTime = t_div_center - (t_divisions * t_div /2);
	acquire[unit_number].SigGenFreq = freq;
	acquire[unit_number].SigGenAmp = sigvolts;
	acquire[unit_number].TriggerAmplitude = trigvolts;
	acquire[unit_number].AcquireMode = trigger_action; //0= auto, 1 = triggered, 3 = stop
	acquire[unit_number].ValueChanged++;
	acquire[unit_number].TriggerSource = trig_chan;
	acquire[unit_number].LinkPort = 0;	//debug UART as default
	if (unit_usage == two_linked)
	{
		if (unit_number == slave)
		{
			acquire[unit_number].TriggerSource = 4;		//Link Input
			acquire[unit_number].LinkPort = 3;		//This is a Slave Cleverscope
		}
		else
		{
			acquire[unit_number].LinkPort = 4;		//This is a Master Cleverscope
		}
	}
	if (clock_source == external_clk)
		acquire[unit_number].ExtSampleClock = 1;
	else
		acquire[unit_number].ExtSampleClock = 0;

	switch (trig_chan)
	{
	case trig_chan_a:
		acquire[unit_number].TriggerAmplitude = trigvolts;
		break;
	case trig_chan_b:
		acquire[unit_number].TriggerAmplitude = trigvolts;
		break;
	case trig_ext:
		acquire[unit_number].ExtTrigThreshold = trigvolts;
		break;
	case trig_dig:
		acquire[unit_number].DigInputThreshold = trigvolts;
		break;
	}
	acquire[unit_number].DigPattern = dig_pattern;
	if (number_of_points < max_samples) samples_required = number_of_points;
	else samples_required = max_samples;
}

//*******************************************************
uInt16 scope_config (int32 unit_number)
//Configures major values for the acquisition unit.
{
	return call_cscope_control_driver(unit_number,c_update);
}

//*************************************************************************

uInt16	scope_acquire (int32 unit_number)
//start an acquisition

{
	return call_cscope_control_driver(unit_number,c_acquire);
}

//**************************************************************************
uInt16 scope_read_waveform (int32 unit_number, float32* a_waveform[], float32* b_waveform[], int32 *num_samples, float64 *delta_t, float64 *t_zero)
//returns the last read waveform for the given channel
{
	*a_waveform = samples[unit_number*2];
	*b_waveform = samples[unit_number*2+1];
	*delta_t = dt;
	*t_zero = t0;
	*num_samples = samples_returned[unit_number];
	return CAU_status;
}

//**************************************************************************

uInt16 check_for_samples(int32 unit_number)
//checks to see if samples have been returned. If so returns 1, else 0
{
	call_cscope_control_driver(unit_number,c_check);
	return got_samples;
}
//****************************************************************************

uInt16	get_CAU_status(int32 unit_number)
//gets the current CAU status
{
	call_cscope_control_driver(unit_number,c_idle);
	return CAU_status;
}

//****************************************************************************

//Support routines



//-------------------------------------------------------------------------
#define PREFIX_START (-24)
/* Smallest power of then for which there is a prefix defined.
If the set of prefixes will be extended, change this constant
and update the table "prefix". */

char *eng(float64 value, int digits, int numeric)
{
	static char *prefix[] = {
		"y", "z", "a", "f", "p", "n", "u", "m", "",
		"k", "M", "G", "T", "P", "E", "Z", "Y"
	};
#define PREFIX_END (PREFIX_START+\
	(int)((sizeof(prefix)/sizeof(char *)-1)*3))

	int expof10;
	static char result[100];
	char *res = result;

	if (value < 0.)
	{
		*res++ = '-';
		value = -value;
	}
	if (value != 0.0)
	{
		expof10 = (int) log10(value);
		if(expof10 > 0)
			expof10 = (expof10/3)*3;
		else
			expof10 = (-expof10+3)/3*(-3);

		value *= pow(10.0,-expof10);

		if (value >= 1000.)
		{ value /= 1000.0; expof10 += 3; }
		else if(value >= 100.0)
			digits -= 2;
		else if(value >= 10.0)
			digits -= 1;
	}
	else
	{
		expof10 = 0;	//no prefix
		digits=3;
	}
	if(numeric || (expof10 < PREFIX_START) ||
		(expof10 > PREFIX_END))
		sprintf_s(res,20, "%.*fe%d", digits-1, value, expof10);
	else
		sprintf_s(res,20, "%.*f %s", digits-1, value,
		prefix[(expof10-PREFIX_START)/3]);
	return result;
}

//**********************************************************************************

uInt32 string_to_IP(String ^ ip_string)
//converts the string pointed at by ip_str into an IP address,
//assuming the format nnn.nnn.nnn.nnn
//
{

	uInt32	ip_adr;
	int8	* pch;
	int8	* next_token;
	char* ip_str = (char*)Marshal::StringToHGlobalAnsi(ip_string).ToPointer();
	ip_adr = 0;
	pch = strtok_s( ip_str,".",&next_token);
	while (pch != NULL)
	{
		ip_adr = (ip_adr<<8) + atoi(pch);
		pch = strtok_s (NULL, ".",&next_token);
	}
	Marshal::FreeHGlobal((IntPtr)ip_str);
	return ip_adr;
}
//------------------------------------------------------------------------



