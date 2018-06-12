/*
  ppLinearSolve

  written by Podoroges 12/06/2016

  Solve Linear Equations

  Usage example:

  ppLinearSolve m;
  m.LoadTestValues();
  MLog->Clear();
  MLog->Lines->Add(m.AsString());
  m.Solve();
  MLog->Lines->Add(m.AsString());

  Output:
[  2.00   1.00  -1.00  |   8.00 ]
[ -3.00  -1.00   2.00  |  -11.00 ]
[ -2.00   1.00   2.00  |  -3.00 ]

[  1.00   0.00   0.00  |   2.00 ]
[  0.00   1.00   0.00  |   3.00 ]
[ -0.00  -0.00   1.00  |  -1.00 ]

*/
class ppLinearSolve{
  private:
  int pSize;
  std::vector <double> pM;
  std::vector <double> pV;
  double __fastcall pGetM(int i,int j){
    return pM[i+j*pSize];
  }
  void __fastcall pSetM(int i,int j,double value){
    assert(i+j*pSize<pM.size());
    pM[i+j*pSize] = value;
  }
  double __fastcall pGetV(int i){
    return pV[i];
  }
  void __fastcall pSetV(int i,double value){
    assert(i<pV.size());
    pV[i] = value;
  }

  int __fastcall pGetSize(){
    return pSize;
  }
  void __fastcall pSetSize(int size){
    while(pM.size()<(unsigned int)size*size)
      pM.push_back(0);
    while(pV.size()<(unsigned int)size)
      pV.push_back(0);
    pSize = size;
  }


  public:

  __property int Size = {read=pGetSize,write=pSetSize};
  __property double M[int i][int j] = {read=pGetM,write=pSetM};
  __property double V[int i] = {read=pGetV,write=pSetV};

  void Solve(){
    for(int i = 0;i<pSize;i++){// Fw
      double c1 = 1.0/M[i][i];
      for(int j = 0;j<pSize;j++)
        M[j][i] = M[j][i]*c1;
      V[i] = V[i]*c1;

      for(int k = i+1;k<pSize;k++){
        double c2 = M[i][k];
        for(int j = 0;j<pSize;j++)
          M[j][k] = M[j][k]-c2*M[j][i];
        V[k] = V[k]-c2*V[i];

      }
    }

    for(int i = pSize-1;i>=0;i--){// Bw
      for(int k = i-1;k>=0;k--){
        double c2 = M[i][k];
        for(int j = 0;j<pSize;j++)
          M[j][k] = M[j][k]-c2*M[j][i];
        V[k] = V[k]-c2*V[i];
      }
    }

  }
  AnsiString AsString(){
    AnsiString st;
    TStringList * str = new TStringList();
    for(int j=0;j<pSize;j++){
      st = "[";
      for(int i=0;i<pSize;i++)
        st = st+AnsiString().sprintf(" % 0.2f ",M[i][j]);
      st = st+" | ";
      st = st+AnsiString().sprintf(" % 0.2f ",V[j]);
      str->Add(st+"]");
    }
    st = str->Text;
    delete str;
    return st;
  }

  void LoadTestValues(){
    Size = 3;
    M[0][0] = 2;
    M[1][0] = 1;
    M[2][0] = -1;
    M[0][1] = -3;
    M[1][1] = -1;
    M[2][1] = 2;
    M[0][2] = -2;
    M[1][2] = 1;
    M[2][2] = 2;
    V[0] = 8;
    V[1] = -11;
    V[2] = -3;
  }
  void ZeroMatrix(){
    for(int i=0;i<pSize;i++)
    for(int j=0;j<pSize;j++)
      M[i][j] = 0;
  }

  ppLinearSolve(){
    Size = 3;
    ZeroMatrix();
  }
  ~ppLinearSolve(){
    pM.clear();
    pV.clear();
  }
};
