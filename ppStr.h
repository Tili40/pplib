/*
  Library ppStr v1.1

  by Podoroges
  Kiev, Ukraine

  31/07/2018 v1.1 Added StrCnt, StrParm
  25/06/2018 v1.0 Initial release

*/

// Extract n-th word, starting with '0'
// If n more than words count, last word returned
// StrParm("Magic The Gathering",1," ") = "The"
AnsiString StrParm(AnsiString st,int n,AnsiString sample = " ");

//  StrCnt("A+B+C","+") = 2
int StrCnt(AnsiString st,AnsiString sample = " ");

// ExtractString1("archive1.zip","arc",".zip") = "hive1"
AnsiString ExtractString1(AnsiString source,AnsiString start,AnsiString end);

// Replace in "st" all "from" to "to"
AnsiString StrRep(AnsiString st,AnsiString from,AnsiString to);
