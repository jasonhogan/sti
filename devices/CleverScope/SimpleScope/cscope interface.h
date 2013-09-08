
#include <string>

using namespace System;

#define num_acquisition_units 2		//set to maximum number of acquisition units to talk to
#define	max_samples 2100000
#define	max_chars 1000
#define	t_divisions	10
#define	v_divisions	8
#define	master	0
#define	slave	1
#define both	1
#define	linked	1

//types
	typedef char				int8;
	typedef unsigned char		uInt8;
	typedef uInt8				uChar;
	typedef short int			int16;
	typedef unsigned short int	uInt16;
	typedef _int32				int32;
	typedef unsigned long		uInt32;
	typedef float				float32;
	typedef double				float64;

	typedef uInt8 LVBoolean;
	typedef struct {
		int32	cnt;		/* number of bytes that follow */
		uChar	str[1];		/* cnt bytes */
		} LStr, *LStrPtr, **LStrHandle;


//actions:
enum scope_acquisition_types {acq_single, acq_auto, acq_triggered, acq_stop};
enum scope_commands {c_init, c_acquire, c_replay, c_check, c_update, c_close, c_idle, c_function, c_get_frame, c_finish};
enum scope_status {c_runtime_closed, c_closed, c_open,  c_fault, c_fault_closed, c_open_pending};
enum scope_trigger_channel { trig_chan_a, trig_chan_b, trig_ext, trig_dig};
enum scope_interface {if_usb, if_ethernet_find_ip, if_ethernet_first_ip, if_ethernet_find_sernum};
enum scope_clock_source {internal_clk, external_clk};
enum scope_unit_usage {one_unit, two_seperate, two_linked};


static float64 v_div[9] = { 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2, 5};
static	float64 t_div[25] = { 1,0.5,0.2,0.1,0.05,0.02,0.01,0.005,0.002,0.001,
					  500e-6,200e-6,100e-6, 50e-6,20e-6,10e-6,5e-6,2e-6,1e-6,
					  500e-9,200e-9,100e-9,50e-9,20e-9,10e-9};
static	float64 freqs[21] = {1.0,2,5,10,20,50,100,200,500,1000,2000,5000,10000,
					 20000,50000,100000,200000,500000,1000000,2000000,5000000};



// = Sample Oscilloscope Include File =====================================
uInt16 call_cscope_control_driver(int32 unit_number, uInt16 command);
uInt16	scope_init (int32 unit_number);
uInt16	cscope_interface(int32 unit_number, uInt16 Intf_source, uInt32 TCP_adr);
uInt16	scope_close (int32 unit_number);
uInt16 scope_finish (void);
uInt16	scope_config (int32 unit_number);
uInt16	scope_acquire (int32 unit_number);
uInt16 scope_read_waveform (int32 unit_number, float32* a_waveform[], float32* b_waveform[],
							int32 *num_samples, float64 *delta_t, float64 *t_zero);
uInt16	check_for_samples(int32 unit_number);
uInt16	get_CAU_status(int32 unit_number);
void update_values(int32 unit_number,float64 a_div, float64 b_div, float64 t_div,
				    float64 a_div_center, float64 b_div_center, float64 t_div_center, int32 number_of_points,
					float64 freq, float64 sigvolts, float64 trigvolts, int32 trig_chan,
					uInt16 trigger_action, uInt32 dig_pattern, uInt32 clock_source, uInt32 unit_usage);

char *eng(float64 value, int digits, int numeric);
uInt32 string_to_IP(String ^ ip_str);

float64 get_function(int32 unit_number, uInt16 cmd_value, float64 cmd_parameter);

