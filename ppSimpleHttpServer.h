/*
  class ppSimpleHttpServer v1.8

  by Podoroges
  Kiev, Ukraine

TODO: https://developer.mozilla.org/en-US/docs/Web/HTTP/Session

  17/09/2018 v1.8 Removed winsock2.h from header to fight BCB6 packing bug.
  27/08/2018 v1.7 Proper WSAEWOULDBLOCK handling
  23/08/2018 v1.6 AllDoneNowMayClose added to reflect FD_CLOSE
  14/08/2018 v1.5 CSocketInfo::ClientIP added
  13/08/2018 v1.4 ExtractFormFileName and ExtractFormFileAs added
  26/06/2018 v1.3 application/octet-stream added
  25/06/2018 v1.2 x-www-form-urlencoded and UrlDecode added
  23/06/2018 v1.1 SimpleFileResponse added
  21/06/2018 v1.0 Initial release

  Usage example:
  1. Put into Form.h, somewhere on top:
  #define WM_SOCKET (WM_USER + 1)

  2. Put into public section of Form class:
  void __fastcall SocketHandler(TMessage Message);
  class CServer * srv;
  BEGIN_MESSAGE_MAP
      MESSAGE_HANDLER(WM_SOCKET, TMessage, SocketHandler)
  END_MESSAGE_MAP(TForm)

  3. .cpp file, create your server implementation:
class CServer: public ppSimpleHttpServer{
  public:
  CServer(int port,HWND hwnd,int wmsg):
    ppSimpleHttpServer(port,hwnd,wmsg){};
  void Log(AnsiString st){
    FMain->MLog->Lines->Add(st);
  }
  void HandleHTTPRequest(CSocketInfo * SocketInfo){
    if(SocketInfo->Url.Pos("/1.gif")==1){
      SocketInfo->SimpleFileResponse("C:\\TestServer\\1.GIF");
      return;
    }
    if(SocketInfo->Url.Pos("/test/")==1){
      SocketInfo->SimpleHtmlResponse("<html><body><h1>Test</h1><i>"+SocketInfo->Url+"</i></body></html>");
      return;
    }
    SocketInfo->SimpleHtmlResponse("<html><body><h1>Test</h1><img src='1.gif'><i>"+SocketInfo->Url+"</i></body></html>");
  }
};
  4. .cpp file, form events:
//---------------------------------------------------------------------------
void __fastcall TFMain::SocketHandler(TMessage Msg){
  srv->HandleMessage(Msg.LParam,Msg.WParam);
}
//---------------------------------------------------------------------------
void __fastcall TFMain::FormShow(TObject *Sender)
{
  MLog->Clear();
  srv = new CServer(5050,Handle,WM_SOCKET);
  srv->StartServer();
}
//---------------------------------------------------------------------------
void __fastcall TFMain::FormHide(TObject *Sender)
{
  delete srv;
}
//---------------------------------------------------------------------------



*/



#include <classes.hpp>
#include <vector>

class ppSimpleHttpServer{
  private:
  class CSocketInfo{
    friend ppSimpleHttpServer;
    private:
    char * Buffer;
    int BufferSize;
    unsigned long DataBuf_len;
    char * DataBuf_buf;
    int Socket;
    int Writable;
    CSocketInfo(int s);
    ~CSocketInfo();
    AnsiString ParseMultipartFormData(int,AnsiString,AnsiString);
    public:
    AnsiString ClientIP;
    TStream * RawInput;
    AnsiString Input;
    TStream * Output;
    AnsiString Url;
    AnsiString Method;
    AnsiString ContentType;
    AnsiString UnparsedParams;
    TStringList * Params;
    int AllDoneNowMayClose;
    int ContentLength;
    void SimpleHtmlResponse(AnsiString);
    void SimpleFileResponse(AnsiString);
    AnsiString ExtractFormFileName(AnsiString);
    void ExtractFormFileAs(AnsiString,AnsiString);
  };
  int Listen;
  CSocketInfo * GetSocketInfo(int s);
  void CreateSocketInfo(int s);
  void FreeSocketInfo(int s);
  HWND hWnd; // Window handle which will accept socket message
  unsigned int wMsg; // Message number
  std::vector <CSocketInfo *> Sockets;
  void ParseHTTPHeader(CSocketInfo * SocketInfo);
  public:
  int Port;
  virtual void Log(AnsiString) = 0;
  virtual void HandleHTTPRequest(CSocketInfo * SocketInfo) = 0;
  void HandleMessage(int Event,int Socket);
  void StartServer();
  ppSimpleHttpServer(int port,HWND hwnd,int wmsg);
  ~ppSimpleHttpServer();
};
