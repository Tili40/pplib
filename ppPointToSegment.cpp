#include <math.h>

double sqr(double x){
  return x*x;
}

double PointToSegment(double x0,double y0,double x1,double y1,double x2,double y2){
  // Legs squares
  double a=sqr(x0-x1)+sqr(y0-y1);
  double b=sqr(x0-x2)+sqr(y0-y2);
  // Base square
  double c=sqr(x2-x1)+sqr(y2-y1);
  if(c==0){ // If zero base, distance is one of the legs
    return sqrt(a);
  }
  // Trinagle area
  double s=fabs((x0-x2)*(y1-y2)-(x1-x2)*(y0-y2))/2;
  // Obtuse triangle check
  if((a+c<b)||(b+c<a)){
    if(a<b)
      return sqrt(a);
    return sqrt(b);
  }
  // Acute triangle - return height above Base
  return 2*s/sqrt(c); // c is not zero, checked above
}

