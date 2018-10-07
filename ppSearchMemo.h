
/*
  ppSearchMemo - TMemo descendant with search/highlighting for BCB 6.0 by Podoroges
  v0.01 06/10/2018 - Start of the project

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
  AnsiString SearchString;
  std::vector <int> Position;
  void HighlightCancel();
  void Highlight();
  void HighlightPart(int pos,int len,int cl);
  public:
  unsigned int SearchCurrent;
  unsigned int SearchCount;
  void Search(AnsiString st);
  void SearchNext();
  void SearchPrev(){
    if(SearchCurrent>1){
      SearchCurrent--;
      SelStart = Position[SearchCurrent-1];
      SelLength = 0;
      Highlight();
    }
  }
  AnsiString SearchPosition(){
    return (AnsiString)SearchCurrent+"/"+SearchCount;
  }
  __fastcall ppSearchMemo(Classes::TComponent*);
  __fastcall ~ppSearchMemo();
};
