//---------------------------------------------------------------------------
#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <vcl\Classes.hpp>
#include <vcl\Controls.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TButton *InitButton;
	TButton *SetParamButton;
	TButton *OnButton;
	TButton *MoveButton;
	TButton *ReadButton;
	TButton *ExitButton;
	void __fastcall InitButtonClick(TObject *Sender);
	void __fastcall SetParamButtonClick(TObject *Sender);
	void __fastcall OnButtonClick(TObject *Sender);
	void __fastcall MoveButtonClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ExitButtonClick(TObject *Sender);
	void __fastcall ReadButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
