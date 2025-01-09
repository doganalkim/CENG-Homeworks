#ifndef __MATRIX4_H__
#define __MATRIX4_H__
#include "Vec3.h"

class Matrix4
{
public:
    double values[4][4];

    Matrix4();
    Matrix4(double values[4][4]);
    Matrix4(Vec3 u, Vec3 v, Vec3 w)
    {
        values[0][0] = u.x;   values[0][1] = u.y;  values[0][2] = u.z;     values[0][3] = 0; 

        values[1][0] = v.x;   values[1][1] = v.y;  values[1][2] = v.z;     values[1][3] = 0; 

        values[2][0] = w.x;   values[2][1] = w.y;  values[2][2] = w.z;     values[2][3] = 0; 

        values[3][0] = 0;   values[3][1] = 0;  values[3][2] = 0;     values[3][3] = 1; 
    }
    Matrix4(const Matrix4 &other);
    friend std::ostream &operator<<(std::ostream &os, const Matrix4 &m);
};


#endif