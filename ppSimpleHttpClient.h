/*
  class ppSimpleHttpClient v1.2

  by Podoroges
  Kiev, Ukraine

  22/08/2018 v1.2 PostSimpleForm and UrlEncode added
  14/08/2018 v1.1 PostSingleFile added
  24/06/2018 v1.0 Initial release

  Usage example:
//---------------------------------------------------------------------------
class Http1:public ppSimpleHttpClient{
  public:
  void __fastcall DownloadReady(){
    TStringStream * ss = new TStringStream("");
    ss->CopyFrom(Output,0);
    FMain->MLog->Lines->Add(ss->DataString);
    delete ss;
  }
  void __fastcall SyncroLog(){
    FMain->MLog->Lines->Add(LogMessage);
  }
  Http1(AnsiString url):ppSimpleHttpClient(){
    Get(url);
  }
};
//---------------------------------------------------------------------------
void __fastcall TFMain::FormShow(TObject *Sender)
{
  new Http1("http://meta.ua");
}
//---------------------------------------------------------------------------
*/

#include <classes.hpp>
class ppSimpleHttpClient:public TThread{
  private:
  AnsiString Url;
  AnsiString ExtractString1(AnsiString,AnsiString,AnsiString);
  TStream * Content;
  public:
  TStream * Output;
  AnsiString Headers;
  AnsiString ContentType;
  AnsiString LogMessage;
  AnsiString Method;

  void Log(AnsiString msg){
    LogMessage = msg;
    Synchronize(SyncroLog);
  }

  __fastcall ppSimpleHttpClient();
  __fastcall ~ppSimpleHttpClient();

  void Get(AnsiString);
  void PostSingleFile(AnsiString,AnsiString,AnsiString);
  void PostSimpleForm(AnsiString,AnsiString);

  virtual void __fastcall DownloadReady() = 0;
  virtual void __fastcall SyncroLog() = 0;
  void __fastcall Execute();

};


