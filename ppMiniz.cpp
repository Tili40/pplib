#include <classes.hpp>

#define Byte zlib_Byte
#define MINIZ_NO_STDIO
#include "miniz.c"
#undef Byte


void ppMinizPack(class TStream * in,class TStream * out){
  unsigned long in_size = in->Size;
  char * in_buf = new char[in_size];
  in->Position = 0;
  in->Read(in_buf,in_size);
  unsigned long out_size = compressBound(in_size);
  char * out_buf = new char[out_size];
  compress(out_buf,&out_size,in_buf,in_size);
  out->Write(out_buf,out_size);
  out->Position = 0;
  delete [] in_buf;
  delete [] out_buf;
}

void ppMinizUnpack(class TStream * in,class TStream * out,unsigned long unp_size){
  unsigned long in_size = in->Size;
  char * in_buf = new char[in_size];
  in->Position = 0;
  in->Read(in_buf,in_size);
  char * out_buf = new char[unp_size];
  unsigned long out_size = unp_size;
  uncompress(out_buf,&out_size,in_buf,in_size);
  out->Write(out_buf,out_size);
  out->Position = 0;
  delete [] in_buf;
  delete [] out_buf;
}
