#include <classes.hpp>
//---------------------------------------------------------------------------
// Extract n-th word, starting with '0'
// If n more than words count, last word returned
// StrParm("Magic The Gathering",1," ") = "The"
AnsiString StrParm(AnsiString st,int n,AnsiString sample){
  while(n>0){
    st = st.Delete(1,st.Pos(sample)+sample.Length()-1);
    n--;
  }
  st = st.Delete(st.Pos(sample),st.Length()-st.Pos(sample)+1);
  return st;
}
//---------------------------------------------------------------------------
//  StrCnt("A+B+C","+") = 2
int StrCnt(AnsiString st,AnsiString sample){
  int a=0;
  while(st.Pos(sample)>0){
    st=st.Delete(st.Pos(sample),sample.Length());
    a++;
  }
  return a;
}
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

