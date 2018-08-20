#include "ppgdiplus.h"
#pragma link "gdiplus.lib"
#include <algorithm>
using std::min;
using std::max;
#include <classes.hpp>
#include <wingdi.h> // RGB
#include <gdiplus.h>

/*
void DrawRoundRectangle(Gdiplus::Graphics* g, Gdiplus::Pen *p, Gdiplus::Rect& rect, int radius)
{
	if (g==NULL) return;
	Gdiplus::GraphicsPath path;

	path.AddLine(rect.X + radius, rect.Y, rect.X + rect.Width - (radius * 2), rect.Y);
	path.AddArc(rect.X + rect.Width - (radius * 2), rect.Y, radius * 2, radius * 2, 270, 90);
	path.AddLine(rect.X + rect.Width, rect.Y + radius, rect.X + rect.Width, rect.Y + rect.Height - (radius * 2));
	path.AddArc(rect.X + rect.Width - (radius * 2), rect.Y + rect.Height - (radius * 2), radius * 2,
		radius * 2, 0, 90);
	path.AddLine(rect.X + rect.Width - (radius * 2), rect.Y + rect.Height, rect.X + radius, rect.Y + rect.Height);
	path.AddArc(rect.X, rect.Y + rect.Height - (radius * 2), radius * 2, radius * 2, 90, 90);
	path.AddLine(rect.X, rect.Y + rect.Height - (radius * 2), rect.X, rect.Y + radius);
	path.AddArc(rect.X, rect.Y, radius * 2, radius * 2, 180, 90);
	path.CloseFigure();
	g->DrawPath(p, &path);
}

void FillRoundRectangle(Gdiplus::Graphics* g, Gdiplus::Brush *p, Gdiplus::Rect& rect, int radius)
{
	if (g == NULL) return;
	Gdiplus::GraphicsPath path;

	path.AddLine(rect.X + radius, rect.Y, rect.X + rect.Width - (radius * 2), rect.Y);
	path.AddArc(rect.X + rect.Width - (radius * 2), rect.Y, radius * 2, radius * 2, 270, 90);
	path.AddLine(rect.X + rect.Width, rect.Y + radius, rect.X + rect.Width, rect.Y + rect.Height - (radius * 2));
	path.AddArc(rect.X + rect.Width - (radius * 2), rect.Y + rect.Height - (radius * 2), radius * 2, radius * 2, 0, 90);
	path.AddLine(rect.X + rect.Width - (radius * 2), rect.Y + rect.Height, rect.X + radius, rect.Y + rect.Height);
	path.AddArc(rect.X, rect.Y + rect.Height - (radius * 2), radius * 2, radius * 2, 90, 90);
	path.AddLine(rect.X, rect.Y + rect.Height - (radius * 2), rect.X, rect.Y + radius);
	path.AddArc(rect.X, rect.Y, radius * 2, radius * 2, 180, 90);
	path.CloseFigure();

	g->FillPath(p, &path);
}



*/

int ppGdiPlus::pInstanceCounter = 0;

// Two variables required for startup
Gdiplus::GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR  gdiplusToken;


Gdiplus::Bitmap * pbmp(void * p){
  return (Gdiplus::Bitmap *)p;
}

void * ppGdiPlus::GetInternalBitmap(){
  return (void *)pbmp;
}


// Constructor
ppGdiPlus::ppGdiPlus(int Width,int Height){
  pWidth = Width;
  pHeight = Height;
  if(pInstanceCounter<1){
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
  }
  _pbmp = (void *)Gdiplus::new Gdiplus::Bitmap(pWidth,pHeight);
  pPenColor = clBlack;
  pPenWidth = 1;
  FontSize = 10;
  pAlpha = 0;
  pInstanceCounter++;
}
// Destructor
ppGdiPlus::~ppGdiPlus(){
  Gdiplus::delete pbmp(_pbmp);
  pInstanceCounter--;
  if(pInstanceCounter<1){
    Gdiplus::GdiplusShutdown(gdiplusToken);
  }
}
// Resize
void ppGdiPlus::pResize(){
  Gdiplus::delete pbmp(_pbmp);
  _pbmp = (void*)Gdiplus::new Gdiplus::Bitmap(pWidth,pHeight);
}
void ppGdiPlus::Resize(int w,int h){
  pWidth = w;
  pHeight = h;
  pResize();
}

int __fastcall ppGdiPlus::pGetWidth(){
  return pWidth;
}
int __fastcall ppGdiPlus::pGetHeight(){
  return pHeight;
}
void __fastcall ppGdiPlus::pSetWidth(int i){
  pWidth = i;
}
void __fastcall ppGdiPlus::pSetHeight(int i){
  pHeight = i;
}
// PenColor
TColor __fastcall ppGdiPlus::pGetPenColor(){
  return pPenColor;
}
void __fastcall ppGdiPlus::pSetPenColor(TColor c){
  pPenColor = c;
}
// PenWidth
double __fastcall ppGdiPlus::pGetPenWidth(){
  return pPenWidth;
}
void __fastcall ppGdiPlus::pSetPenWidth(double d){
  pPenWidth = d;
}
// BrushColor
TColor __fastcall ppGdiPlus::pGetBrushColor(){
  return pBrushColor;
}
void __fastcall ppGdiPlus::pSetBrushColor(TColor c){
  pBrushColor = c;
}
// Alpha
double __fastcall ppGdiPlus::pGetAlpha(){
  return pAlpha;
}
void __fastcall ppGdiPlus::pSetAlpha(double d){
  pAlpha = d;
}
// FontSize
int __fastcall ppGdiPlus::pGetFontSize(){
  return pFontSize;
}
void __fastcall ppGdiPlus::pSetFontSize(int s){
  pFontSize = s;
}

// Line
void ppGdiPlus::Line(int x1,int y1,int x2,int y2){
  Gdiplus::Graphics g(pbmp(_pbmp));
  Gdiplus::Pen pen(Gdiplus::Color(GetRValue(pPenColor), GetGValue(pPenColor), GetBValue(pPenColor)),(float)pPenWidth);
  g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
  g.DrawLine(&pen, x1, y1, x2, y2);
}
// Draw
void ppGdiPlus::Draw(TCanvas * c,int x,int y){
  Gdiplus::Graphics g2(c->Handle);
  g2.DrawImage(pbmp(_pbmp),x,y);
}
// Clear
void ppGdiPlus::Clear(TColor c){
  Gdiplus::Graphics g(pbmp(_pbmp));
  g.Clear(Gdiplus::Color(GetRValue(c), GetGValue(c), GetBValue(c)));
}
// Rect
void ppGdiPlus::Rect(int x1,int y1,int x2,int y2){
  Gdiplus::Graphics g(pbmp(_pbmp));
  Gdiplus::Pen pen(Gdiplus::Color(GetRValue(pPenColor), GetGValue(pPenColor), GetBValue(pPenColor)),pPenWidth);
  g.DrawRectangle(&pen, x1, y1, x2-x1, y2-y1);
}
// FillRect
void ppGdiPlus::FillRect(int x1,int y1,int x2,int y2){
  Gdiplus::Graphics g(pbmp(_pbmp));
  Gdiplus::SolidBrush brush(Gdiplus::Color(pAlpha,GetRValue(pBrushColor), GetGValue(pBrushColor), GetBValue(pBrushColor)));
//  Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(pBrushColor), GetGValue(pBrushColor), GetBValue(pBrushColor)));
  g.FillRectangle(&brush, x1, y1, x2-x1, y2-y1);
}
// DrawEllipse
void ppGdiPlus::DrawEllipse(int x1,int y1,int x2,int y2){
  Gdiplus::Graphics g(pbmp(_pbmp));
  g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
  Gdiplus::Pen pen(Gdiplus::Color(GetRValue(pPenColor), GetGValue(pPenColor), GetBValue(pPenColor)),pPenWidth);
  g.DrawEllipse(&pen, x1, y1, x2-x1, y2-y1);
}
// FillEllipse
void ppGdiPlus::FillEllipse(int x1,int y1,int x2,int y2){
  Gdiplus::Graphics g(pbmp(_pbmp));
  g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
  Gdiplus::SolidBrush brush(Gdiplus::Color(GetRValue(pBrushColor), GetGValue(pBrushColor), GetBValue(pBrushColor)));
  g.FillEllipse(&brush, x1, y1, x2-x1, x2-x1);
}
// DrawCircle
void ppGdiPlus::DrawCircle(double x1,double y1,double r){
  Gdiplus::Graphics g(pbmp(_pbmp));
  g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
  Gdiplus::Pen pen(Gdiplus::Color(GetRValue(pPenColor), GetGValue(pPenColor), GetBValue(pPenColor)),pPenWidth);
  g.DrawEllipse(&pen, float(x1-r), float(y1-r), float(2*r), float(2*r));
}
// FillCircle
void ppGdiPlus::FillCircle(double x1,double y1,double r){
  Gdiplus::Graphics g(pbmp(_pbmp));
  Gdiplus::SolidBrush brush(Gdiplus::Color(pAlpha,GetRValue(pBrushColor), GetGValue(pBrushColor), GetBValue(pBrushColor)));
  g.FillEllipse(&brush, float(x1-r), float(y1-r), float(2*r), float(2*r));
}
// Polyline
void ppGdiPlus::Polyline(TPoint * p,int cnt){
  Gdiplus::Graphics g(pbmp(_pbmp));
  g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
  Gdiplus::Pen pen(Gdiplus::Color(GetRValue(pPenColor), GetGValue(pPenColor), GetBValue(pPenColor)),pPenWidth);
  g.DrawPolygon(&pen, (Gdiplus::Point*)p ,cnt);
}
// Polygon
void ppGdiPlus::Polygon(TPoint * p,int cnt){
  Gdiplus::Graphics g(pbmp(_pbmp));
  g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
  Gdiplus::SolidBrush brush(Gdiplus::Color(pAlpha,GetRValue(pBrushColor), GetGValue(pBrushColor), GetBValue(pBrushColor)));
  g.FillPolygon(&brush, (Gdiplus::Point*)p ,cnt);
}
// DrawImage
void ppGdiPlus::DrawImage(void * image,float x1,float y1,float x2,float y2){
  Gdiplus::Graphics g(pbmp(_pbmp));
  g.DrawImage((Gdiplus::Bitmap *)image,x1,y1,x2,y2);
}
// TextOut
void ppGdiPlus::TextOut(double x,double y,AnsiString st){
  WideString wst = WideString(st);
  Gdiplus::Graphics g(pbmp(_pbmp));
  Gdiplus::SolidBrush brush(Gdiplus::Color(pAlpha,GetRValue(pBrushColor), GetGValue(pBrushColor), GetBValue(pBrushColor)));
  Gdiplus::Font font(&Gdiplus::FontFamily(L"Arial"), pFontSize);
  Gdiplus::PointF p(x,y);
  g.DrawString((WCHAR *)wst.c_bstr(),wst.Length(),&font,p,&brush);
}







