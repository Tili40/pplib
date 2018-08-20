/*
  ppGdiPlus - GdiPlus wrapper for BCB 6.0 by Podoroges
  v0.06 20/08/2018 - Added pInstanceCounter to allow several instances at once
                     pbmp included into class ppGdiPlus
  v0.05 22/06/2018 - Added GetInternalBitmap
  v0.04 12/06/2018 - Added TextOut, FontSize
  v0.03 23/05/2018 - Added DrawCircle, FillCircle, Alpha
  v0.02 22/05/2018 - Added Polygon, Polyline, FillEllipse
  v0.01 30/03/2018 - Start of the project

  In case of following error:
  [C++ Error] GdiplusGraphics.h(34): E2015 Ambiguity between
  'Gdiplus::Graphics::Graphics(void *)' and 'Gdiplus::Graphics::Graphics(void *,int)'
  FIX: add ";STRICT" into Project->Options->Conditional->Conditional defines
*/
#include <graphics.hpp> // TColor

class ppGdiPlus{
private:
  void * _pbmp; // Internal bitmap
  static int pInstanceCounter;
  int pWidth;
  int __fastcall pGetWidth();
  void __fastcall pSetWidth(int);
  int pHeight;
  int __fastcall pGetHeight();
  void __fastcall pSetHeight(int);
  void pResize();
  TColor pPenColor;
  TColor __fastcall pGetPenColor();
  void __fastcall pSetPenColor(TColor);
  double pPenWidth;
  double __fastcall pGetPenWidth();
  void __fastcall pSetPenWidth(double);
  TColor pBrushColor;
  TColor __fastcall pGetBrushColor();
  void __fastcall pSetBrushColor(TColor);
  double pAlpha;
  double __fastcall pGetAlpha();
  void __fastcall pSetAlpha(double);
  int pFontSize;
  int __fastcall pGetFontSize();
  void __fastcall pSetFontSize(int);

public:

  __property int Width = {read=pGetWidth,write=pSetWidth};
  __property int Height = {read=pGetHeight,write=pSetHeight};
  void Resize(int,int);
  __property TColor PenColor = {read=pGetPenColor,write=pSetPenColor};
  __property double PenWidth = {read=pGetPenWidth,write=pSetPenWidth};
  __property TColor BrushColor = {read=pGetBrushColor,write=pSetBrushColor};
  __property double Alpha = {read=pGetAlpha,write=pSetAlpha};
  __property int FontSize = {read=pGetFontSize,write=pSetFontSize};

  void Line(int,int,int,int);
  void Rect(int,int,int,int);
  void DrawEllipse(int,int,int,int);
  void FillEllipse(int,int,int,int);
  void DrawCircle(double,double,double);
  void FillCircle(double,double,double);
  void FillRect(int,int,int,int);
  void Draw(TCanvas *,int = 0,int = 0);
  void Clear(TColor);
  void Polyline(TPoint *,int);
  void Polygon(TPoint *,int);
  void DrawImage(void *,float,float,float,float);
  void TextOut(double,double,AnsiString);
  void * GetInternalBitmap();

  ppGdiPlus(int = 100,int = 100); // Constr
  ~ppGdiPlus();       // Destr
};
