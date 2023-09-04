#ifndef AZIMUTECACULATE_H
#define AZIMUTECACULATE_H

#include <QtMath>
#define FE 1.0/298.257222101
#define AE 6378137
extern double PI;

typedef struct BLH	//(经度L   纬度B   高程H)
{
    double b;
    double l;
    double h;
}stBLH;

//东北天坐标ENU0
typedef struct ENU
{
    double n;
    double e;
    double u;
}stENU;
class AzimuteCaculate
{
public:
    AzimuteCaculate();
    ~AzimuteCaculate();
    void BenuToAenu(stENU Cenu,stBLH Bblh,stBLH Ablh,double * CenuToA);// BLH 坐标转换 单位弧度
    void pos2ecef(const double *pos, double *r);
    void XYZ_to_NEU(double deltaX,double deltaY,double deltaZ,double B,double L,double *deltaN,double *deltaE,double *deltaH);
    void enu2ecef(const double *pos, const double *e, double *r);
    void xyz2enu(const double *pos, double *E);
    void matmul(const char *tr, int n, int k, int m, double alpha,
        const double *A, const double *B, double beta, double *C);

};

#endif // AZIMUTECACULATE_H
