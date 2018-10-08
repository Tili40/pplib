#include "ppSearchMemo.h"

//------------------------------------------------------------------------------
__fastcall ppSearchMemo::ppSearchMemo(Classes::TComponent* Owner):TMemo(Owner){
  Parent = (TWinControl*)Owner;
  Font->Name = "Fixedsys";
  ScrollBars = ssVertical;
  Color = clWhite;
}
//------------------------------------------------------------------------------
__fastcall ppSearchMemo::~ppSearchMemo(){
  Position.clear();
}
//------------------------------------------------------------------------------
void ppSearchMemo::HighlightCancel(){
  for(unsigned int a=0;a<Position.size();a++)
    HighlightPart(Position[a],SearchString.Length(),Color);
}
//------------------------------------------------------------------------------
void ppSearchMemo::Paint(TWMPaint Event){
  Highlight();
  TMemo::Dispatch(&Event);
}
//------------------------------------------------------------------------------
void ppSearchMemo::Highlight(){
  for(unsigned int a=0;a<Position.size();a++)
    HighlightPart(Position[a],SearchString.Length(),
      (a+1==SearchCurrent?RGB(255,165,0):clYellow));
}
//------------------------------------------------------------------------------
void ppSearchMemo::SearchNext(){
  if(SearchCurrent<SearchCount)
    SearchCurrent++;
  else
    SearchCurrent = 1;
  SelStart = Position[SearchCurrent-1];
  SelLength = 0;
  Highlight();
}
//------------------------------------------------------------------------------
void ppSearchMemo::Search(AnsiString st){
  if(SearchString.Length()>0){
    HighlightCancel();
  }
  Position.clear();
  SearchCurrent = 0;
  SearchCount = 0;
  SearchString = st;
  if(st.Length()==0)
    return;
  AnsiString text = Lines->Text.UpperCase();
  int start = 0,found;
  do{
    found = text.Pos(st.UpperCase());
    if(found){
      SearchCount++;
      Position.push_back(start+found-1);
      start = start+found+st.Length()-1;
      text = text.Delete(1,found+st.Length()-1);
    }
  }
  while(found);
  if(SearchCount>0){
    SearchCurrent = 1;
    SelStart = Position[0];
    SelLength = 0;
    Highlight();
  }
}
//------------------------------------------------------------------------------
void ppSearchMemo::HighlightPart(int pos,int len,int cl){
  TSmallPoint Pt1;
  HDC hdc = GetDC(Handle);
  // clip to not to draw to non-text area (internal margins)
  TRect rect;
  SendMessage(Handle, EM_GETRECT, 0, int(&rect));
  HRGN ClipRgn = CreateRectRgn(rect.Left, rect.Top, rect.Right, rect.Bottom);
  SelectClipRgn(hdc, ClipRgn);
  DeleteObject(ClipRgn);
  // done with region
  int * pi = (int *)&Pt1;
  pi[0] = (int)SendMessage(Handle, EM_POSFROMCHAR, pos, 0);
  SelectObject(hdc, Font->Handle);
  SetBkColor(hdc,cl);
  AnsiString st = Lines->Text.SubString(pos+1,len);
  TextOutA(hdc,Pt1.x, Pt1.y,st.c_str(),len);
  ReleaseDC(Handle, hdc); // done with dc
}
//------------------------------------------------------------------------------
void __fastcall ppSearchMemo::Change(){
  Search("");
  Invalidate();
}
//------------------------------------------------------------------------------
