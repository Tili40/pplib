#include "ppGeoCoord.h"
#include <stdio.h>
#include <math.h>

// SK42xy (Genstab) => SK42bl (Lat/Lon)
ppGeoCoord::d2 ppGeoCoord::fromSK42xyToSK42bl(d2 p){
  double x = p.d1;
  double y = p.d2;
  int No = y * pow( 10 , -6);
  double Bi = x / 6367558.4968; // Beta
  double Bo = Bi + sin(Bi * 2.0) * (0.00252588685 - 0.0000149186 * pow(sin(Bi) , 2) + 0.00000011904 * pow(sin(Bi) , 4));
  double Zo = (y - (10.0 * double(No) + 5.0) * 100000.0) / (6378245.0 * cos(Bo));
  double powSindBo2 = pow(sin(Bo) , 2.0);
  double powSindBo4 = pow(sin(Bo) , 4.0);
  double powSindBo6 = pow(sin(Bo) , 6.0);
  // Lat
  double Ba = Zo * Zo * (0.01672 - 0.0063 * powSindBo2 + 0.01188 * powSindBo4 - 0.00328 * powSindBo6);
  double Bb = Zo * Zo * (0.042858 - 0.025318 * powSindBo2 + 0.014346 * powSindBo4 - 0.001264 * powSindBo6 - Ba);
  double Bc = Zo * Zo * (0.10500614 - 0.04559916 * powSindBo2 + 0.00228901 * powSindBo4 - 0.00002987 * powSindBo6 - Bb);
  double dB = Zo * Zo * sin(Bo * 2.0) * (0.251684631 - 0.003369263 * powSindBo2 + 0.000011276 * powSindBo4 - Bc);
  p.d1 = (Bo - dB) * 180.0 / Pi;
  // Lon
  double La = Zo * Zo * (0.0038 + 0.0524 * powSindBo2 + 0.0482 * powSindBo4 + 0.0032 * powSindBo6);
  double Lb = Zo * Zo * (0.01225 + 0.09477 * powSindBo2 + 0.03282 * powSindBo4 - 0.00034 * powSindBo6 - La);
  double Lc = Zo * Zo * (0.0420025 + 0.1487407 * powSindBo2 + 0.005942 * powSindBo4 - 0.000015 * powSindBo6 - Lb);
  double Ld = Zo * Zo * (0.16778975 + 0.16273586 * powSindBo2 - 0.0005249 * powSindBo4 - 0.00000846 * powSindBo6 - Lc);
  double dL = Zo * (1.0 - 0.0033467108 * powSindBo2 - 0.0000056002 * powSindBo4 - 0.0000000187 * powSindBo6 - Ld);
  p.d2 = (6.0 * (double(No) - 0.5) / 57.29577951 + dL) * 180.0 / Pi;
  return p;
}

// SK42bl (Lat/Lon) => SK42xy (Genstab)
ppGeoCoord::d2 ppGeoCoord::fromSK42blToSK42xy(d2 p){
  double lat = p.d1;
  double lon = p.d2;
  double L = lon; // Longitude, degrees
  double Bo = lat * Pi / 180.0; // Latitude, radians
  int No = (6.0 + L) / 6.0;
  double Lo = (L - (3.0 + 6.0 * (double(No) - 1.0))) / 57.29577951;
  double powSindBo2 = pow(sin(Bo) , 2);
  double powSindBo4 = pow(sin(Bo) , 4);
  double powSindBo6 = pow(sin(Bo) , 6);
  double powLo2 = pow(Lo , 2);
  // X
  double Xa = powLo2 * (109500.0 - 574700.0 * powSindBo2 + 863700.0 * powSindBo4 - 398600.0 * powSindBo6);
  double Xb = powLo2 * (278194.0 - 830174.0 * powSindBo2 + 572434.0 * powSindBo4 - 16010.0 * powSindBo6 + Xa);
  double Xc = powLo2 * (672483.4 - 811219.9 * powSindBo2 + 5420.0 * powSindBo4 - 10.6 * powSindBo6 + Xb);
  double Xd = powLo2 * (1594561.25 + 5336.535 * powSindBo2 + 26.79 * powSindBo4 + 0.149 * powSindBo6 + Xc);
  p.d1 = 6367558.4968 * Bo - sin(Bo * 2) * (16002.89 + 66.9607 * powSindBo2 + 0.3515 * powSindBo4 - Xd);
  // Y
  double Ya = powLo2 * (79690.0 - 866190.0 * powSindBo2 + 1730360.0 * powSindBo4 - 945460.0 * powSindBo6);
  double Yb = powLo2 * (270806.0 - 1523417.0 * powSindBo2 + 1327645.0 * powSindBo4 - 21701.0 * powSindBo6 + Ya);
  double Yc = powLo2 * (1070204.16 - 2136826.66 * powSindBo2 + 17.98 * powSindBo4 - 11.99 * powSindBo6 + Yb);
  p.d2 = (5.0 + 10.0 * No) * 100000.0 + Lo * cos(Bo) * (6378245.0 + 21346.1415 * powSindBo2 + 107.159 * powSindBo4 + 0.5977 * powSindBo6 + Yc);
  return p;
}


// WGS84 <=> CK42 conversion, delta Latitude
double ppGeoCoord::dB(double Bd, double Ld, double H){
  double B, L, M, N;
  B = Bd * Pi / 180.0;
  L = Ld * Pi / 180.0;
  M = a * (1 - e2) / pow((1 - e2 * pow(sin(B) , 2)) , 1.5);
  N = a * pow((1 - e2 * pow(sin(B) , 2)) , -0.5);
  return ro / (M + H) * (N / a * e2 * sin(B) * cos(B) * da + ((N * N) / (a * a) + 1) * N * sin(B) * cos(B) * de2 / 2
    - (dx * cos(L) + dy * sin(L)) * sin(B) + dz * cos(B))
    - wx * sin(L) * (1 + e2 * cos(2 * B))
    + wy * cos(L) * (1 + e2 * cos(2 * B))
    - ro * ms * e2 * sin(B) * cos(B);
}

// WGS84 <=> CK42 conversion, delta Longitude
double ppGeoCoord::dL(double Bd, double Ld, double H){
  double  B, L, N;
  B = Bd * Pi / 180.0;
  L = Ld * Pi / 180.0;
  N = a * pow((1 - e2 * pow(sin(B) , 2)) , -0.5);
  return ro / ((N + H) * cos(B)) * (-dx * sin(L) + dy * cos(L))
    + tan(B) * (1 - e2) * (wx * cos(L) + wy * sin(L)) - wz;
}

// WGS84 => SK42bl (Lat/Lon)
ppGeoCoord::d2 ppGeoCoord::fromWGS84ToSK42bl(d2 p){
  double lat = p.d1;
  double lon = p.d2;
  p.d1 = lat - dB(lat, lon, 0) / 3600.0;
  p.d2 = lon - dL(lat, lon, 0) / 3600.0;
  return p;
}

// SK42bl (Lat/Lon) => WGS84
ppGeoCoord::d2 ppGeoCoord::fromSK42blToWGS84(d2 p){
  double lat = p.d1;
  double lon = p.d2;
  p.d1 = lat + dB(lat, lon, 0) / 3600.0;
  p.d2 = lon + dL(lat, lon, 0) / 3600.0;
  return p;
}

// Set conversion constants
ppGeoCoord::ppGeoCoord(){
  Pi = M_PI;
  ro = 206264.8062;
  aP = 6378245;
  alP = 1 / 298.3;
  e2P = 2 * alP - pow(alP , 2);
  aW = 6378137;
  alW = 1 / 298.257223563;
  e2W = 2 * alW - pow(alW , 2);
  a = (aP + aW) / 2;
  e2 = (e2P + e2W) / 2;
  da = aW - aP;
  de2 = e2W - e2P;
/*
  Datum from http://dgm.gki.com.ua/ua/transkord
  dx = 22.8788;
  dy = -122.977;
  dz = -80.113;
*/
/*
  Datum from http://gis-lab.info/qa/wgs84-sk42-wgs84-formula.html
  dx = 23.92;
  dy = -141.27;
  dz = -80.9;
*/
  dx = 22.8788;
  dy = -122.977;
  dz = -80.113;
  wx = 0;
  wy = 0;
  wz = 0;
  ms = 0;
  pLat = 50;
  pLon = 24;
}

// Input from SK42xy (Genstab)
void ppGeoCoord::fromSK42xy(double x,double y){
  // SK42xy -> SK42bl
  d2 sk42 = fromSK42xyToSK42bl(d2(x,y));
  // SK42bl -> WGS
  d2 wgs84 = fromSK42blToWGS84(sk42);
  pLat = wgs84.d1;
  pLon = wgs84.d2;
}

// Input from SK42bl (Lat/Lon)
void ppGeoCoord::fromSK42bl(double lat,double lon){
  pLat = lat + dB(lat, lon, 0) / 3600.0;
  pLon = lon + dL(lat, lon, 0) / 3600.0;
}

// Input from SK42bl DMS
void ppGeoCoord::fromSK42bl(int lat_d,int lat_m,int lat_s,int south,int lon_d,int lon_m,int lon_s,int west){
  double lat = (south?-1:1)*dms2flo(lat_d,lat_m,lat_s);
  double lon = (west?-1:1)*dms2flo(lon_d,lon_m,lon_s);
  pLat = lat + dB(lat, lon, 0) / 3600.0;
  pLon = lon + dL(lat, lon, 0) / 3600.0;
}

// Input from WGS84
void ppGeoCoord::fromWGS84(double lat,double lon){
  pLat = lat;
  pLon = lon;
}

// Input from WGS84 DMS
void ppGeoCoord::fromWGS84(int lat_d,int lat_m,int lat_s,int south,int lon_d,int lon_m,int lon_s,int west){
  pLat = (south?-1:1)*dms2flo(lat_d,lat_m,lat_s);
  pLon = (west?-1:1)*dms2flo(lon_d,lon_m,lon_s);
}

// Output to WGS84
void ppGeoCoord::toWGS84(double *lat,double *lon){
  *lat = pLat;
  *lon = pLon;
}

// Output to WGS84 DMS
void ppGeoCoord::toWGS84(int *lat_d,int *lat_m,int *lat_s,int *south,int *lon_d,int *lon_m,int *lon_s,int *west){
  flo2dms(pLat,lat_d,lat_m,lat_s);
  flo2dms(pLon,lon_d,lon_m,lon_s);
  *south = (pLat<0);
  *west = (pLon<0);
}


// Output to SK42 Lat/Lon
void ppGeoCoord::toSK42bl(double *lat,double *lon){
  d2 p = fromWGS84ToSK42bl(d2(pLat,pLon));
  *lat = p.d1;
  *lon = p.d2;
}

// Output to SK42 Lat/Lon DMS
void ppGeoCoord::toSK42bl(int *lat_d,int *lat_m,int *lat_s,int *south,int *lon_d,int *lon_m,int *lon_s,int *west){
  d2 p = fromWGS84ToSK42bl(d2(pLat,pLon));
  flo2dms(p.d1,lat_d,lat_m,lat_s);
  flo2dms(p.d2,lon_d,lon_m,lon_s);
  *south = (p.d1<0);
  *west = (p.d2<0);
}

// Output to GenStab
void ppGeoCoord::toSK42xy(double *x,double *y){
  d2 p = fromWGS84ToSK42bl(d2(pLat,pLon));
  p = fromSK42blToSK42xy(p);
  *x = p.d1;
  *y = p.d2;
}

// Double to DMS conversion
void ppGeoCoord::flo2dms(double f,int *d,int *m,int *s){
  *d=floor(fabs(f));
  *m=floor(((fabs(f))- *d)*60);
  *s=floor(((((fabs(f))- *d)*60)- *m)*60);
}

// DMS to double conversion
double ppGeoCoord::dms2flo(int d,int m,int s){
  return double(d)+(double(m)+double(s)/60.0)/60.0;
}


#ifdef USE_STD_STRING

// Double to DMS conversion
std::string ppGeoCoord::flo2dms(double f,int lat){
  char buf[64];
  int g=floor(fabs(f));
  int m=floor(((fabs(f))-g)*60);
  int s=floor(((((fabs(f))-g)*60)-m)*60);
#ifdef CONSOLE_STD_OUTPUT
  sprintf(buf,"%c%0.2i\370%0.2i'%0.2i\"",(f>0?(lat?'N':'E'):(lat?'S':'W')),g,m,s);
#endif
#ifndef CONSOLE_STD_OUTPUT
  sprintf(buf,"%c%0.2i°%0.2i'%0.2i\"",(f>0?(lat?'N':'E'):(lat?'S':'W')),g,m,s);
#endif
  return std::string(buf);
}

// Output as WGS84 string Degrees/Minutes/Seconds
std::string ppGeoCoord::toWGS84dms(){
  std::string lat = flo2dms(pLat,1);
  std::string lon = flo2dms(pLon,0);
  return std::string(lat+" "+lon);
}

// Output as SK42 string Degrees/Minutes/Seconds
std::string ppGeoCoord::toSK42dms(){
  d2 sk42 = fromWGS84ToSK42bl(d2(pLat,pLon));
  std::string lat = flo2dms(sk42.d1,1);
  std::string lon = flo2dms(sk42.d2,0);
  return std::string(lat+" "+lon);
}

// Output as SK42 string X/Y
std::string ppGeoCoord::toSK42xy(){
  d2 p = fromWGS84ToSK42bl(d2(pLat,pLon));
  p = fromSK42blToSK42xy(p);
  char buf[64];
  sprintf(buf,"X:%d Y:%d",int(p.d1),int(p.d2));
  return std::string(buf);
}
#endif
