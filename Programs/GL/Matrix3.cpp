#include "Matrix3.h"

Matrix3::Matrix3()
{
    elems[0] = 1;
    elems[1] = 0;
    elems[2] = 0;
    elems[3] = 0;
    elems[4] = 1;
    elems[5] = 0;
    elems[6] = 0;
    elems[7] = 0;
    elems[8] = 1;
}

Matrix3::Matrix3(float elements[])
{
    for (int i = 0; i < 9; i++) {
        elems[i] = elements[i];
    }
}

Matrix3 Matrix3::Inverse() const
{
    double det = elems[0] * (elems[4] * elems[8] - elems[7] * elems[5]) -
                 elems[1] * (elems[3] * elems[8] - elems[5] * elems[6]) +
                 elems[2] * (elems[3] * elems[7] - elems[4] * elems[6]);

    if (det == 0)
        return Matrix3();

    double invdet = 1 / det;

    Matrix3 minv;
    minv[0] = (elems[4] * elems[8] - elems[7] * elems[5]) * invdet;
    minv[1] = (elems[2] * elems[7] - elems[1] * elems[8]) * invdet;
    minv[2] = (elems[1] * elems[5] - elems[2] * elems[4]) * invdet;
    minv[3] = (elems[5] * elems[6] - elems[3] * elems[8]) * invdet;
    minv[4] = (elems[0] * elems[8] - elems[2] * elems[6]) * invdet;
    minv[5] = (elems[3] * elems[2] - elems[0] * elems[5]) * invdet;
    minv[6] = (elems[3] * elems[7] - elems[6] * elems[4]) * invdet;
    minv[7] = (elems[6] * elems[1] - elems[0] * elems[7]) * invdet;
    minv[8] = (elems[0] * elems[4] - elems[3] * elems[1]) * invdet;
    return minv;
}

Matrix3 Matrix3::Transpose() const
{
    Matrix3 mat;
    mat.elems[0] = elems[0];
    mat.elems[1] = elems[3];
    mat.elems[2] = elems[6];
    mat.elems[3] = elems[1];
    mat.elems[4] = elems[4];
    mat.elems[5] = elems[7];
    mat.elems[6] = elems[2];
    mat.elems[7] = elems[5];
    mat.elems[8] = elems[8];
    return mat;
}

Matrix3 Matrix3::CreateRotation(Vector3 rot)
{
    float sx = sin(-rot.x);
    float cx = cos(-rot.x);
    float sy = sin(-rot.y);
    float cy = cos(-rot.y);
    float sz = sin(-rot.z);
    float cz = cos(-rot.z);

    Matrix3 X;
    X.elems[4] = cx;
    X.elems[5] = -sx;
    X.elems[7] = sx;
    X.elems[8] = cx;

    Matrix3 Y;
    Y.elems[0] = cy;
    Y.elems[2] = sy;
    Y.elems[6] = -sy;
    Y.elems[8] = cy;

    Matrix3 Z;
    Z.elems[0] = cz;
    Z.elems[1] = -sz;
    Z.elems[3] = sz;
    Z.elems[4] = cz;
    return X * Y * Z;
}

Matrix3 operator+(const Matrix3 &lhs, const Matrix3 &rhs)
{
    Matrix3 ret = lhs;

    for (int i = 0; i < 9; i++) {
        ret[i] += rhs.elems[i];
    }

    return ret;
}

Matrix3 operator-(const Matrix3 &lhs, const Matrix3 &rhs)
{
    Matrix3 ret = lhs;

    for (int i = 0; i < 9; i++) {
        ret[i] -= rhs.elems[i];
    }

    return ret;
}

Matrix3 operator*(const Matrix3 &lhs, const Matrix3 &rhs)
{
    Matrix3 ret = lhs;
    ret *= rhs;
    return ret;
}

Matrix3 operator*(const Matrix3 &lhs, const float &rhs)
{
    Matrix3 ret = lhs;

    for (int i = 0; i < 9; i++) {
        ret[i] *= rhs;
    }

    return ret;
}

Vector3 operator*(const Matrix3 &mat, const Vector3 &vec)
{
    float nx = mat.elems[0] * vec.x + mat.elems[1] * vec.y + mat.elems[2] * vec.z;
    float ny = mat.elems[3] * vec.x + mat.elems[4] * vec.y + mat.elems[5] * vec.z;
    float nz = mat.elems[6] * vec.x + mat.elems[7] * vec.y + mat.elems[8] * vec.z;
    return Vector3(nx, ny, nz);
}
