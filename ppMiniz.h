/*

  ppMiniz.h v1.0

  by Podoroges
  Kiev, Ukraine

  18/09/2018 v1.0 Initial release

  Lame wrapper around miniz (https://code.google.com/archive/p/miniz/)
  No error checking, no optimization.

  Usage example:

  TStringStream * s1 = new TStringStream("Good morning Dr. Chandra. This is Hal. I am ready for my first lesson.");
  TStringStream * s2 = new TStringStream("");
  TStringStream * s3 = new TStringStream("");

  ppMinizPack(s1,s2);
  ppMinizUnpack(s2,s3,s1->Size);

  ShowMessage(s3->DataString);

  delete s1;
  delete s2;
  delete s3;

*/

/*
  ppMinizPack
  First parameter - source srtream with data
  Second parameter - empty stream
*/
void ppMinizPack(class TStream *,class TStream *);

/*
  ppMinizUnpack
  First parameter - source srtream with packed data
  Second parameter - empty stream
  Third parameter - size of unpacked data. We have to store this value among packed data.
*/
void ppMinizUnpack(class TStream *,class TStream *,unsigned long);
