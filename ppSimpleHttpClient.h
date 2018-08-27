/*
  class ppSimpleHttpClient v1.4

  by Podoroges
  Kiev, Ukraine

  27/08/2018 v1.4 ContentLength, Progress() added
  23/08/2018 v1.3 Removed forgotten debug code
  22/08/2018 v1.2 PostSimpleForm and UrlEncode added
  14/08/2018 v1.1 PostSingleFile added
  24/06/2018 v1.0 Initial release

  Usage example:
//---------------------------------------------------------------------------
class Http1:public ppSimpleHttpClient{
  public:
  void __fastcall Progress(){
    FMain->Caption = (AnsiString)"Progress: "+ProgressPercent+"%";
  }
  void __fastcall DownloadReady(){
    TFileStream * fs = new TFileStream("C:\\TestWebClient\\1.jpg",fmCreate);
    fs->CopyFrom(Output,0);
    FMain->MLog->Lines->Add("Download finished.");
    if(fs->Size<ContentLength)
      FMain->MLog->Lines->Add("Incomplete");
    else
      FMain->MLog->Lines->Add("Download complete");

    delete fs;
  }
  void __fastcall SyncroLog(){
    FMain->MLog->Lines->Add(LogMessage);
  }
  Http1(AnsiString url):ppSimpleHttpClient(){
    this->Get(url);
  }
};
//---------------------------------------------------------------------------





void __fastcall TFMain::FormShow(TObject *Sender)
{
  new Http1("http://127.0.0.1/download.jpg");
  //new Http1("http://www.sonaca-aircraft.com/sites/default/files/product_design/DSC01460.JPG");

  //new Http1("http://www.antonov.kiev.ua/booklet2018.pdf");

}
//---------------------------------------------------------------------------

*/

#include <classes.hpp>
class ppSimpleHttpClient:public TThread{
  private:
  AnsiString Url;
  AnsiString ExtractString1(AnsiString,AnsiString,AnsiString);
  TStream * Content;
  int HeadersParsed;
  void ParseHTTPHeaders(TStream * ms);
  int ContentStart;
  public:
  TStream * Output;
  AnsiString Headers;
  AnsiString ContentType;
  int ContentLength;
  AnsiString LogMessage;
  AnsiString Method;
  int ProgressPercent;

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
  virtual void __fastcall Progress() = 0;
  void __fastcall Execute();

};


