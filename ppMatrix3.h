
class ppMatrix3{
  private:
  void KramerSwap(int col){
    for(int i=0;i<Size;i++){
      double t = V[i];
      V[i] = M[col][i];
      M[col][i] = t;
    }
  }

  public:
  int Size;// = 3;
  double M[3][3];
  double V[3];

  int KramerSolve(){
    double d = Det();
    if(d==0)
      return 0;
    // 0
    KramerSwap(0);
    double d0 = Det();
    KramerSwap(0);
    // 1
    KramerSwap(1);
    double d1 = Det();
    KramerSwap(1);
    // 2
    KramerSwap(2);
    double d2 = Det();
    KramerSwap(2);
    // S
    V[0] = d0/d;
    V[1] = d1/d;
    V[2] = d2/d;
    return 1;
  }

  double Det(){
    return
       M[0][0]*M[1][1]*M[2][2]
      +M[0][1]*M[1][2]*M[2][0]
      +M[0][2]*M[1][0]*M[2][1]
      -M[0][2]*M[1][1]*M[2][0]
      -M[0][1]*M[1][0]*M[2][2]
      -M[0][0]*M[1][2]*M[2][1];
  }

  void FillRow(int a,double a0,double a1,double a2,double v){
    M[0][a] = a0;
    M[1][a] = a1;
    M[2][a] = a2;
    V[a] = v;
  }

  AnsiString AsString(){
    AnsiString st;
    TStringList * str = new TStringList();
    for(int j=0;j<Size;j++){
      st = "[";
      for(int i=0;i<Size;i++)
        st = st+AnsiString().sprintf(" % 7.2f ",M[i][j]);
      st = st+" | ";
      st = st+AnsiString().sprintf(" % 7.2f ",V[j]);
      str->Add(st+"]");
    }
    st = str->Text;
    delete str;
    return st;
  }
  AnsiString SolutionString(){
    AnsiString st = "[";
    for(int i=0;i<Size;i++)
      st = st+AnsiString().sprintf(" % 7.2f ",V[i]);
    return st+"]";
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
    for(int i=0;i<Size;i++)
    for(int j=0;j<Size;j++)
      M[i][j] = 0;
    for(int i=0;i<Size;i++)
      V[i] = 0;
  }
  ppMatrix3(){
    Size = 3;
    ZeroMatrix();
  }


};