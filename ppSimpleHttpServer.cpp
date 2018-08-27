#include "ppSimpleHttpServer.h"

#pragma link "ws2_32.lib"
#pragma comment(lib, "ws2_32.lib")


//---------------------------------------------------------------------------
//  ExtractString1("archive1.zip","arc",".zip") = "hive1"
AnsiString ExtractString1(AnsiString source,AnsiString start,AnsiString end){
  if(!source.Pos(start))return "";
  source = source.Delete(1,source.Pos(start)+start.Length()-1);
  if(!source.Pos(end))return source;
  source = source.Delete(source.Pos(end),source.Length());
  return source;
}
//---------------------------------------------------------------------------
// Replace in "st" all "from" to "to" (BCC version, copyright Never)
AnsiString StrRep(AnsiString st,AnsiString from,AnsiString to){
  while(st.Pos(from)){
    int a = st.Pos(from);
    st.Delete(a,from.Length());
    st.Insert(to,a);
  }
  return st;
}
//---------------------------------------------------------------------------
/*
  And the answer is rather boring: you just do it. That's all.
  application/x/www-form-urlencoded only does two things:
  replace spaces with "+" signs, and replace most other kind of punctuation
  (including the real "+" sign itself) with %xx,
  where xx is the octet in hexadecimal.
  https://stackoverflow.com/questions/27525002/un-escape-string-received-via-post-data
*/
void UrlDecode(AnsiString &st){
  for(int a=1;a<=st.Length();a++){
    if(st[a]=='+')
      st[a] = ' ';
    if((st[a]=='%')&&(a<st.Length()-1)){
      char buf[3];
      buf[0] = st[a+1];
      buf[1] = st[a+2];
      buf[2] = 0;
      st[a] = strtol(buf, NULL, 16);
      st = st.Delete(a+1,2);
    }
  }
}
//---------------------------------------------------------------------------
// We are looking for pattern with length patlen in buf with length buflen
// starting from start. Returning first occurence, or -1 if not found
int memsearch(char * buf,int buflen,int start,char * pattern,int patlen){
  int found;
  for(int a=start;a<buflen-patlen+1;a++){
    found = 1;
    int b=0;
    while(b<patlen){
      if(buf[a+b]!=pattern[b]){
        found = 0;
        b = patlen;
      }
      b++;
    }
    if(found)
      return a;
  }
  return -1;
}
//---------------------------------------------------------------------------
//  ppSimpleHttpServer::CSocketInfo
//---------------------------------------------------------------------------
ppSimpleHttpServer::CSocketInfo::CSocketInfo(SOCKET s){
  Socket = s;
  Writable = 0;
  ContentLength = 0;
  AllDoneNowMayClose = 0;
  //BufferSize = 0x400; // 1024
  BufferSize = 100000;
  Buffer = new char[BufferSize];
  RawInput = new TMemoryStream();
  Output = NULL;
  Params = new TStringList();
  // Get Client IP
  // http://wiredrevolution.com/c/find-ip-address-from-remote-end-of-a-tcp-socket
  struct sockaddr_in sin;
  int len = sizeof(sin);
  if (getpeername(s, (struct sockaddr *) &sin, &len)==0){
    ClientIP = AnsiString(inet_ntoa(sin.sin_addr));
  }
}
//---------------------------------------------------------------------------
ppSimpleHttpServer::CSocketInfo::~CSocketInfo(){
  delete RawInput;
  delete Output;
  delete Params;
  closesocket(Socket);
  delete [] Buffer;
}
//---------------------------------------------------------------------------
AnsiString ppSimpleHttpServer::CSocketInfo::ParseMultipartFormData(int mode,AnsiString fieldname,AnsiString filename){
  // Universal routine for both ExtractFormFileName and ExtractFormFileAs
  AnsiString result = "";
  int size = RawInput->Size;
  char * buf = (char *)malloc(size);
  RawInput->Position = 0;
  RawInput->Read(buf,size);
  AnsiString boundary;
  if(ContentType.Pos("boundary="))
    boundary = ContentType.SubString(ContentType.Pos("boundary=")+9,ContentType.Length());
  if(boundary.Length()){
    // The encapsulation boundary is defined as a line consisting entirely of two hyphen
    // characters ("-", decimal code 45) followed by the boundary parameter value from the Content-Type header field.
    // https://www.w3.org/Protocols/rfc1341/7_2_Multipart.html
    boundary = (AnsiString)"--"+boundary;
    int p1;
    int p2 = -1;
    do{
      p1 = p2;
      p2 = memsearch(buf,size,(p2<0?0:p2+boundary.Length()),boundary.c_str(),boundary.Length());
      if((p1>=0)&&(p2>=0)){
        // Between p1 and p2 file candidate is located
        int p3 = memsearch(buf,size,p1,"\r\n\r\n",4);
        AnsiString st = AnsiString(buf+p1,p3-p1);
        AnsiString field = ExtractString1(st," name=\"","\"");
        if(field==fieldname){
          if(mode==1){
            result = ExtractString1(st,"filename=\"","\"");
          }
          if(mode==2){
            TFileStream * fs = new TFileStream(filename,fmCreate);
            fs->Write(buf+p3+4,p2-p3-6);// 6 = 2xCRLF+CRLF
            delete fs;
          }
        }
      }
    }
    while(p2>=0);
  }
  free(buf);
  return result;
}
//---------------------------------------------------------------------------
AnsiString ppSimpleHttpServer::CSocketInfo::ExtractFormFileName(AnsiString f1){
  return ParseMultipartFormData(1,f1,"");
}
//---------------------------------------------------------------------------
void ppSimpleHttpServer::CSocketInfo::ExtractFormFileAs(AnsiString f1,AnsiString fname){
  ParseMultipartFormData(2,f1,fname);
}
//---------------------------------------------------------------------------
ppSimpleHttpServer::CSocketInfo * ppSimpleHttpServer::GetSocketInfo(SOCKET s){
  for(unsigned int a=0;a<Sockets.size();a++)
    if(Sockets[a]->Socket==s)
      return Sockets[a];
  return NULL;
}
//---------------------------------------------------------------------------
//  ppSimpleHttpServer
//---------------------------------------------------------------------------
void ppSimpleHttpServer::CreateSocketInfo(SOCKET s){
  Sockets.push_back(new CSocketInfo(s));
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
void ppSimpleHttpServer::CSocketInfo::SimpleFileResponse(AnsiString fname){
  TStringList * str = new TStringList();
  TFileStream * fs = new TFileStream(fname,fmOpenRead|fmShareDenyNone);
  str->Add("HTTP/1.1 200 OK");
  str->Add((AnsiString)"Content-Length: "+fs->Size);
  // Unknown file type: application/octet-stream
  // http://www.rfc-editor.org/rfc/rfc2046.txt
  AnsiString ContentType = "application/octet-stream";
  if(ExtractFileExt(fname).LowerCase()==AnsiString(".gif"))
    ContentType = "image/gif";
  if(ExtractFileExt(fname).LowerCase()==AnsiString(".html"))
    ContentType = "text/html";
  str->Add((AnsiString)"Content-Type: "+ContentType);
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
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
void ppSimpleHttpServer::ParseHTTPHeader(CSocketInfo * SocketInfo){
  if((SocketInfo->Input.Pos("\r\n\r\n"))&&(SocketInfo->Input.Pos(" HTTP"))){
    SocketInfo->Method = SocketInfo->Input.SubString(1,SocketInfo->Input.Pos(" ")-1);
    int posSpace = SocketInfo->Input.Pos(" ")+1;
    int posHttp = SocketInfo->Input.Pos(" HTTP");
    SocketInfo->Url = SocketInfo->Input.SubString(posSpace,posHttp-posSpace);
    int thinkFinish = 1;

    if(SocketInfo->Input.Pos("Content-Length:")){
      SocketInfo->ContentLength = ExtractString1(SocketInfo->Input,"Content-Length:","\r\n").Trim().ToIntDef(0);
      if(SocketInfo->Input.Pos("Content-Type:")){
        SocketInfo->ContentType = ExtractString1(SocketInfo->Input,"Content-Type:","\r\n").Trim();
        Log("*** Content-Type:["+SocketInfo->ContentType+"]");
      }
      int HeaderLength = SocketInfo->Input.Pos("\r\n\r\n")+3;//+4-1
      int TotalLength = SocketInfo->Input.Length();
      Log((AnsiString)"*** ContentLength="+SocketInfo->ContentLength);
      Log((AnsiString)"*** HeaderLength="+HeaderLength);
      Log((AnsiString)"*** TotalLength="+TotalLength);

      if(TotalLength<SocketInfo->ContentLength+HeaderLength){
        Log((AnsiString)"*** Waiting.");
        thinkFinish = 0;
      }
    }

    if(thinkFinish){
      Log("*** Handling HTTP request.");
      //Content-Type:
      if(SocketInfo->ContentType==AnsiString("application/x-www-form-urlencoded")){
        // Parse params
        SocketInfo->UnparsedParams = SocketInfo->Input.SubString(SocketInfo->Input.Pos("\r\n\r\n")+4,SocketInfo->ContentLength);
        UrlDecode(SocketInfo->UnparsedParams);
        SocketInfo->Params->Text = StrRep(SocketInfo->UnparsedParams,"&","\r\n");
        Log((AnsiString)"Unparsed: ["+SocketInfo->UnparsedParams+"]");
        for(int a=0;a<SocketInfo->Params->Count;a++)
          Log((AnsiString)SocketInfo->Params->Names[a]+": ["+SocketInfo->Params->Values[SocketInfo->Params->Names[a]]+"]");
      }
      HandleHTTPRequest(SocketInfo); // Calling virtual function
      if(SocketInfo->Output)
        PostMessage(hWnd, wMsg, SocketInfo->Socket, FD_WRITE);
      SocketInfo->Input = "";
    }
  }
}
//---------------------------------------------------------------------------
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
    Log((AnsiString)"New connection: "+Accept+" IP:"+GetSocketInfo(Accept)->ClientIP);
    WSAAsyncSelect(Accept, hWnd, wMsg, FD_READ|FD_WRITE|FD_CLOSE);
  }

  if(WSAGETSELECTEVENT(Event)==FD_READ){
    CSocketInfo * SocketInfo = GetSocketInfo(Socket);
    if(!SocketInfo){
      Log("Closed socket on FD_READ");
      return;
    }
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
    SocketInfo->RawInput->Write(SocketInfo->Buffer,RecvBytes);
    Log((AnsiString)"RECV: "+RecvBytes+" "+AnsiString(SocketInfo->Buffer,RecvBytes));
    ParseHTTPHeader(SocketInfo);
  }
  if(WSAGETSELECTEVENT(Event)==FD_WRITE){
    DWORD SendBytes;
    CSocketInfo * SocketInfo = GetSocketInfo(Socket);
    if(!SocketInfo){
      Log("Closed socket on FD_WRITE");
      return;
    }
    if((SocketInfo->Output)&&(!SocketInfo->AllDoneNowMayClose)){
      //Log((AnsiString)"SocketInfo->Output->Size: "+SocketInfo->Output->Size+" Pos:"+SocketInfo->Output->Position);
      int ChunkSize = std::min(SocketInfo->Output->Size-SocketInfo->Output->Position,SocketInfo->BufferSize);
      SocketInfo->DataBuf.len = ChunkSize;
      SocketInfo->Output->Read(SocketInfo->DataBuf.buf,ChunkSize);
      if(WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SendBytes, 0, NULL, NULL) == SOCKET_ERROR){
        if(WSAGetLastError() != WSAEWOULDBLOCK){
          Log((AnsiString)"WSASend() failed with error "+WSAGetLastError());
          FreeSocketInfo(Socket);
          return;
        }
        else{
          Log("WSAEWOULDBLOCK");
          SocketInfo->Output->Position = SocketInfo->Output->Position - ChunkSize;
          return;
        }
      }
      Log((AnsiString)"Bytes sent: "+SendBytes+" Pos:"+SocketInfo->Output->Position);

      if(SocketInfo->Output->Position<SocketInfo->Output->Size){
        PostMessage(hWnd, wMsg, SocketInfo->Socket, FD_WRITE);
      }
      else{
        //FreeSocketInfo(SocketInfo->Socket);
        // todo: check
        /*closesocket(SocketInfo->Socket);
        delete SocketInfo->Output;
        SocketInfo->Output = NULL;  */
        SocketInfo->AllDoneNowMayClose = 1;
        Log("******* AllDoneNowMayClose ***********");
        // https://stackoverflow.com/questions/41855865/why-should-i-use-shutdown-before-closing-a-socket
        shutdown(SocketInfo->Socket,SD_SEND);
        PostMessage(hWnd, wMsg, SocketInfo->Socket, FD_CLOSE);
      }


      //SocketInfo->Output.Delete(1,SendBytes);
      //if(SocketInfo->Output.Length()>0)
      //  PostMessage(hWnd, wMsg, SocketInfo->Socket, FD_WRITE);
    }
  }

  if(WSAGETSELECTEVENT(Event)==FD_CLOSE){
    CSocketInfo * SocketInfo = GetSocketInfo(Socket);
    if(!SocketInfo){
      Log("Already closed socket on FD_CLOSE");
      return;
    }
    if(SocketInfo->AllDoneNowMayClose){
      Log((AnsiString)"Closing socket "+Socket);
      FreeSocketInfo(Socket);
    }
    else{
      Log((AnsiString)"FD_CLOSE received for socket "+Socket);
    }
  }
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
ppSimpleHttpServer::ppSimpleHttpServer(int port,HWND hwnd,int wmsg){
  Port = port;
  hWnd = hwnd;
  wMsg = wmsg;
}
//---------------------------------------------------------------------------
ppSimpleHttpServer::~ppSimpleHttpServer(){
  for(unsigned int a=0;a<Sockets.size();a++)
    delete Sockets[a];
  Sockets.clear();
  closesocket(Listen);
  WSACleanup();
}
//---------------------------------------------------------------------------



