/*
  class ppSimpleHttpServer v1.0

  by Podoroges
  Kiev, Ukraine

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
    if(SocketInfo->Url.Pos("/test/")==1){
      SocketInfo->Output = SimpleHTTPResponse("<html><body><h1>Test</h1><i>"+SocketInfo->Url+"</i></body></html>");
      return;
    }
    SocketInfo->Output = SimpleHTTPResponse("<html><body><h1>404</h1><i>"+SocketInfo->Url+"</i></body></html>");
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


#include <winsock2.h>
#include <classes.hpp>
#include <vector>

class ppSimpleHttpServer{
  private:
  class CSocketInfo{
    public:
    char * Buffer;
    int BufferSize;
    WSABUF DataBuf;
    SOCKET Socket;
    int Writable;
    AnsiString Input;
    AnsiString Output;
    AnsiString Url;
    AnsiString Method;
    CSocketInfo(SOCKET s);
    ~CSocketInfo();
  };
  SOCKET Listen;
  CSocketInfo * GetSocketInfo(SOCKET s);
  void CreateSocketInfo(SOCKET s);
  void FreeSocketInfo(SOCKET s);
  HWND hWnd; // Window handle which will accept socket message
  unsigned int wMsg; // Message number
  std::vector <CSocketInfo *> Sockets;
  public:
  int Port;
  virtual void Log(AnsiString) = 0;
  AnsiString SimpleHTTPResponse(AnsiString);
  virtual void HandleHTTPRequest(CSocketInfo * SocketInfo) = 0;
  void ParseHTTPHeader(CSocketInfo * SocketInfo);
  void HandleMessage(int Event,int Socket);
  void StartServer();
  ppSimpleHttpServer(int port,HWND hwnd,int wmsg);
  ~ppSimpleHttpServer();
};
