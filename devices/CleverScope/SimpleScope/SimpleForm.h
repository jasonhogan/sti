#pragma once

#include <string>
#include <string.h>


//**************************************************************************************
//
//
//  See the timer_tick function at the end of this file for where all the action happens
//
//**********************************************************************************

#include "STIDeviceCLRBridge.h"




namespace SimpleScope {

	using namespace System;

	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace	ZedGraph;
	using namespace std;
//	using namespace System::Diagnostics;

	using namespace System::Runtime::InteropServices;
	
	[UnmanagedFunctionPointerAttribute(CallingConvention::Cdecl)]
//	delegate void ManagedSummer(int arg);

	delegate void ManagedCallback(void);

		enum e_calibration : unsigned short {Idle,StartCalibrate,GndOfsAmeasure,GndOfsBMeasure,BaselineMeasure,
				RefAMeasure,RefBMeasure,SaveHWValues,StartSigGenCal,StartExtTrigCal,
				StartDigInpCal,DoCalibrate,SetRef};

		enum e_functions	:unsigned short {GetSerialNum,GetFirmwareVer,GetDriverVer,GetResolution,GetFrameLength,
				GetTemperature,StartLinkSend,SendLinkData,ReadLinkData,ActiveMessage,Calibrate,
				SetCalRef,SamplingStatus};

	/// <summary>
	/// Summary for SimpleForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>


	public ref class SimpleForm : public System::Windows::Forms::Form
	{
	public:
		SimpleForm(STIDeviceCLRBridgeWrapper* device)
		{
			deviceWrapper = device;
			returnTraceA = false;
			returnTraceB = false;

			InitializeComponent();
		}
	private:
			//For control by STI device
		STIDeviceCLRBridgeWrapper* deviceWrapper;
		bool returnTraceA;
		bool returnTraceB;
		static short ChannelA = 0;
		static short ChannelB = 1;

	//***********************************************************************
	//                                APPLICATION CODE
	//************************************************************************

	static double 		a_vert = 1, b_vert = 1, horz = 0.001;
	static	double		a_vert_center = 0, b_vert_center = 0, horz_center = 0;			//center of amplitude and time windows
	static	double		sig_gen_volts = 1.0, sig_gen_freq = 1000.0;
	static	_int32		channel = 1;
	static	_int32 		trig_channel = 0;
	static	double		trig_volts = 0.0;
	static	_int32		single_acquire = false;
	static	_int32		auto_acquire = false;
	static	_int32		trigger_acquire = false;
	static  _int32		num_points = 1000;
	static  _int32		vals_changed = 0;  
	static	_int32		clock_source = 0;	//0 = Internal, 1 = external
	static	_int32		unit_usage = 0;		//0 = One Unit, 1 = Two units - seperate, 2 = Two units - linked 

	static  unsigned _int16 	trigger_action = 3;				//0 = auto, 1 = single, 3 = stop
	static  unsigned _int32 	digital_pattern =0x00010001;	//digital pattern used for digital triggering
	static  unsigned _int16  	Interface_Source;				//The interface source
	static  unsigned _int32 	Interface_address;				//the IP address for the interface
	
	static  unsigned _int16 	CAU_status;					//{c_runtime_closed, c_closed, c_open,  c_fault, c_fault_closed};
	static  _int32				Cscope_index;				//index into current cleverscope
	static	_int32				waiting_for_trigger = 0;	//0 means we have not started an acquire. 
															//1 means we are waiting for the trigger on the slave
															//2 means we are waiting for the trigger on the master
	
	static	bool			f_timer_active;		//true if timer may become active
	static	_int32			stop_sync;			//used for synchronizing stop with timer.
	static  unsigned _int16	function_value;		//contains function number to send
	static	bool			f_calibrating;		//true if we are calibrating
	static	e_calibration	calibration;		//holds current calibration value
	static	e_functions		functions;			//holds current functions value



	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::Label^  label14;
	private: System::Windows::Forms::Label^  label13;
	private: System::Windows::Forms::Label^  label12;
	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label15;
	private: System::Windows::Forms::Label^  label16;


	private: System::Windows::Forms::ComboBox^  Intf_source;
	private: System::Windows::Forms::ComboBox^  Cscope_Status;
	private: System::Windows::Forms::Button^	button1;
	private: System::Windows::Forms::TextBox^	IP_address;
	private: System::Windows::Forms::Label^		label17;
	private: System::Windows::Forms::Label^		label18;
	private: System::Windows::Forms::ComboBox^  Cscope_unit_index;
	private: System::Windows::Forms::ComboBox^  Trigger_channel;
	private: System::Windows::Forms::Label^		label19;
	private: System::Windows::Forms::Label^		label20;
	private: System::Windows::Forms::Timer^		timer1;
	private: System::Windows::Forms::Button^	Restart;
	private: System::Windows::Forms::Label^		label21;
	private: System::Windows::Forms::Label^		label22;
	private: System::Windows::Forms::ComboBox^  Cmd_Function;

	private: System::Windows::Forms::Button^		Cmd_get;
	private: System::Windows::Forms::Label^			Cmd_value;
	private: System::Windows::Forms::RichTextBox^	richTextBox1;
	private: System::Windows::Forms::Label^			label23;
	private: System::Windows::Forms::ComboBox^		CalibrateStep;
	private: System::Windows::Forms::Label^			label24;
	private: System::Windows::Forms::Button^		Triggered;
	private: System::Windows::Forms::NumericUpDown^ TCenter;
	private: System::Windows::Forms::Label^			label25;
	private: System::Windows::Forms::Label^			label26;
	private: System::Windows::Forms::NumericUpDown^ ACenter;
	private: System::Windows::Forms::Label^			label27;
	private: System::Windows::Forms::Label^			label28;
	private: System::Windows::Forms::NumericUpDown^ BCenter;
	private: System::Windows::Forms::NumericUpDown^ NumSamples;
	private: System::Windows::Forms::Label^			label29;
	private: System::Windows::Forms::NumericUpDown^ SamplesReturned;

	private: System::Windows::Forms::Label^		label30;
	private: System::Windows::Forms::ComboBox^  ClockSource;
	private: System::Windows::Forms::Label^		label31;
	private: System::Windows::Forms::ComboBox^  Num_units;
	private: System::Windows::Forms::Label^		label32;
	private: System::Windows::Forms::Label^		label8;


//****************************************************************************

void Setval(System::Windows::Forms::Label^  ctrl, double value, char *suffix)
{
	  char  st[20] = "";
	  strcat_s(st, eng(value, 3, 0));
	  strcat_s(st,suffix);
	  String^ ctrl_val = gcnew String(st);
	  ctrl->Text = ctrl_val;
}

void SetReal(System::Windows::Forms::Label^  ctrl, double value, char *suffix)
{
	  ctrl->Text = value.ToString();
}

//***********************************************
void InitializeGUI ()
{

	single_acquire = false;
	auto_acquire = false;
	waiting_for_trigger = false;
	vals_changed = true;
	f_calibrating = false;
	calibration = Idle;
	functions = GetSerialNum;

	Setval(Adiv, a_vert,"V");

	Setval(Bdiv, b_vert,"V");
	Setval(Tdiv, horz,"s");
	Setval(TrigV, trig_volts,"V");
	Setval(SigGenF, sig_gen_freq,"Hz");
	Setval(SigGenV, sig_gen_volts,"V");

	SetA_Vertical->Value = 6;
	SetB_Vertical->Value = 6;
	SetHorizontal->Value = 9;
	zedGraphControl1->GraphPane->YAxis->Scale->Min = -a_vert*v_divisions/2;
	zedGraphControl1->GraphPane->YAxis->Scale->Max = a_vert*v_divisions/2 ;
	zedGraphControl1->GraphPane->YAxis->Scale->MajorStep = a_vert;
	zedGraphControl1->GraphPane->Y2Axis->Scale->Min = -b_vert*v_divisions/2;
	zedGraphControl1->GraphPane->Y2Axis->Scale->Max = b_vert*v_divisions/2 ;
	zedGraphControl1->GraphPane->Y2Axis->Scale->MajorStep = b_vert;
	zedGraphControl1->GraphPane->XAxis->Scale->Min = -horz*v_divisions/2;
	zedGraphControl1->GraphPane->XAxis->Scale->Max = horz*v_divisions/2 ;
	zedGraphControl1->GraphPane->XAxis->Scale->MajorStep = horz;
	stop_sync = 0;
	f_timer_active = true;
	timer1->Enabled = true;
}
//*************************************************************************


//**************************************************************
// Build the Chart
void CreateGraph( ZedGraphControl ^zgc )
{
	// get a reference to the GraphPane
	GraphPane ^myPane = zgc->GraphPane;

	// Set the Titles
	myPane->Title->Text = "Simple Scope";
	myPane->XAxis->Title->Text = "Time (s)";
	myPane->YAxis->Title->Text = "A Volts";
	myPane->Y2Axis->Title->Text = "B Volts";

	// Make up some data arrays based on the Sine function
	double x, y;
	PointPairList ^list1 = gcnew PointPairList();
	PointPairList ^list2 = gcnew PointPairList();
	PointPairList ^list3 = gcnew PointPairList();
	PointPairList ^list4 = gcnew PointPairList();
	for ( _int32 i = 0; i < 2; i++ )	//just add one point to each pair list
	{
		x = 0;
		y = 0;
		list1->Add( x, y );
		list2->Add( x, y );
		list3->Add( x, y );
		list4->Add( x, y );
	}

	// Generate a red curve
	LineItem ^myCurve = myPane->AddCurve( "",
		list1, Color::Red, SymbolType::None );
	// Generate a blue curve
	myCurve = myPane->AddCurve( "",
		list2, Color::Blue, SymbolType::None );
	myCurve->IsY2Axis = true;
	// Generate a green curve
	myCurve = myPane->AddCurve( "",list3, Color::Green, SymbolType::None );
	// Generate a Purple curve
	myCurve = myPane->AddCurve( "", list4, Color::Purple, SymbolType::None );
	// Associate this curve with the Y2 axis
	myCurve->IsY2Axis = true;

	// Show the x axis grid
	myPane->XAxis->MajorTic->IsOpposite = false;
	myPane->XAxis->MinorTic->IsOpposite = false;
	myPane->XAxis->MajorGrid->IsVisible = true;


	// Make the Y axis scale red
	myPane->YAxis->Scale->FontSpec->FontColor = Color::Red;
	myPane->YAxis->Title->FontSpec->FontColor = Color::Red;
	// turn off the opposite tics so the Y tics don't show up on the Y2 axis
	myPane->YAxis->MajorTic->IsOpposite = false;
	myPane->YAxis->MinorTic->IsOpposite = false;
	// Don't display the Y zero line
	myPane->YAxis->MajorGrid->IsZeroLine = false;
	myPane->YAxis->MajorGrid->IsVisible = true;
	// Align the Y axis labels so they are flush to the axis
	myPane->YAxis->Scale->Align = AlignP::Inside;
	// Manually set the axis range
	myPane->YAxis->Scale->Min = -10;
	myPane->YAxis->Scale->Max = 10;


	// Enable the Y2 axis display
	myPane->Y2Axis->IsVisible = true;
	// Make the Y2 axis scale blue
	myPane->Y2Axis->Scale->FontSpec->FontColor = Color::Blue;
	myPane->Y2Axis->Title->FontSpec->FontColor = Color::Blue;
	// turn off the opposite tics so the Y2 tics don't show up on the Y axis
	myPane->Y2Axis->MajorTic->IsOpposite = false;
	myPane->Y2Axis->MinorTic->IsOpposite = false;
	// Display the Y2 axis grid lines
	myPane->Y2Axis->MajorGrid->IsVisible = false;
	// Align the Y2 axis labels so they are flush to the axis
	myPane->Y2Axis->Scale->Align = AlignP::Inside;
	myPane->Y2Axis->MajorGrid->IsZeroLine = false;


	// Tell ZedGraph to refigure the
	// axes since the data have changed
	zgc->AxisChange();
	zgc->Invalidate();
}

//--------------------------------------------------------------------------

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~SimpleForm()
		{
			if (components)
			{
				delete components;
			}
		}
private: System::Windows::Forms::Button^  Auto;
protected:

	private: System::Windows::Forms::Button^	single_trigger;
	private: System::Windows::Forms::TrackBar^  SetHorizontal;
	private: System::Windows::Forms::TrackBar^  Set_SigGen_Freq;
	private: System::Windows::Forms::TrackBar^  Set_SigGen_Volts;
	private: System::Windows::Forms::TrackBar^  SetA_Vertical;
	private: System::Windows::Forms::TrackBar^  SetB_Vertical;
	private: System::Windows::Forms::TrackBar^  Set_Trigger_Volts;


	private: System::Windows::Forms::Button^  QuitButton;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  Adiv;
	private: System::Windows::Forms::Label^  Bdiv;
	private: System::Windows::Forms::Label^  Tdiv;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  TrigV;
	private: System::Windows::Forms::Label^  SigGenF;
	private: System::Windows::Forms::Label^  SigGenV;
	private: ZedGraph::ZedGraphControl^  zedGraphControl1;
private: System::Windows::Forms::PictureBox^  Trig_LED;


	protected:
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(SimpleForm::typeid));
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->Auto = (gcnew System::Windows::Forms::Button());
			this->single_trigger = (gcnew System::Windows::Forms::Button());
			this->SetHorizontal = (gcnew System::Windows::Forms::TrackBar());
			this->Set_SigGen_Freq = (gcnew System::Windows::Forms::TrackBar());
			this->Set_SigGen_Volts = (gcnew System::Windows::Forms::TrackBar());
			this->SetA_Vertical = (gcnew System::Windows::Forms::TrackBar());
			this->SetB_Vertical = (gcnew System::Windows::Forms::TrackBar());
			this->Set_Trigger_Volts = (gcnew System::Windows::Forms::TrackBar());
			this->QuitButton = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->Adiv = (gcnew System::Windows::Forms::Label());
			this->Bdiv = (gcnew System::Windows::Forms::Label());
			this->Tdiv = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->TrigV = (gcnew System::Windows::Forms::Label());
			this->SigGenF = (gcnew System::Windows::Forms::Label());
			this->SigGenV = (gcnew System::Windows::Forms::Label());
			this->zedGraphControl1 = (gcnew ZedGraph::ZedGraphControl());
			this->Trig_LED = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->Intf_source = (gcnew System::Windows::Forms::ComboBox());
			this->Cscope_Status = (gcnew System::Windows::Forms::ComboBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->IP_address = (gcnew System::Windows::Forms::TextBox());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->Cscope_unit_index = (gcnew System::Windows::Forms::ComboBox());
			this->Trigger_channel = (gcnew System::Windows::Forms::ComboBox());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->Restart = (gcnew System::Windows::Forms::Button());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->Cmd_Function = (gcnew System::Windows::Forms::ComboBox());
			this->Cmd_get = (gcnew System::Windows::Forms::Button());
			this->Cmd_value = (gcnew System::Windows::Forms::Label());
			this->richTextBox1 = (gcnew System::Windows::Forms::RichTextBox());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->CalibrateStep = (gcnew System::Windows::Forms::ComboBox());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->Triggered = (gcnew System::Windows::Forms::Button());
			this->TCenter = (gcnew System::Windows::Forms::NumericUpDown());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->ACenter = (gcnew System::Windows::Forms::NumericUpDown());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->BCenter = (gcnew System::Windows::Forms::NumericUpDown());
			this->NumSamples = (gcnew System::Windows::Forms::NumericUpDown());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->SamplesReturned = (gcnew System::Windows::Forms::NumericUpDown());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->ClockSource = (gcnew System::Windows::Forms::ComboBox());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->Num_units = (gcnew System::Windows::Forms::ComboBox());
			this->label32 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SetHorizontal))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Set_SigGen_Freq))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Set_SigGen_Volts))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SetA_Vertical))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SetB_Vertical))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Set_Trigger_Volts))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Trig_LED))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TCenter))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ACenter))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->BCenter))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NumSamples))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SamplesReturned))->BeginInit();
			this->SuspendLayout();
			// 
			// timer1
			// 
			this->timer1->Interval = 50;
			this->timer1->Tick += gcnew System::EventHandler(this, &SimpleForm::timer1_Tick);
			// 
			// Auto
			// 
			this->Auto->Location = System::Drawing::Point(520, 472);
			this->Auto->Name = L"Auto";
			this->Auto->Size = System::Drawing::Size(64, 27);
			this->Auto->TabIndex = 0;
			this->Auto->Text = L"Auto";
			this->Auto->UseVisualStyleBackColor = true;
			this->Auto->Click += gcnew System::EventHandler(this, &SimpleForm::Auto_Click);
			// 
			// single_trigger
			// 
			this->single_trigger->Location = System::Drawing::Point(468, 472);
			this->single_trigger->Name = L"single_trigger";
			this->single_trigger->Size = System::Drawing::Size(48, 27);
			this->single_trigger->TabIndex = 1;
			this->single_trigger->Text = L"Single";
			this->single_trigger->UseVisualStyleBackColor = true;
			this->single_trigger->Click += gcnew System::EventHandler(this, &SimpleForm::single_trigger_Click);
			// 
			// SetHorizontal
			// 
			this->SetHorizontal->LargeChange = 1;
			this->SetHorizontal->Location = System::Drawing::Point(481, 206);
			this->SetHorizontal->Maximum = 23;
			this->SetHorizontal->Name = L"SetHorizontal";
			this->SetHorizontal->Size = System::Drawing::Size(184, 42);
			this->SetHorizontal->TabIndex = 2;
			this->SetHorizontal->Value = 15;
			this->SetHorizontal->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::SetHorizontal_ValueChanged);
			// 
			// Set_SigGen_Freq
			// 
			this->Set_SigGen_Freq->LargeChange = 1;
			this->Set_SigGen_Freq->Location = System::Drawing::Point(12, 67);
			this->Set_SigGen_Freq->Maximum = 20;
			this->Set_SigGen_Freq->Name = L"Set_SigGen_Freq";
			this->Set_SigGen_Freq->Size = System::Drawing::Size(160, 42);
			this->Set_SigGen_Freq->TabIndex = 3;
			this->Set_SigGen_Freq->Value = 9;
			this->Set_SigGen_Freq->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::Set_SigGen_Freq_ValueChanged);
			// 
			// Set_SigGen_Volts
			// 
			this->Set_SigGen_Volts->LargeChange = 1;
			this->Set_SigGen_Volts->Location = System::Drawing::Point(178, 67);
			this->Set_SigGen_Volts->Maximum = 40;
			this->Set_SigGen_Volts->Name = L"Set_SigGen_Volts";
			this->Set_SigGen_Volts->Size = System::Drawing::Size(129, 42);
			this->Set_SigGen_Volts->TabIndex = 4;
			this->Set_SigGen_Volts->Value = 5;
			this->Set_SigGen_Volts->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::Set_SigGen_Volts_ValueChanged);
			// 
			// SetA_Vertical
			// 
			this->SetA_Vertical->LargeChange = 1;
			this->SetA_Vertical->Location = System::Drawing::Point(514, 308);
			this->SetA_Vertical->Maximum = 8;
			this->SetA_Vertical->Name = L"SetA_Vertical";
			this->SetA_Vertical->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->SetA_Vertical->Size = System::Drawing::Size(42, 103);
			this->SetA_Vertical->TabIndex = 5;
			this->SetA_Vertical->Value = 6;
			this->SetA_Vertical->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::SetA_Vertical_ValueChanged);
			// 
			// SetB_Vertical
			// 
			this->SetB_Vertical->LargeChange = 1;
			this->SetB_Vertical->Location = System::Drawing::Point(619, 308);
			this->SetB_Vertical->Maximum = 8;
			this->SetB_Vertical->Name = L"SetB_Vertical";
			this->SetB_Vertical->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->SetB_Vertical->Size = System::Drawing::Size(42, 98);
			this->SetB_Vertical->TabIndex = 6;
			this->SetB_Vertical->Value = 6;
			this->SetB_Vertical->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::SetB_Vertical_ValueChanged);
			// 
			// Set_Trigger_Volts
			// 
			this->Set_Trigger_Volts->LargeChange = 1;
			this->Set_Trigger_Volts->Location = System::Drawing::Point(637, 28);
			this->Set_Trigger_Volts->Minimum = -10;
			this->Set_Trigger_Volts->Name = L"Set_Trigger_Volts";
			this->Set_Trigger_Volts->Orientation = System::Windows::Forms::Orientation::Vertical;
			this->Set_Trigger_Volts->Size = System::Drawing::Size(42, 114);
			this->Set_Trigger_Volts->TabIndex = 7;
			this->Set_Trigger_Volts->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::Set_Trigger_Volts_ValueChanged);
			// 
			// QuitButton
			// 
			this->QuitButton->Location = System::Drawing::Point(316, 9);
			this->QuitButton->Name = L"QuitButton";
			this->QuitButton->Size = System::Drawing::Size(50, 24);
			this->QuitButton->TabIndex = 10;
			this->QuitButton->Text = L"Quit";
			this->QuitButton->UseVisualStyleBackColor = true;
			this->QuitButton->Click += gcnew System::EventHandler(this, &SimpleForm::QuitButton_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(64, 57);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(69, 13);
			this->label1->TabIndex = 11;
			this->label1->Text = L"Sig Gen Freq";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(223, 57);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(71, 13);
			this->label2->TabIndex = 12;
			this->label2->Text = L"Sig Gen Volts";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(592, 140);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(39, 13);
			this->label3->TabIndex = 13;
			this->label3->Text = L"Volts =";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(496, 272);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(61, 13);
			this->label4->TabIndex = 14;
			this->label4->Text = L"A Volts/Div";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(604, 272);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(61, 13);
			this->label5->TabIndex = 15;
			this->label5->Text = L"B Volts/Div";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(528, 188);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(60, 13);
			this->label6->TabIndex = 16;
			this->label6->Text = L"Time/Div =";
			// 
			// Adiv
			// 
			this->Adiv->AutoSize = true;
			this->Adiv->Location = System::Drawing::Point(508, 288);
			this->Adiv->Name = L"Adiv";
			this->Adiv->Size = System::Drawing::Size(43, 13);
			this->Adiv->TabIndex = 17;
			this->Adiv->Text = L"500 mV";
			this->Adiv->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// Bdiv
			// 
			this->Bdiv->AutoSize = true;
			this->Bdiv->Location = System::Drawing::Point(612, 288);
			this->Bdiv->Name = L"Bdiv";
			this->Bdiv->Size = System::Drawing::Size(43, 13);
			this->Bdiv->TabIndex = 18;
			this->Bdiv->Text = L"500 mV";
			this->Bdiv->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// Tdiv
			// 
			this->Tdiv->AutoSize = true;
			this->Tdiv->Location = System::Drawing::Point(592, 188);
			this->Tdiv->Name = L"Tdiv";
			this->Tdiv->Size = System::Drawing::Size(43, 13);
			this->Tdiv->TabIndex = 19;
			this->Tdiv->Text = L"500 mV";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label7->Location = System::Drawing::Point(617, 5);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(65, 20);
			this->label7->TabIndex = 20;
			this->label7->Text = L"Trigger";
			// 
			// TrigV
			// 
			this->TrigV->AutoSize = true;
			this->TrigV->Location = System::Drawing::Point(630, 140);
			this->TrigV->Name = L"TrigV";
			this->TrigV->Size = System::Drawing::Size(43, 13);
			this->TrigV->TabIndex = 21;
			this->TrigV->Text = L"500 mV";
			// 
			// SigGenF
			// 
			this->SigGenF->AutoSize = true;
			this->SigGenF->Location = System::Drawing::Point(66, 101);
			this->SigGenF->Name = L"SigGenF";
			this->SigGenF->Size = System::Drawing::Size(43, 13);
			this->SigGenF->TabIndex = 22;
			this->SigGenF->Text = L"500 mV";
			// 
			// SigGenV
			// 
			this->SigGenV->AutoSize = true;
			this->SigGenV->Location = System::Drawing::Point(232, 101);
			this->SigGenV->Name = L"SigGenV";
			this->SigGenV->Size = System::Drawing::Size(43, 13);
			this->SigGenV->TabIndex = 23;
			this->SigGenV->Text = L"500 mV";
			// 
			// zedGraphControl1
			// 
			this->zedGraphControl1->Location = System::Drawing::Point(7, 135);
			this->zedGraphControl1->Name = L"zedGraphControl1";
			this->zedGraphControl1->ScrollGrace = 0;
			this->zedGraphControl1->ScrollMaxX = 0;
			this->zedGraphControl1->ScrollMaxY = 0;
			this->zedGraphControl1->ScrollMaxY2 = 0;
			this->zedGraphControl1->ScrollMinX = 0;
			this->zedGraphControl1->ScrollMinY = 0;
			this->zedGraphControl1->ScrollMinY2 = 0;
			this->zedGraphControl1->Size = System::Drawing::Size(456, 361);
			this->zedGraphControl1->TabIndex = 24;
			// 
			// Trig_LED
			// 
			this->Trig_LED->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Trig_LED.Image")));
			this->Trig_LED->Location = System::Drawing::Point(656, 473);
			this->Trig_LED->Name = L"Trig_LED";
			this->Trig_LED->Size = System::Drawing::Size(22, 24);
			this->Trig_LED->TabIndex = 25;
			this->Trig_LED->TabStop = false;
			this->Trig_LED->Visible = false;
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(656, 473);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(22, 24);
			this->pictureBox1->TabIndex = 26;
			this->pictureBox1->TabStop = false;
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(384, 92);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(137, 13);
			this->label14->TabIndex = 59;
			this->label14->Text = L"arrow keys, or scroll wheel. ";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(384, 79);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(135, 13);
			this->label13->TabIndex = 58;
			this->label13->Text = L"Adjust controls, with mouse";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(384, 66);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(117, 13);
			this->label12->TabIndex = 57;
			this->label12->Text = L"Click on Single or Auto.";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(384, 53);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(141, 13);
			this->label11->TabIndex = 56;
			this->label11->Text = L"Connect Sig Gen to Chan A.";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(384, 39);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(145, 13);
			this->label10->TabIndex = 55;
			this->label10->Text = L"Cleverscope Acquisition Unit.";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(384, 26);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(115, 13);
			this->label9->TabIndex = 54;
			this->label9->Text = L"Connect and power up";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(384, 9);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(165, 16);
			this->label8->TabIndex = 53;
			this->label8->Text = L"How to use Simple Scope:";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label15->Location = System::Drawing::Point(4, 508);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(105, 16);
			this->label15->TabIndex = 62;
			this->label15->Text = L"Interface Source";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label16->Location = System::Drawing::Point(386, 508);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(76, 16);
			this->label16->TabIndex = 64;
			this->label16->Text = L"CAU Status";
			// 
			// Intf_source
			// 
			this->Intf_source->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Intf_source->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"USB", L"Ethernet - Open IP address", L"Ethernet - Find first Cleverscope", 
				L"Ethernet - Open serial number"});
			this->Intf_source->Location = System::Drawing::Point(7, 528);
			this->Intf_source->Name = L"Intf_source";
			this->Intf_source->Size = System::Drawing::Size(151, 20);
			this->Intf_source->TabIndex = 67;
			this->Intf_source->Text = L"USB";
			this->Intf_source->SelectedIndexChanged += gcnew System::EventHandler(this, &SimpleForm::Intf_source_SelectedIndexChanged);
			// 
			// Cscope_Status
			// 
			this->Cscope_Status->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->Cscope_Status->Items->AddRange(gcnew cli::array< System::Object^  >(6) {L"Runtime Closed", L"Closed", L"Open", L"Fault", 
				L"Fault Closed", L"Open pending"});
			this->Cscope_Status->Location = System::Drawing::Point(389, 527);
			this->Cscope_Status->Name = L"Cscope_Status";
			this->Cscope_Status->Size = System::Drawing::Size(101, 21);
			this->Cscope_Status->TabIndex = 68;
			this->Cscope_Status->Text = L"Runtime Closed";
			// 
			// button1
			// 
			this->button1->BackColor = System::Drawing::SystemColors::Control;
			this->button1->Enabled = false;
			this->button1->FlatAppearance->BorderColor = System::Drawing::SystemColors::Control;
			this->button1->FlatAppearance->BorderSize = 0;
			this->button1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button1->ForeColor = System::Drawing::SystemColors::Control;
			this->button1->Location = System::Drawing::Point(468, 525);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(41, 26);
			this->button1->TabIndex = 69;
			this->button1->UseVisualStyleBackColor = false;
			// 
			// IP_address
			// 
			this->IP_address->Location = System::Drawing::Point(164, 527);
			this->IP_address->Name = L"IP_address";
			this->IP_address->Size = System::Drawing::Size(100, 20);
			this->IP_address->TabIndex = 70;
			this->IP_address->Text = L"192.168.1.100";
			this->IP_address->TextChanged += gcnew System::EventHandler(this, &SimpleForm::IP_address_TextChanged);
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label17->Location = System::Drawing::Point(164, 508);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(89, 16);
			this->label17->TabIndex = 71;
			this->label17->Text = L"TCP Address";
			this->label17->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label18
			// 
			this->label18->AutoSize = true;
			this->label18->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label18->Location = System::Drawing::Point(264, 508);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(47, 16);
			this->label18->TabIndex = 73;
			this->label18->Text = L"Unit ID";
			this->label18->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// Cscope_unit_index
			// 
			this->Cscope_unit_index->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->Cscope_unit_index->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"0", L"1", L"2", L"3", L"4", L"5", L"6", 
				L"7"});
			this->Cscope_unit_index->Location = System::Drawing::Point(270, 526);
			this->Cscope_unit_index->Name = L"Cscope_unit_index";
			this->Cscope_unit_index->Size = System::Drawing::Size(47, 21);
			this->Cscope_unit_index->TabIndex = 74;
			this->Cscope_unit_index->Text = L"0";
			this->Cscope_unit_index->SelectedIndexChanged += gcnew System::EventHandler(this, &SimpleForm::Cscope_unit_index_SelectedIndexChanged);
			// 
			// Trigger_channel
			// 
			this->Trigger_channel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Trigger_channel->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Chan A", L"Chan B", L"Ext Trigger", L"Dig Trigger"});
			this->Trigger_channel->Location = System::Drawing::Point(547, 84);
			this->Trigger_channel->Name = L"Trigger_channel";
			this->Trigger_channel->Size = System::Drawing::Size(76, 21);
			this->Trigger_channel->TabIndex = 75;
			this->Trigger_channel->Text = L"Chan A";
			this->Trigger_channel->SelectedIndexChanged += gcnew System::EventHandler(this, &SimpleForm::Trigger_channel_SelectedIndexChanged);
			// 
			// label19
			// 
			this->label19->AutoSize = true;
			this->label19->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label19->Location = System::Drawing::Point(544, 42);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(52, 16);
			this->label19->TabIndex = 76;
			this->label19->Text = L"Trigger";
			this->label19->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// label20
			// 
			this->label20->AutoSize = true;
			this->label20->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label20->Location = System::Drawing::Point(544, 60);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(57, 16);
			this->label20->TabIndex = 77;
			this->label20->Text = L"Channel";
			this->label20->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// Restart
			// 
			this->Restart->Location = System::Drawing::Point(323, 525);
			this->Restart->Name = L"Restart";
			this->Restart->Size = System::Drawing::Size(57, 23);
			this->Restart->TabIndex = 78;
			this->Restart->Text = L"Re-open";
			this->Restart->UseVisualStyleBackColor = true;
			this->Restart->Click += gcnew System::EventHandler(this, &SimpleForm::Restart_Click);
			// 
			// label21
			// 
			this->label21->AutoSize = true;
			this->label21->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label21->Location = System::Drawing::Point(12, 16);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(146, 20);
			this->label21->TabIndex = 79;
			this->label21->Text = L"Signal Generator";
			// 
			// label22
			// 
			this->label22->AutoSize = true;
			this->label22->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label22->Location = System::Drawing::Point(520, 164);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(110, 20);
			this->label22->TabIndex = 80;
			this->label22->Text = L"Oscilloscope";
			// 
			// Cmd_Function
			// 
			this->Cmd_Function->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Cmd_Function->Items->AddRange(gcnew cli::array< System::Object^  >(13) {L"Get Serial Num", L"Get Firmware Ver", L"Get Driver Ver", 
				L"Get Resolution", L"Get Frame length", L"Get Temperature", L"Start Link Send", L"Send Link Data", L"Read Link Data", L"Active Message", 
				L"Calibrate", L"Set Cal Ref", L"Sampling Status"});
			this->Cmd_Function->Location = System::Drawing::Point(476, 525);
			this->Cmd_Function->Name = L"Cmd_Function";
			this->Cmd_Function->Size = System::Drawing::Size(110, 21);
			this->Cmd_Function->TabIndex = 81;
			this->Cmd_Function->Text = L"Get Serial Num";
			this->Cmd_Function->SelectedIndexChanged += gcnew System::EventHandler(this, &SimpleForm::Cmd_Function_SelectedIndexChanged);
			// 
			// Cmd_get
			// 
			this->Cmd_get->Location = System::Drawing::Point(590, 524);
			this->Cmd_get->Name = L"Cmd_get";
			this->Cmd_get->Size = System::Drawing::Size(43, 23);
			this->Cmd_get->TabIndex = 82;
			this->Cmd_get->Text = L"Do";
			this->Cmd_get->UseVisualStyleBackColor = true;
			this->Cmd_get->Click += gcnew System::EventHandler(this, &SimpleForm::Cmd_get_Click);
			// 
			// Cmd_value
			// 
			this->Cmd_value->AutoSize = true;
			this->Cmd_value->Location = System::Drawing::Point(634, 529);
			this->Cmd_value->Name = L"Cmd_value";
			this->Cmd_value->Size = System::Drawing::Size(37, 13);
			this->Cmd_value->TabIndex = 83;
			this->Cmd_value->Text = L"00000";
			this->Cmd_value->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// richTextBox1
			// 
			this->richTextBox1->Location = System::Drawing::Point(716, 5);
			this->richTextBox1->Name = L"richTextBox1";
			this->richTextBox1->Size = System::Drawing::Size(284, 352);
			this->richTextBox1->TabIndex = 84;
			this->richTextBox1->Text = resources->GetString(L"richTextBox1.Text");
			// 
			// label23
			// 
			this->label23->AutoSize = true;
			this->label23->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label23->Location = System::Drawing::Point(472, 508);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(123, 16);
			this->label23->TabIndex = 85;
			this->label23->Text = L"Function Command";
			// 
			// CalibrateStep
			// 
			this->CalibrateStep->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->CalibrateStep->Items->AddRange(gcnew cli::array< System::Object^  >(13) {L"Idle", L"Start Calibrate", L"Gnd ofs A measure", 
				L"Gnd ofs B measure", L"Baseline measure", L"Ref A measure", L"Ref B measure", L"Save HW values", L"Start Sig Gen cal", L"Start Ext Trig cal", 
				L"Start Dig Inp cal", L"Calibrate", L"Set Ref"});
			this->CalibrateStep->Location = System::Drawing::Point(716, 404);
			this->CalibrateStep->Name = L"CalibrateStep";
			this->CalibrateStep->Size = System::Drawing::Size(110, 21);
			this->CalibrateStep->TabIndex = 86;
			this->CalibrateStep->Text = L"Idle";
			this->CalibrateStep->SelectedIndexChanged += gcnew System::EventHandler(this, &SimpleForm::CalibrateStep_SelectedIndexChanged);
			// 
			// label24
			// 
			this->label24->AutoSize = true;
			this->label24->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label24->Location = System::Drawing::Point(716, 384);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(93, 16);
			this->label24->TabIndex = 87;
			this->label24->Text = L"Calibrate Step";
			// 
			// Triggered
			// 
			this->Triggered->Location = System::Drawing::Point(588, 472);
			this->Triggered->Name = L"Triggered";
			this->Triggered->Size = System::Drawing::Size(64, 27);
			this->Triggered->TabIndex = 88;
			this->Triggered->Text = L"Triggered";
			this->Triggered->UseVisualStyleBackColor = true;
			this->Triggered->Click += gcnew System::EventHandler(this, &SimpleForm::Triggered_Click);
			// 
			// TCenter
			// 
			this->TCenter->DecimalPlaces = 3;
			this->TCenter->Location = System::Drawing::Point(564, 240);
			this->TCenter->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, 0});
			this->TCenter->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10000, 0, 0, System::Int32::MinValue});
			this->TCenter->Name = L"TCenter";
			this->TCenter->Size = System::Drawing::Size(84, 20);
			this->TCenter->TabIndex = 89;
			this->TCenter->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->TCenter->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::TCenter_ValueChanged);
			// 
			// label25
			// 
			this->label25->AutoSize = true;
			this->label25->Location = System::Drawing::Point(512, 244);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(48, 13);
			this->label25->TabIndex = 90;
			this->label25->Text = L"T Center";
			// 
			// label26
			// 
			this->label26->AutoSize = true;
			this->label26->Location = System::Drawing::Point(652, 244);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(20, 13);
			this->label26->TabIndex = 91;
			this->label26->Text = L"ms";
			// 
			// ACenter
			// 
			this->ACenter->DecimalPlaces = 1;
			this->ACenter->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->ACenter->Location = System::Drawing::Point(496, 428);
			this->ACenter->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->ACenter->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, System::Int32::MinValue});
			this->ACenter->Name = L"ACenter";
			this->ACenter->Size = System::Drawing::Size(52, 20);
			this->ACenter->TabIndex = 92;
			this->ACenter->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->ACenter->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::ACenter_ValueChanged);
			// 
			// label27
			// 
			this->label27->AutoSize = true;
			this->label27->Location = System::Drawing::Point(496, 412);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(48, 13);
			this->label27->TabIndex = 93;
			this->label27->Text = L"A Center";
			// 
			// label28
			// 
			this->label28->AutoSize = true;
			this->label28->Location = System::Drawing::Point(608, 412);
			this->label28->Name = L"label28";
			this->label28->Size = System::Drawing::Size(48, 13);
			this->label28->TabIndex = 95;
			this->label28->Text = L"B Center";
			// 
			// BCenter
			// 
			this->BCenter->DecimalPlaces = 1;
			this->BCenter->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1, 0, 0, 65536});
			this->BCenter->Location = System::Drawing::Point(604, 428);
			this->BCenter->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->BCenter->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, System::Int32::MinValue});
			this->BCenter->Name = L"BCenter";
			this->BCenter->Size = System::Drawing::Size(52, 20);
			this->BCenter->TabIndex = 94;
			this->BCenter->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->BCenter->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::BCenter_ValueChanged);
			// 
			// NumSamples
			// 
			this->NumSamples->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->NumSamples->Location = System::Drawing::Point(719, 472);
			this->NumSamples->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4000000, 0, 0, 0});
			this->NumSamples->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->NumSamples->Name = L"NumSamples";
			this->NumSamples->Size = System::Drawing::Size(84, 20);
			this->NumSamples->TabIndex = 96;
			this->NumSamples->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->NumSamples->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->NumSamples->ValueChanged += gcnew System::EventHandler(this, &SimpleForm::NumSamples_ValueChanged);
			// 
			// label29
			// 
			this->label29->AutoSize = true;
			this->label29->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label29->Location = System::Drawing::Point(716, 453);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(110, 15);
			this->label29->TabIndex = 97;
			this->label29->Text = L"Samples Required";
			// 
			// SamplesReturned
			// 
			this->SamplesReturned->Enabled = false;
			this->SamplesReturned->Increment = System::Decimal(gcnew cli::array< System::Int32 >(4) {1000, 0, 0, 0});
			this->SamplesReturned->Location = System::Drawing::Point(719, 526);
			this->SamplesReturned->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {4000000, 0, 0, 0});
			this->SamplesReturned->Name = L"SamplesReturned";
			this->SamplesReturned->ReadOnly = true;
			this->SamplesReturned->Size = System::Drawing::Size(84, 20);
			this->SamplesReturned->TabIndex = 98;
			this->SamplesReturned->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label30
			// 
			this->label30->AutoSize = true;
			this->label30->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label30->Location = System::Drawing::Point(716, 508);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(110, 15);
			this->label30->TabIndex = 99;
			this->label30->Text = L"Samples Returned";
			// 
			// ClockSource
			// 
			this->ClockSource->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->ClockSource->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Internal", L"External"});
			this->ClockSource->Location = System::Drawing::Point(857, 471);
			this->ClockSource->Name = L"ClockSource";
			this->ClockSource->Size = System::Drawing::Size(87, 21);
			this->ClockSource->TabIndex = 100;
			this->ClockSource->Text = L"Internal";
			this->ClockSource->SelectedIndexChanged += gcnew System::EventHandler(this, &SimpleForm::ClockSource_SelectedIndexChanged);
			// 
			// label31
			// 
			this->label31->AutoSize = true;
			this->label31->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label31->Location = System::Drawing::Point(857, 453);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(79, 15);
			this->label31->TabIndex = 101;
			this->label31->Text = L"Clock Source";
			// 
			// Num_units
			// 
			this->Num_units->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Num_units->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"1", L"2-seperate", L"2-linked"});
			this->Num_units->Location = System::Drawing::Point(860, 524);
			this->Num_units->Name = L"Num_units";
			this->Num_units->Size = System::Drawing::Size(87, 21);
			this->Num_units->TabIndex = 102;
			this->Num_units->Text = L"1";
			this->Num_units->SelectedIndexChanged += gcnew System::EventHandler(this, &SimpleForm::Num_units_SelectedIndexChanged);
			// 
			// label32
			// 
			this->label32->AutoSize = true;
			this->label32->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label32->Location = System::Drawing::Point(857, 506);
			this->label32->Name = L"label32";
			this->label32->Size = System::Drawing::Size(65, 15);
			this->label32->TabIndex = 103;
			this->label32->Text = L"Num Units";
			// 
			// SimpleForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1018, 572);
			this->Controls->Add(this->label32);
			this->Controls->Add(this->Num_units);
			this->Controls->Add(this->label31);
			this->Controls->Add(this->ClockSource);
			this->Controls->Add(this->label30);
			this->Controls->Add(this->SamplesReturned);
			this->Controls->Add(this->label29);
			this->Controls->Add(this->NumSamples);
			this->Controls->Add(this->label28);
			this->Controls->Add(this->BCenter);
			this->Controls->Add(this->label27);
			this->Controls->Add(this->ACenter);
			this->Controls->Add(this->label26);
			this->Controls->Add(this->label25);
			this->Controls->Add(this->TCenter);
			this->Controls->Add(this->Triggered);
			this->Controls->Add(this->label24);
			this->Controls->Add(this->CalibrateStep);
			this->Controls->Add(this->label23);
			this->Controls->Add(this->richTextBox1);
			this->Controls->Add(this->Cmd_value);
			this->Controls->Add(this->Cmd_get);
			this->Controls->Add(this->Cmd_Function);
			this->Controls->Add(this->label22);
			this->Controls->Add(this->label21);
			this->Controls->Add(this->Restart);
			this->Controls->Add(this->label20);
			this->Controls->Add(this->label19);
			this->Controls->Add(this->Trigger_channel);
			this->Controls->Add(this->Cscope_unit_index);
			this->Controls->Add(this->label18);
			this->Controls->Add(this->label17);
			this->Controls->Add(this->IP_address);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->Cscope_Status);
			this->Controls->Add(this->Intf_source);
			this->Controls->Add(this->label16);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->Trig_LED);
			this->Controls->Add(this->zedGraphControl1);
			this->Controls->Add(this->SigGenV);
			this->Controls->Add(this->SigGenF);
			this->Controls->Add(this->TrigV);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->Tdiv);
			this->Controls->Add(this->Bdiv);
			this->Controls->Add(this->Adiv);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->QuitButton);
			this->Controls->Add(this->Set_Trigger_Volts);
			this->Controls->Add(this->SetB_Vertical);
			this->Controls->Add(this->SetA_Vertical);
			this->Controls->Add(this->Set_SigGen_Volts);
			this->Controls->Add(this->Set_SigGen_Freq);
			this->Controls->Add(this->SetHorizontal);
			this->Controls->Add(this->single_trigger);
			this->Controls->Add(this->Auto);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"SimpleForm";
			this->Text = L"Simple Scope";
			this->Load += gcnew System::EventHandler(this, &SimpleForm::SimpleForm_Load);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &SimpleForm::SimpleForm_FormClosing);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SetHorizontal))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Set_SigGen_Freq))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Set_SigGen_Volts))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SetA_Vertical))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SetB_Vertical))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Set_Trigger_Volts))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Trig_LED))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->TCenter))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->ACenter))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->BCenter))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NumSamples))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SamplesReturned))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


private: System::Void SimpleForm_Load(System::Object^  sender, System::EventArgs^  e)
		 {
			 _int32		status;

			 // Setup the graph
			 CreateGraph( zedGraphControl1 );
			 status =scope_init (master);
			 if (unit_usage>one_unit) status =scope_init (slave);

			 InitializeGUI ();
		 }
//---------------------------------------------------------------------------------------
private: System::Void QuitButton_Click(System::Object^  sender, System::EventArgs^  e)
		 {
			 f_timer_active = false;

			 deviceWrapper->deviceShutdown();
		 }
//---------------------------------------------------------------------------------------
private: System::Void SimpleForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e)
		 {
			 bool	f_tick_happened = false;

			 timer1->Enabled = false;	//we wait until the current timer operation is complete
			 //before closing down.
			 f_timer_active = false;
			 while(Interlocked::CompareExchange(stop_sync, -1, 0) !=0)
			 {
				 f_tick_happened = true;
			 }

			 if (!f_tick_happened)
			 {
				 timer1->Enabled = false;	//turn of timer
				 scope_close (master);
				 if (unit_usage>one_unit) scope_close (slave);
				 scope_finish();
			 }

		 }
//---------------------------------------------------------------------------------------
private: System::Void SetA_Vertical_ValueChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 a_vert =  v_div[SetA_Vertical->Value];
			 Setval(Adiv, a_vert,"V");
			 vals_changed = true;
			 zedGraphControl1->GraphPane->YAxis->Scale->Min = a_vert_center - (a_vert*v_divisions/2);
			 zedGraphControl1->GraphPane->YAxis->Scale->Max = a_vert_center + (a_vert*v_divisions/2) ;
			 zedGraphControl1->GraphPane->YAxis->Scale->MajorStep = a_vert;
			 zedGraphControl1->GraphPane->AxisChange();
			 zedGraphControl1->Invalidate();
		 }
//---------------------------------------------------------------------------------------

private: System::Void SetB_Vertical_ValueChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 b_vert =  v_div[SetB_Vertical->Value];
			 Setval(Bdiv, b_vert,"V");
			 vals_changed = true;
			 zedGraphControl1->GraphPane->Y2Axis->Scale->Min = b_vert_center - (b_vert*v_divisions/2);
			 zedGraphControl1->GraphPane->Y2Axis->Scale->Max = b_vert_center + (b_vert*v_divisions/2) ;
			 zedGraphControl1->GraphPane->Y2Axis->Scale->MajorStep = b_vert;
			 zedGraphControl1->GraphPane->AxisChange();
			 zedGraphControl1->Invalidate();
		 }
//---------------------------------------------------------------------------------------

private: System::Void SetHorizontal_ValueChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 horz =  t_div[SetHorizontal->Value];
			 Setval(Tdiv, horz,"sec");
			 vals_changed = true;
			 zedGraphControl1->GraphPane->XAxis->Scale->Min = horz_center - (horz*t_divisions/2);
			 zedGraphControl1->GraphPane->XAxis->Scale->Max = horz_center + (horz*t_divisions/2);
			 zedGraphControl1->GraphPane->XAxis->Scale->MajorStep = horz;
			 zedGraphControl1->GraphPane->AxisChange();
			 zedGraphControl1->Invalidate();
		 }
//---------------------------------------------------------------------------------------

private: System::Void Set_SigGen_Freq_ValueChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 sig_gen_freq =  freqs[Set_SigGen_Freq-> Value];
			 Setval(SigGenF, sig_gen_freq, "Hz");
			 vals_changed = true;
		 }
//---------------------------------------------------------------------------------------

private: System::Void Set_SigGen_Volts_ValueChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 sig_gen_volts = Set_SigGen_Volts-> Value * 0.2;
			 Setval(SigGenV, sig_gen_volts, "V");
			 vals_changed = true;
		 }
//---------------------------------------------------------------------------------------

private: System::Void Set_Trigger_Volts_ValueChanged(System::Object^  sender, System::EventArgs^  e)
		 {
			 Set_Trigger_Volts_ValueChanged_Impl();
		 }
public: System::Void Set_Trigger_Volts_ValueChanged_Ext()
		 {
			 Set_Trigger_Volts_ValueChanged_Impl();
		 }

private: System::Void Set_Trigger_Volts_ValueChanged_Impl()
		 {
			 trig_volts = Set_Trigger_Volts-> Value * 0.2;
			 Setval(TrigV, trig_volts, "V");
			 vals_changed = true;
		 }
//---------------------------------------------------------------------------------------


private: System::Void single_trigger_Click(System::Object^ sender, System::EventArgs^ e)
		 {
			 single_trigger_Click_Impl(true);
		 }
			 //(Added by Jason Hogan)
private: System::Void single_trigger_Click_Impl(bool toggleSingle)
		 {

 	unsigned _int16	status2;
	status2 = get_CAU_status(master);
//	std::cout << "get_CAU_status Status: " << status2 << std::endl;
//	Console::WriteLine(L"Hello World: " + status2);
//	deviceWrapper->stiError("Hello World: " );


		vals_changed = true;
		auto_acquire = false;
		trigger_acquire = false;
		Auto->Text="Auto";
		Triggered->Text="Triggered";
		if (single_acquire)
			{
				if(toggleSingle)	//Toggles mode if already in single
				{
				single_acquire = false;
				single_trigger->Text="Single";
				trigger_action = acq_stop;   //stop doing stuff
				}
			}
		else
			{
			single_acquire = true;
			single_trigger->Text="Wait..";
			trigger_action = acq_single;   //use triggered captured
			}

		 }
//---------------------------------------------------------------------------------------

 private: System::Void Auto_Click(System::Object^ sender, System::EventArgs^ e) 
		  {
			  Auto_Click_Impl();
		  }

 public: System::Void Auto_Click_External()
		 {
			 Auto_Click_Impl();
		 }

 public: System::Void AcquireAndReturnTraceChannelA()
		 {
			 returnTraceA = true;
			 single_trigger_Click_Impl(false);
		 }

 public: System::Void AcquireAndReturnTraceChannelB()
		 {
			 returnTraceB = true;
			 single_trigger_Click_Impl(false);
		 }

 private: System::Void Auto_Click_Impl()  
		  {
			  vals_changed = true;
			  single_acquire = false;
			  trigger_acquire = false;
			  single_trigger->Text="Single";
			  Triggered->Text="Triggered";

			  if (auto_acquire) {
				  auto_acquire = false;
				  Auto->Text="Auto";
				  trigger_action = acq_stop;   //stop doing stuff
			  }
			  else {
				  auto_acquire = true;
				  Auto->Text="Auto Act..";
				  trigger_action = acq_auto;   //use auto captured
			  }
		  }
//--------------------------------------------------------------------------------
private: System::Void Triggered_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
//			 deviceWrapper->stiError("Triggered!");

			 vals_changed = true;
			 auto_acquire = false;
			 single_acquire = false;
			 Auto->Text="Auto";
			 single_trigger->Text="Single";
			 if (trigger_acquire)
			 {
				 trigger_acquire = false;
				 Triggered->Text="Triggered";
				 trigger_action = acq_stop;   //stop doing stuff
			 }
			 else
			 {
				 trigger_acquire = true;
				 Triggered->Text="Trig Act..";
				 trigger_action = acq_single;   //use triggered captured
			 }


		 }
//*****************************************************************************************
//This is the main event function. It checks to see if any values have changed, and if
//they have, it updates the acquire variable.
//Next it sees if an acquisition is to be made - and if so, makes it.
//Next it checks if there are results - if so, get them
//Finally it checks if the changes that should have been updated by teh acquire (if it happened)
//have happened, and if not, it updates the acquisition unit itself.

 private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e)
		  {
			  _int32	i,step;
			  double	xval,dT_step;
			  _int32	num_samples;
			  double	dT, T0;
			  float32*	a_waveform;
			  float32*	b_waveform;
			  float32*	c_waveform;
			  float32*	d_waveform;
			  unsigned _int16	status;
			  double	cal_result;

			  _int32 sync = Interlocked::CompareExchange(stop_sync, 1, 0);		//take sync to stop close
			  if (sync == 0)	//not being used by close window
			  {
				  timer1->Enabled = false;			//we stop timer events, in case this process takes longer
				  //than a timer tick
				  if (!(single_acquire || auto_acquire || trigger_acquire)) 
				  {
					  waiting_for_trigger = 0;   //kill off wait if nothing to do
					  Trig_LED->Visible = false;
				  }
				  status = get_CAU_status(master);
				  if (unit_usage>one_unit)
				  {
					  if (status == c_open)		//only update status if master already open. 
						  status = get_CAU_status(slave);
				  }
				  if (CAU_status != status)
				  {
					  CAU_status = status;
					  Cscope_Status->SelectedIndex = CAU_status;
				  }

				  if (waiting_for_trigger == 2)	//waiting for the slave
				  {
					  if (check_for_samples(slave))
					  {
						  waiting_for_trigger = 1;			//go and do master
						  scope_read_waveform (slave,&c_waveform, &d_waveform, &num_samples, &dT, &T0);
						  xval = T0 ;
						  IPointListEdit ^ip3 = (IPointListEdit^) zedGraphControl1->GraphPane->CurveList[2]->Points;  //           CurveList[L"Beta"]->Points;
						  IPointListEdit ^ip4 = (IPointListEdit^) zedGraphControl1->GraphPane->CurveList[3]->Points;  //           CurveList[L"Beta"]->Points;
						  step = (int) ((num_samples / 1000.0)+0.5); //1000 points on graph
						  if (step == 0) step = 1;
						  dT_step = dT*step;


						  if (( ip3 != nullptr ) && (ip4 != nullptr))
						  {

							  ip3->Clear();
							  ip4->Clear();

							  for (i=0; i < num_samples; i+=step)
							  {
								  ip3->Add( xval, c_waveform[i] );
								  ip4->Add( xval, d_waveform[i] );
								  xval = xval+dT_step;
							  }

						  }
						  zedGraphControl1->Invalidate();
					  }
				  }

				  if (waiting_for_trigger == 1)	//waiting for master
				  {
					  if (check_for_samples(master))
					  {
						  Trig_LED->Visible = true;
						  waiting_for_trigger = 0;			//finish off
						  if (single_acquire) single_trigger->Text="Single";
						  single_acquire = false;
						  scope_read_waveform (master,&a_waveform, &b_waveform, &num_samples, &dT, &T0);
						  SamplesReturned->Value = num_samples;
						  xval = T0 ;

						  IPointListEdit ^ip1 = (IPointListEdit^) zedGraphControl1->GraphPane->CurveList[0]->Points;
						  IPointListEdit ^ip2 = (IPointListEdit^) zedGraphControl1->GraphPane->CurveList[1]->Points;
						  step = (int) ((num_samples / 1000.0)+0.5); //1000 points on graph
						  if (step == 0) step = 1;
						  dT_step = dT*step;

						  if (( ip1 != nullptr ) && (ip2 != nullptr))
						  {
							  ip1->Clear();
							  ip2->Clear();

							  for (i=0; i < num_samples; i+=step)
							  {
								  ip1->Add( xval, a_waveform[i] );
								  ip2->Add( xval, b_waveform[i] );
								  xval = xval+dT_step;
							  }

							  //Below is a test of a_waveform; puts 0th element in a text box [JMH]
							  double value = a_waveform[0];
							  Cmd_value->Text = value.ToString();

							  if(returnTraceA)
							  {
								  deviceWrapper->sendData(a_waveform, num_samples, ChannelA);
								  returnTraceA = false;

							  }

							  if(returnTraceB)
							  {
								  deviceWrapper->sendData(b_waveform, num_samples, ChannelB);
								  returnTraceB = false;
							  }
						  }
						  zedGraphControl1->Invalidate();
					  }
					  else
					  {
						  Trig_LED->Visible = false;
					  }
				  }

				  if ((waiting_for_trigger == 0) && (single_acquire || auto_acquire || trigger_acquire) && (CAU_status == c_open))
				  {
					  if (vals_changed == 1)	//here we put the latest set of updates in teh acquisition array
					  {
						  update_values(master,a_vert, b_vert, horz,a_vert_center, b_vert_center, horz_center, num_points,
							  sig_gen_freq,sig_gen_volts,trig_volts, trig_channel,trigger_action, digital_pattern,clock_source,unit_usage);

						  update_values(slave,a_vert, b_vert, horz,a_vert_center, b_vert_center, horz_center, num_points,
							  sig_gen_freq,sig_gen_volts,trig_volts, trig_channel,trigger_action, digital_pattern,clock_source,unit_usage);
						  vals_changed = 0;
					  }
					  if (unit_usage>one_unit)
					  {
						  scope_acquire (slave);       //start the slave first so it is ready for the master.
						  waiting_for_trigger = 2;		//start again with slave first
					  }
					  else waiting_for_trigger = 1;		//start again with master first
					  scope_acquire (master);       //is set true while acquiring, there
				  }

				  if (f_calibrating)		//we want to run the calibration system
				  {
					  cal_result = get_function(master,Calibrate,DoCalibrate);
					  SetReal(Cmd_value, cal_result, " ");
					  if ((cal_result <0) || (cal_result == 100))
					  {
						  f_calibrating = false;
						  calibration = Idle;
						  CalibrateStep->SelectedIndex = Idle;
					  }
				  }
				  if (vals_changed)
				  {					//vals have changed that couldn't be handled by teh acquire.
					  update_values(master,a_vert, b_vert, horz,a_vert_center, b_vert_center, horz_center, num_points,
						  sig_gen_freq,sig_gen_volts,trig_volts, trig_channel,trigger_action, digital_pattern,clock_source,unit_usage);

					  update_values(slave,a_vert, b_vert, horz,a_vert_center, b_vert_center, horz_center, num_points,
						  sig_gen_freq,sig_gen_volts,trig_volts, trig_channel,trigger_action, digital_pattern,clock_source,unit_usage);

					  scope_config(master);
					  if (unit_usage>one_unit) scope_config(slave);
					  vals_changed = false;		//updated if acquire did not update them
				  }
				  timer1->Enabled = f_timer_active;	//go back to allowing timer events unless closing
				  if (!f_timer_active)            //we do it here at a nice quantized point
				  {
					  scope_close (master);
					  if (unit_usage>one_unit) scope_close (slave);
					  scope_finish();
					  exit(0);
				  }

				  stop_sync = 0;		//release sync
			  }
			  else
			  {
				  ;	//we are in the middle of closing down
			  }
		  }

//********************************************************************************************
private: System::Void Intf_source_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		 {
         Interface_Source =Intf_source->SelectedIndex;
    	cscope_interface(Cscope_index, Interface_Source, Interface_address);
		 }

//**************************************************************************************
private: System::Void IP_address_TextChanged(System::Object^  sender, System::EventArgs^  e)
		 {
        Interface_address =  string_to_IP(IP_address->Text);
    	cscope_interface(Cscope_index, Interface_Source, Interface_address);
		 }

//*******************************************************************************
private: System::Void Cscope_unit_index_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		 {
		Cscope_index = Cscope_unit_index->SelectedIndex;
    	cscope_interface(Cscope_index, Interface_Source, Interface_address);
		 }
//*******************************************************************************


private: System::Void Trigger_channel_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		 {
		trig_channel = Trigger_channel->SelectedIndex;
		vals_changed = true;
		 }
//*******************************************************************************

private: System::Void Restart_Click(System::Object^  sender, System::EventArgs^  e)
		 {
		 cscope_interface(Cscope_index, Interface_Source, Interface_address);
		 }
//*******************************************************************************


private: System::Void Cmd_get_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
		 SetReal(Cmd_value, get_function(Cscope_index, functions,calibration), " ");
		 if (functions == Calibrate)
			{
			if (!((calibration == Idle) || (calibration == SaveHWValues) || (calibration == SetRef)))
				{
				f_calibrating = true;
				calibration = DoCalibrate;
				CalibrateStep->SelectedIndex = DoCalibrate;
				}
			}
		 }

private: System::Void Cmd_Function_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 functions = (e_functions) Cmd_Function->SelectedIndex;
		 }

private: System::Void CalibrateStep_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 calibration = (e_calibration) CalibrateStep->SelectedIndex;
		 }


private: System::Void TCenter_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
 			horz_center = ((Double) (TCenter->Value))*0.001;
			zedGraphControl1->GraphPane->XAxis->Scale->Min = horz_center - (horz*t_divisions/2);
			zedGraphControl1->GraphPane->XAxis->Scale->Max = horz_center + (horz*t_divisions/2);
			zedGraphControl1->GraphPane->AxisChange();
			zedGraphControl1->Invalidate();
			vals_changed = true;

		 }

private: System::Void ACenter_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
		a_vert_center =  ((Double) (ACenter->Value));
		vals_changed = true;
		zedGraphControl1->GraphPane->YAxis->Scale->Min = a_vert_center - (a_vert*v_divisions/2);
		zedGraphControl1->GraphPane->YAxis->Scale->Max = a_vert_center + (a_vert*v_divisions/2) ;
		zedGraphControl1->GraphPane->AxisChange();
		zedGraphControl1->Invalidate();
		 }
private: System::Void BCenter_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
		b_vert_center =  ((Double) (BCenter->Value));
		vals_changed = true;
		zedGraphControl1->GraphPane->Y2Axis->Scale->Min = b_vert_center - (b_vert*v_divisions/2);
		zedGraphControl1->GraphPane->Y2Axis->Scale->Max = b_vert_center + (b_vert*v_divisions/2);
		zedGraphControl1->GraphPane->AxisChange();
		zedGraphControl1->Invalidate();
		 }
private: System::Void NumSamples_ValueChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 num_points = ((int32) NumSamples->Value);
			 vals_changed= true;
		 }
private: System::Void label29_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void ClockSource_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
		clock_source = ClockSource->SelectedIndex;
		vals_changed = true;
		 }
private: System::Void Num_units_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
		 _int32 old_unit_usage = unit_usage;
		 _int32 status;
		unit_usage = Num_units->SelectedIndex;
		if ((old_unit_usage == one_unit) && (unit_usage>one_unit)) status =scope_init (slave);
		vals_changed = true;
		 }
};
}

