/*
  class ppSimpleHttpClient v1.0

  by Podoroges
  Kiev, Ukraine

  24/06/2018 v1.0 Initial release

  Usage example:
//---------------------------------------------------------------------------
class Http1:public ppSimpleHttpClient{
  public:
  void __fastcall DownloadReady(){
    TFileStream * fs = new TFileStream("C:\\TestClient2\\1.gif",fmCreate);
    fs->CopyFrom(Output,0);
    delete fs;
  }
  void __fastcall SyncroLog(){
    FMain->MLog->Lines->Add(LogMessage);
  }
  Http1(AnsiString url):ppSimpleHttpClient(url){}
};

//---------------------------------------------------------------------------
void __fastcall TFMain::FormShow(TObject *Sender)
{
  new Http1("http://meta.ua/img/m/logo_t.gif");
}
//---------------------------------------------------------------------------
*/

#include <classes.hpp>
class ppSimpleHttpClient:public TThread{
  private:
  AnsiString Url;
  AnsiString ExtractString1(AnsiString,AnsiString,AnsiString);
  public:
  TStream * Output;
  AnsiString Headers;
  AnsiString ContentType;
  AnsiString LogMessage;

  void Log(AnsiString msg){
    LogMessage = msg;
    Synchronize(SyncroLog);
  }

  __fastcall ppSimpleHttpClient(AnsiString);
  __fastcall ~ppSimpleHttpClient();

  virtual void __fastcall DownloadReady() = 0;
  virtual void __fastcall SyncroLog() = 0;
  void __fastcall Execute();

};


