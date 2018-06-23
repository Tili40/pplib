#include "ppSimpleHttpServer.h"

#pragma link "ws2_32.lib"
#pragma comment(lib, "ws2_32.lib")

// CSocketInfo

ppSimpleHttpServer::CSocketInfo::CSocketInfo(SOCKET s){
  Socket = s;
  Writable = 0;
  BufferSize = 50; // 1024
  Buffer = new char[BufferSize];
  Output = NULL;
}

ppSimpleHttpServer::CSocketInfo::~CSocketInfo(){
  closesocket(Socket);
  delete [] Buffer;
}


ppSimpleHttpServer::CSocketInfo * ppSimpleHttpServer::GetSocketInfo(SOCKET s){
  for(unsigned int a=0;a<Sockets.size();a++)
    if(Sockets[a]->Socket==s)
      return Sockets[a];
  return NULL;
}

void ppSimpleHttpServer::CreateSocketInfo(SOCKET s){
  Sockets.push_back(new CSocketInfo(s));
}

void ppSimpleHttpServer::FreeSocketInfo(SOCKET s){
  int Found = -1;
  for(unsigned int a=0;a<Sockets.size();a++)
    if(Sockets[a]->Socket==s)
      Found = a;
  if(Found>=0){
    delete Sockets[Found];
    Sockets.erase(Sockets.begin()+Found);
  }
}


void ppSimpleHttpServer::CSocketInfo::SimpleFileResponse(AnsiString fname){
  TStringList * str = new TStringList();
  TFileStream * fs = new TFileStream(fname,fmOpenRead|fmShareDenyNone);
  str->Add("HTTP/1.1 200 OK");
  str->Add((AnsiString)"Content-Length: "+fs->Size);
  if(ExtractFileExt(fname).LowerCase()==AnsiString(".gif"))
    str->Add("Content-Type: image/gif");
  //todo content types
  str->Add("Connection: Closed");
  str->Add("");
  Output = new TMemoryStream();
  Output->Write(str->Text.c_str(),str->Text.Length());
  Output->CopyFrom(fs,0);
  delete fs;
  Output->Position = 0;
  delete str;
}


void ppSimpleHttpServer::CSocketInfo::SimpleHtmlResponse(AnsiString Content){
  TStringList * str = new TStringList();
  str->Add("HTTP/1.1 200 OK");
  str->Add((AnsiString)"Content-Length: "+Content.Length());
  str->Add("Content-Type: text/html");
  str->Add("Connection: Closed");
  str->Add("");
  str->Add(Content);
  Output = new TStringStream(str->Text);
  Output->Position = 0;
  delete str;
}




void ppSimpleHttpServer::ParseHTTPHeader(CSocketInfo * SocketInfo){
  if((SocketInfo->Input.Pos("\r\n\r\n"))&&(SocketInfo->Input.Pos(" HTTP"))){
    SocketInfo->Method = SocketInfo->Input.SubString(1,SocketInfo->Input.Pos(" ")-1);
    int posSpace = SocketInfo->Input.Pos(" ")+1;
    int posHttp = SocketInfo->Input.Pos(" HTTP");
    SocketInfo->Url = SocketInfo->Input.SubString(posSpace,posHttp-posSpace);
    HandleHTTPRequest(SocketInfo);
    if(SocketInfo->Output)
      PostMessage(hWnd, wMsg, SocketInfo->Socket, FD_WRITE);
    SocketInfo->Input = "";
  }
}

void ppSimpleHttpServer::HandleMessage(int Event,int Socket){
  if(WSAGETSELECTERROR(Event)){
    Log((AnsiString)"Socket failed with error "+WSAGETSELECTERROR(Event));
    FreeSocketInfo(Socket);
  }

  if(WSAGETSELECTEVENT(Event)==FD_ACCEPT){
    SOCKET Accept;
    if((Accept = accept(Socket, NULL, NULL)) == INVALID_SOCKET){
      Log((AnsiString)"accept() failed with error "+WSAGetLastError());
      return;
    }
    CreateSocketInfo(Accept);
    Log((AnsiString)"New connection: "+Accept);
    WSAAsyncSelect(Accept, hWnd, wMsg, FD_READ|FD_WRITE|FD_CLOSE);
  }

  if(WSAGETSELECTEVENT(Event)==FD_READ){
    CSocketInfo * SocketInfo = GetSocketInfo(Socket);
    SocketInfo->DataBuf.buf = SocketInfo->Buffer;
    SocketInfo->DataBuf.len = SocketInfo->BufferSize;
    DWORD Flags = 0;
    DWORD RecvBytes;
    if(WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR){
      if(WSAGetLastError() != WSAEWOULDBLOCK){
        Log((AnsiString)"WSARecv() failed with error "+WSAGetLastError());
        FreeSocketInfo(Socket);
        return;
      }
    }
    SocketInfo->Input = SocketInfo->Input+AnsiString(SocketInfo->Buffer,RecvBytes);
    Log((AnsiString)"RECV: "+RecvBytes+" "+AnsiString(SocketInfo->Buffer,RecvBytes));
    ParseHTTPHeader(SocketInfo);
  }
  if(WSAGETSELECTEVENT(Event)==FD_WRITE){
    DWORD SendBytes;
    CSocketInfo * SocketInfo = GetSocketInfo(Socket);
    if(SocketInfo->Output){
      Log((AnsiString)"SocketInfo->Output->Size: "+SocketInfo->Output->Size+" Pos:"+SocketInfo->Output->Position);
      SocketInfo->DataBuf.len = std::min(SocketInfo->Output->Size-SocketInfo->Output->Position,SocketInfo->BufferSize);
      SocketInfo->Output->ReadBuffer(SocketInfo->DataBuf.buf,SocketInfo->DataBuf.len);
      if(WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SendBytes, 0, NULL, NULL) == SOCKET_ERROR){
        if(WSAGetLastError() != WSAEWOULDBLOCK){
          Log((AnsiString)"WSASend() failed with error "+WSAGetLastError());
          FreeSocketInfo(Socket);
          return;
        }
      }
      Log((AnsiString)"Bytes sent: "+SendBytes+" Pos:"+SocketInfo->Output->Position);

      if(SocketInfo->Output->Position<SocketInfo->Output->Size){
        PostMessage(hWnd, wMsg, SocketInfo->Socket, FD_WRITE);
      }
      else{
        delete SocketInfo->Output;
        SocketInfo->Output = NULL;
      }


      //SocketInfo->Output.Delete(1,SendBytes);
      //if(SocketInfo->Output.Length()>0)
      //  PostMessage(hWnd, wMsg, SocketInfo->Socket, FD_WRITE);
    }
  }

  if(WSAGETSELECTEVENT(Event)==FD_CLOSE){
    Log((AnsiString)"Closing socket "+Socket);
    FreeSocketInfo(Socket);
  }
}


void ppSimpleHttpServer::StartServer(){
  WSADATA wsaData;
  SOCKADDR_IN InternetAddr;
  if(WSAStartup((2,2), &wsaData) != 0)
    Log((AnsiString)"WSAStartup() failed with error "+WSAGetLastError());

  if((Listen = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    Log((AnsiString)"Listen socket() failed with error "+WSAGetLastError());

  if(WSAAsyncSelect(Listen, hWnd, wMsg, FD_ACCEPT|FD_CLOSE) != 0)
    Log((AnsiString)"WSAAsyncSelect() failed with error code "+ WSAGetLastError());

  InternetAddr.sin_family = AF_INET;
  InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  InternetAddr.sin_port = htons(Port);
  if(bind(Listen, (PSOCKADDR) &InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
    Log((AnsiString)"bind() failed with error "+WSAGetLastError());
  if(listen(Listen, 5)){//If no error occurs, listen returns zero
    Log((AnsiString)"listen() failed with error "+WSAGetLastError());
    return;
  }
  Log((AnsiString)"Server started on port "+Port);
}
ppSimpleHttpServer::ppSimpleHttpServer(int port,HWND hwnd,int wmsg){
  Port = port;
  hWnd = hwnd;
  wMsg = wmsg;
}

ppSimpleHttpServer::~ppSimpleHttpServer(){
  for(unsigned int a=0;a<Sockets.size();a++)
    delete Sockets[a];
  Sockets.clear();
  closesocket(Listen);
  WSACleanup();
}



