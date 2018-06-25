#include <classes.hpp>
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

