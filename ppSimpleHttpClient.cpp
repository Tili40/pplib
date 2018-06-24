#include <winsock2.h>
#include <ws2tcpip.h>
#include "ppSimpleHttpClient.h"
#pragma link "ws2_32.lib"
#pragma comment(lib, "ws2_32.lib")
//---------------------------------------------------------------------------
//  ExtractString1("archive1.zip","arc",".zip") = "hive1"
AnsiString ppSimpleHttpClient::ExtractString1(AnsiString source,AnsiString start,AnsiString end){
  if(!source.Pos(start))return "";
  source = source.Delete(1,source.Pos(start)+start.Length()-1);
  if(!source.Pos(end))return source;
  source = source.Delete(source.Pos(end),source.Length());
  return source;
}
//---------------------------------------------------------------------------
  __fastcall ppSimpleHttpClient::ppSimpleHttpClient(AnsiString url): TThread(0){
    FreeOnTerminate = 1;
    Url = url;
    Output = NULL;
  }
//---------------------------------------------------------------------------
  __fastcall ppSimpleHttpClient::~ppSimpleHttpClient(){
    delete Output;
    Log((AnsiString)"Thread ["+int(this)+"] released.");
  }
//---------------------------------------------------------------------------
  void __fastcall ppSimpleHttpClient::Execute(){
    // 1. Trying to parse Url: {Service}://{Server}/{Path}
    AnsiString Service = "http";
    AnsiString Server = Url;
    AnsiString Path = "/";
    int p1 = Url.Pos("://");
    if(p1>0){
      Service = Url.SubString(1,p1-1);
      Server = Url.SubString(p1+3,Url.Length());
    }
    p1 = Server.Pos("/");
    if(p1>0){
      Path = Server.SubString(p1,Server.Length());
      Server = Server.SubString(1,p1-1);
    }
    Log((AnsiString)"Service:["+Service+"]");
    Log((AnsiString)"Server:["+Server+"]");
    Log((AnsiString)"Path:["+Path+"]");


    struct addrinfo hints;
    struct addrinfo *result, *ptr;
    SOCKET Socket;

    WSADATA wsaData;
    if(WSAStartup((2,2), &wsaData) != 0)
      Log((AnsiString)"WSAStartup() failed: "+WSAGetLastError());

    // 2. Resolve hostname
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /* Any protocol */

    int res = getaddrinfo(Server.c_str(), Service.c_str(), &hints, &result);
    if(res!=0){
      if(res==11001)
        Log((AnsiString)"HOST_NOT_FOUND: "+Url);
      else
        Log((AnsiString)"Error getaddrinfo: "+ res);
    }
    else{
   /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully connect(2).
       If socket(2) (or connect(2)) fails, we (close the socket
       and) try the next address. */

      for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
          Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
          if (Socket == -1)
              continue;

          if (connect(Socket, ptr->ai_addr, ptr->ai_addrlen) != -1)
              break;                  /* Success */

          closesocket(Socket);
      }

      if (ptr == NULL) {               /* No address succeeded */
        Log("Cannot connect to host");
      }
      else{

        struct sockaddr_in * sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
        Log((AnsiString)"Connected to "+inet_ntoa(sockaddr_ipv4->sin_addr));

        // 3. Send Http Request
        TStringList * str = new TStringList;
        str->Add((AnsiString)"GET "+Path+" HTTP/1.1");
        str->Add((AnsiString)"Host: "+Server);
        str->Add("");


        AnsiString HttpRequest = str->Text;
        delete str;

        if(send( Socket, HttpRequest.c_str(), HttpRequest.Length(), 0 )== SOCKET_ERROR)
          Log((AnsiString)"Error send: "+ WSAGetLastError());


        if (shutdown(Socket, SD_SEND) == SOCKET_ERROR)
          Log((AnsiString)"Error shutdown: "+ WSAGetLastError());


        Log("HttpRequest sent.");

        // 4. Read response

        TMemoryStream * ms = new TMemoryStream();
        int iResult;
        do {
          const int buflen = 0x400;
          char buf[buflen];

          iResult = recv(Socket, buf, buflen, 0);
          if ( iResult > 0 ){
            Log((AnsiString)"Bytes received: "+ iResult);
            ms->Write(buf,iResult);
          }
          else if ( iResult == 0 )
            Log("Connection closed");
          else
            Log((AnsiString)"recv failed with error: "+ WSAGetLastError());

        } while( iResult > 0 );
        // 5. Parse result
        TStringStream * ss = new TStringStream("");
        ss->CopyFrom(ms,0);
        if((ss->DataString.Pos("HTTP")==1)&&(ss->DataString.Pos("\r\n\r\n"))){
          Log("Http headers found.");
          Headers = ss->DataString.SubString(1,ss->DataString.Pos("\r\n\r\n")).Trim();
          ContentType = ExtractString1(Headers,"Content-Type:","\r\n").Trim();
          Log(Headers);
          int contentStart = ss->DataString.Pos("\r\n\r\n")+3; //+4-1
          Log((AnsiString)"ContentType "+ContentType);
          Output = new TMemoryStream();
          ms->Position = contentStart;
          Output->CopyFrom(ms,ms->Size-ms->Position);
        }
        else{
          // Cannot find headers, take all stream
          Output = new TMemoryStream();
          Output->CopyFrom(ms,0);
        }
        delete ss;
        delete ms;
      }
      freeaddrinfo(result);           /* No longer needed */
      closesocket(Socket);
    }
    WSACleanup();
    Log((AnsiString)"Thread ["+int(this)+"] finished.");
    Synchronize(DownloadReady);
  }