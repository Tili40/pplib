/*
  ppSearchMemo v1.2
  TMemo descendant with search/highlighting for BCB 6.0 by Podoroges

  by Podoroges
  Kiev, Ukraine

  v1.2 09/10/2018 - Corrected wrong array index in SearchNext.
  v1.1 08/10/2018 - Corrected buggy behaviour when text edited during search.
  v1.0 06/10/2018 - Start of the project

  Usage example:

void __fastcall TFMain::FormShow(TObject *Sender){
  MyEdit = new ppSearchMemo(FMain);
  MyEdit->Align = alClient;
  MyEdit->Color = clMaroon;
}

void __fastcall TFMain::FormHide(TObject *Sender){
  delete MyEdit;
}

void __fastcall TFMain::Edit1KeyDown(TObject *Sender, WORD &Key, TShiftState Shift){
  if(Key==VK_RETURN){
    MyEdit->SearchNext();
    Panel1->Caption = MyEdit->SearchPosition();
    return;
  }
}

void __fastcall TFMain::SpeedButton1Click(TObject *Sender){
  MyEdit->SearchPrev();
  Panel1->Caption = MyEdit->SearchPosition();
}

void __fastcall TFMain::SpeedButton2Click(TObject *Sender){
  MyEdit->SearchNext();
  Panel1->Caption = MyEdit->SearchPosition();
}

void __fastcall TFMain::Edit1Change(TObject *Sender){
  MyEdit->Search(Edit1->Text);
  Panel1->Caption = MyEdit->SearchPosition();
}

*/
#include <vector>
#include <stdctrls.hpp>

class ppSearchMemo : public TMemo{
  private:
  BEGIN_MESSAGE_MAP
    VCL_MESSAGE_HANDLER(WM_PAINT, TWMPaint, Paint);
  END_MESSAGE_MAP(TMemo);
  void Paint(TWMPaint Event);
  DYNAMIC void __fastcall Change();
  std::vector <int> Position;
  void HighlightCancel();
  void Highlight();
  void HighlightPart(int pos,int len,int cl);
  public:
  AnsiString SearchString;
  unsigned int SearchCurrent;
  unsigned int SearchCount;
  void Search(AnsiString st);
  void SearchNext();
  void SearchPrev();
  AnsiString SearchPosition();
  __fastcall ppSearchMemo(Classes::TComponent*);
  __fastcall ~ppSearchMemo();
};
