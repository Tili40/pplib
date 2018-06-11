/*
  ppInvertMatrix

  written by Podoroges 11/06/2016

  Invert Square Matrix

  Usage example:

  ppInvertMatrix m;
  m.LoadTestValues();
  MLog->Clear();
  MLog->Lines->Add(m.AsString());
  m.Invert();
  MLog->Lines->Add(m.AsString());

  Output:
[  1.00  -1.00   3.00 ]
[  2.00   1.00   2.00 ]
[ -2.00  -2.00   1.00 ]

[  1.00  -1.00  -1.00 ]
[ -1.20   1.40   0.80 ]
[ -0.40   0.80   0.60 ]

*/
class ppInvertMatrix{
  private:
  int pSize;
  std::vector <double> pM;
  double __fastcall pGetM(int i,int j){
    return pM[i+j*pSize];
  }
  void __fastcall pSetM(int i,int j,double value){
    assert(i+j*pSize<pM.size());
    pM[i+j*pSize] = value;
  }
  int __fastcall pGetSize(){
    return pSize;
  }
  void __fastcall pSetSize(int size){
    while(pM.size()<(unsigned int)size*size)
      pM.push_back(0);
    while(pT.size()<(unsigned int)size*size)
      pT.push_back(0);
    pSize = size;
  }
  void SwapRows(int i,int j){
    double t;
    for(int a=0;a<pSize;a++){
      t = M[a][i];
      M[a][i] = M[a][j];
      M[a][j] = t;
    }
  }

  // + Temp matrix
  std::vector <double> pT;
  double __fastcall pGetT(int i,int j){
    return pT[i+j*pSize];
  }
  void __fastcall pSetT(int i,int j,double value){
    assert(i+j*pSize<pT.size());
    pT[i+j*pSize] = value;
  }
  __property double T[int i][int j] = {read=pGetT,write=pSetT};
  void IdentT(){
    for(int i=0;i<pSize;i++)
    for(int j=0;j<pSize;j++)
      T[i][j] = (i==j);
  }
  void TtoM(){
    for(int i=0;i<pSize;i++)
    for(int j=0;j<pSize;j++)
      M[i][j] = T[i][j];
  }
  // - Temp matrix
  public:

  __property int Size = {read=pGetSize,write=pSetSize};
  __property double M[int i][int j] = {read=pGetM,write=pSetM};

  void Invert(){
    IdentT(); // Now 1-diagonal in T
    for(int i = 0;i<pSize;i++){// Fw
      double c1 = 1.0/M[i][i];
      for(int j = 0;j<pSize;j++){
        M[j][i] = M[j][i]*c1;
        T[j][i] = T[j][i]*c1;
      }
      for(int k = i+1;k<pSize;k++){
        double c2 = M[i][k];
        for(int j = 0;j<pSize;j++){
          M[j][k] = M[j][k]-c2*M[j][i];
          T[j][k] = T[j][k]-c2*T[j][i];
        }
      }
    }
    for(int i = pSize-1;i>=0;i--){// Bw
      for(int k = i-1;k>=0;k--){
        double c2 = M[i][k];
        for(int j = 0;j<pSize;j++){
          M[j][k] = M[j][k]-c2*M[j][i];
          T[j][k] = T[j][k]-c2*T[j][i];
        }
      }
    }
    TtoM();
  }
  AnsiString AsString(){
    AnsiString st;
    TStringList * str = new TStringList();
    for(int j=0;j<pSize;j++){
      st = "[";
      for(int i=0;i<pSize;i++)
        st = st+AnsiString().sprintf(" % 0.2f ",M[i][j]);
      str->Add(st+"]");
    }
    st = str->Text;
    delete str;
    return st;
  }
  AnsiString AsStringT(){
    AnsiString st;
    TStringList * str = new TStringList();
    for(int j=0;j<pSize;j++){
      st = "[";
      for(int i=0;i<pSize;i++)
        st = st+AnsiString().sprintf(" % 0.2f ",M[i][j]);
      st = st+" | ";
      for(int i=0;i<pSize;i++)
        st = st+AnsiString().sprintf(" % 0.2f ",T[i][j]);
      str->Add(st+"]");
    }
    st = str->Text;
    delete str;
    return st;
  }

  void LoadTestValues(){
    Size = 3;
    M[0][0] = 1;
    M[1][0] = -1;
    M[2][0] = 3;
    M[0][1] = 2;
    M[1][1] = 1;
    M[2][1] = 2;
    M[0][2] = -2;
    M[1][2] = -2;
    M[2][2] = 1;
  }
  void ZeroMatrix(){
    for(int i=0;i<pSize;i++)
    for(int j=0;j<pSize;j++)
      M[i][j] = 0;
  }

  ppInvertMatrix(){
    Size = 3;
    ZeroMatrix();
  }
  ~ppInvertMatrix(){
    pM.clear();
    pT.clear();
  }
};
