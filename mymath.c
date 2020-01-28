#define _USE_MATH_DEFINES
#include <windows.h>
#include <math.h>
#include "mymath.h"
double *tanTabl = NULL;
double *sinTabl = NULL;
double *cosTabl = NULL;

int InitMyMath()
{
    tanTabl = VirtualAlloc(NULL, sizeof(double)*900, MEM_COMMIT, PAGE_READWRITE);
    cosTabl = VirtualAlloc(NULL, sizeof(double)*901, MEM_COMMIT, PAGE_READWRITE);
    sinTabl = VirtualAlloc(NULL, sizeof(double)*901, MEM_COMMIT, PAGE_READWRITE);
    for(int i=0; i<900; i++)
    {
        tanTabl[i]=tan(i*M_PI/1800.0);
        sinTabl[i]=sin(i*M_PI/1800.0);
        cosTabl[i]=cos(i*M_PI/1800.0);
    }
    sinTabl[900]=1.0;
    cosTabl[900]=0.0;
    return 0;
}

double MySin(int angle)
{
    if(angle>=3600)
        angle %= 3600;
    while(angle<0)
        angle += 3600;

    if(angle<=900)
        return sinTabl[angle];
    else if(angle<=1800)
        return sinTabl[1800-angle];
    else if(angle<=2700)
        return -sinTabl[angle-1800];
    else
        return -sinTabl[3600-angle];
}

double MyCos(int angle)
{
    if(angle>=3600)
        angle %= 3600;
    while(angle<0)
        angle += 3600;

    if(angle<=900)
        return cosTabl[angle];
    else if(angle<=1800)
        return -cosTabl[1800-angle];
    else if(angle<=2700)
        return -cosTabl[angle-1800];
    else
        return cosTabl[3600-angle];
}

double MyTan(int angle)
{
    if(angle>=3600)
        angle %= 3600;
    while(angle<0)
        angle += 3600;

    if(angle<900)
        return tanTabl[angle];
    else if(angle<1800)
        return -tanTabl[1800-angle];
    else if(angle<2700)
        return tanTabl[angle-1800];
    else
        return -tanTabl[3600-angle];
}

int MathShutdown()
{
    VirtualFree(tanTabl, 0, MEM_RELEASE);
    VirtualFree(sinTabl, 0, MEM_RELEASE);
    VirtualFree(cosTabl, 0, MEM_RELEASE);
    return 0;
}
