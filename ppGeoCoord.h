/*
  class ppGeoCoord v1.2
  Convert Gauss-Krueger (GenShtab) into CK42, WGS84 and back

  by Podoroges
  Kiev, Ukraine

  26/09/2018 v1.2 Added default values to pLat/pLon (50N24E)
  20/06/2018 v1.1 to/fromWGS84 and to/fromSK42bl added DMS versions
  15/06/2018 v1.0 Initial release

  Based on Magenta code:
  https://gis-lab.info/forum/viewtopic.php?t=7281
  Corrected errors according to GOST 51794-2001:
  http://gis-lab.info/docs/legislation.html

  Comment this line "#define USE_STD_STRING" if you dont want std::string

  Usage example:

  ppGeoCoord c;
  c.fromWGS84(50.35,30.38);
  std::cout << c.toSK42dms() << "\n";	
  std::cout << c.toWGS84dms() << "\n";	
  std::cout << c.toSK42xy() << "\n";	
  c.fromSK42xy(5542000,4644000); 
  std::cout << c.toSK42dms() << "\n";	
  std::cout << c.toWGS84dms() << "\n";	
  std::cout << c.toSK42xy() << "\n";	  
  double lat,lon;
  c.toWGS84(&lat,&lon);
  std::cout << lat << "\n";
  std::cout << lon << "\n";
  
  Output:
  
N50°21'00" E30°22'53"
N50°21'00" E30°22'47"
X:5583171 Y:6313651
N49°59'31" E23°00'29"
N49°59'30" E23°00'23"
X:5542000 Y:4644000
49.9919
23.0065
*/

#define USE_STD_STRING
//#define CONSOLE_STD_OUTPUT

#ifdef USE_STD_STRING
#include <string>
#endif

class ppGeoCoord{
  private:
  double pLat,pLon; // private Latitude/Longitude - Main internal representation in WGS84
  class d2{
    public:
    double d1,d2;
    d2(double f1,double f2):d1(f1),d2(f2){}
  };
  double Pi,ro,aP,alP,e2P,aW,alW,e2W,a,e2,da,de2,dx,dy,dz,wx,wy,wz,ms;
  double dB(double,double,double);
  double dL(double,double,double);
#ifdef USE_STD_STRING
  std::string ppGeoCoord::flo2dms(double,int);
#endif
  void flo2dms(double f,int *d,int *m,int *s);
  double dms2flo(int d,int m,int s);

  d2 fromSK42xyToSK42bl(d2);
  d2 fromSK42blToSK42xy(d2);
  d2 fromWGS84ToSK42bl(d2);
  d2 fromSK42blToWGS84(d2);
  
  public:

  void fromWGS84(double lat,double lon);
  void fromWGS84(int lat_d,int lat_m,int lat_s,int south,int lon_d,int lon_m,int lon_s,int west);
  void fromSK42xy(double x,double y);
  void fromSK42bl(double lat,double lon);
  void fromSK42bl(int lat_d,int lat_m,int lat_s,int south,int lon_d,int lon_m,int lon_s,int west);

  void toWGS84(double *lat,double *lon);
  void toWGS84(int *lat_d,int *lat_m,int *lat_s,int *south,int *lon_d,int *lon_m,int *lon_s,int *west);
  void toSK42bl(double *lat,double *lon);
  void toSK42bl(int *lat_d,int *lat_m,int *lat_s,int *south,int *lon_d,int *lon_m,int *lon_s,int *west);
  void toSK42xy(double *x,double *y);


#ifdef USE_STD_STRING
  std::string toWGS84dms();
  std::string toSK42dms();
  std::string toSK42xy();
#endif
  ppGeoCoord();
};