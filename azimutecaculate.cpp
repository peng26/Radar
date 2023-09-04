#include "azimutecaculate.h"

double PI = 3.1415926535898;//圆周

AzimuteCaculate::AzimuteCaculate(){}
AzimuteCaculate::~AzimuteCaculate(){}
void AzimuteCaculate::BenuToAenu(stENU Cenu,stBLH Bblh,stBLH Ablh,double * CenuToA)// BLH 坐标转换 单位弧度
{
    double Bjwd[3] = {0.0};
    double Ajwd[3] = {0.0};
    double Bxyz[3] = {0.0};
    double Axyz[3] = {0.0};
    double enuToB[3] = {0.0};
    double deltaBC[3] = {0.0};
    double deltaAC[3] = {0.0};
    double Czuobiao[3] = {0.0};
    Bjwd[0] = Bblh.b * PI / 180;
    Bjwd[1] = Bblh.l * PI / 180;
    Bjwd[2] = Bblh.h;
    Ajwd[0] = Ablh.b * PI / 180;
    Ajwd[1] = Ablh.l * PI / 180;
    Ajwd[2] = Ablh.h;
    enuToB[0] = Cenu.e;
    enuToB[1] = Cenu.n;
    enuToB[2] = Cenu.u;

    pos2ecef(Bjwd,Bxyz);
    pos2ecef(Ajwd,Axyz);

    enu2ecef(Bjwd,enuToB,deltaBC);

    for(int i=0;i<3;i++)
        Czuobiao[i] = deltaBC[i] + Bxyz[i];
    for(int n=0;n<3;n++)
        deltaAC[n] = Czuobiao[n] - Axyz[n];
    XYZ_to_NEU(deltaAC[0], deltaAC[1], deltaAC[2], Ajwd[0], Ajwd[1], &CenuToA[1], &CenuToA[0], &CenuToA[2]);
}

void AzimuteCaculate::pos2ecef(const double *pos, double *r)
{
    double sinp=sin(pos[0]),cosp=cos(pos[0]),sinl=sin(pos[1]),cosl=cos(pos[1]);
    double e2=FE*(2.0-FE),v=AE/sqrt(1.0-e2*sinp*sinp);

    r[0]=(v+pos[2])*cosp*cosl;
    r[1]=(v+pos[2])*cosp*sinl;
    r[2]=(v*(1.0-e2)+pos[2])*sinp;
}

void AzimuteCaculate::enu2ecef(const double *pos, const double *e, double *r)
{
    double E[9];

    xyz2enu(pos,E);
    matmul("TN",3,1,3,1.0,E,e,0.0,r);
}

void AzimuteCaculate::XYZ_to_NEU(double deltaX,double deltaY,double deltaZ,double B,double L,double *deltaN,double *deltaE,double *deltaH)
{
    //B=B*PI/180;
    //L=L*PI/180;
    *deltaN=-sin(B)*cos(L)*deltaX-sin(B)*sin(L)*deltaY+cos(B)*deltaZ;
    *deltaE=-sin(L)*deltaX+cos(L)*deltaY;
    *deltaH=cos(B)*cos(L)*deltaX+cos(B)*sin(L)*deltaY+sin(B)*deltaZ;
}

void AzimuteCaculate::xyz2enu(const double *pos, double *E)
{
    double sinp=sin(pos[0]),cosp=cos(pos[0]),sinl=sin(pos[1]),cosl=cos(pos[1]);

    E[0]=-sinl;      E[3]=cosl;       E[6]=0.0;
    E[1]=-sinp*cosl; E[4]=-sinp*sinl; E[7]=cosp;
    E[2]=cosp*cosl;  E[5]=cosp*sinl;  E[8]=sinp;
}

void AzimuteCaculate::matmul(const char *tr, int n, int k, int m, double alpha,
    const double *A, const double *B, double beta, double *C)
{
    double d;
    int i,j,x;
    //20180806:yingjj 周宇说第三方测评三目运算符存在问题修改
    //int f=tr[0]=='N'?(tr[1]=='N'?1:2):(tr[1]=='N'?3:4);
    int f1=0,f2=0,f=0;
     f1=((tr[1]=='N')?1:2);
     f2=((tr[1]=='N')?3:4);
     f=((tr[0]=='N')?f1:f2);

    for (i=0;i<n;i++)
        for (j=0;j<k;j++)
        {
            d=0.0;
            switch (f)
            {
            case 1: for (x=0;x<m;x++) d+=A[i+x*n]*B[x+j*m]; break;
            case 2: for (x=0;x<m;x++) d+=A[i+x*n]*B[j+x*k]; break;
            case 3: for (x=0;x<m;x++) d+=A[x+i*m]*B[x+j*m]; break;
            case 4: for (x=0;x<m;x++) d+=A[x+i*m]*B[j+x*k]; break;
            default:
                break;
            }
            //20190118:yingjj 之前为beta==0.0
            //if (beta==0.0)
            //20190118:周宇软件测评修改
            if ((beta > -0.00000001) && (beta < 0.00000001))
                C[i+j*n]=alpha*d;
            else
                C[i+j*n]=alpha*d+beta*C[i+j*n];
        }
}
