#pragma once
#include "Vector3.h"
#include <string.h>

struct Matrix3
{
    float elems[9];

    Matrix3();
    Matrix3(float elements[]);

    Matrix3 Inverse() const;
    Matrix3 Transpose() const;

    Matrix3 &operator+=(const Matrix3 &rhs)
    {
        for (int i = 0; i < 9; i++) {
            elems[i] += rhs.elems[i];
        }
        return *this;
    }

    Matrix3 &operator-=(const Matrix3 &rhs)
    {
        for (int i = 0; i < 9; i++) {
            elems[i] -= rhs.elems[i];
        }
        return *this;
    }

    Matrix3 &operator*=(const Matrix3 &rhs)
    {
        float m_copy[9];
        memcpy(m_copy, elems, sizeof(elems));

        elems[0] = m_copy[0] * rhs.elems[0] + m_copy[1] * rhs.elems[3] + m_copy[2] * rhs.elems[6];
        elems[1] = m_copy[0] * rhs.elems[1] + m_copy[1] * rhs.elems[4] + m_copy[2] * rhs.elems[7];
        elems[2] = m_copy[0] * rhs.elems[2] + m_copy[1] * rhs.elems[5] + m_copy[2] * rhs.elems[8];

        elems[3] = m_copy[3] * rhs.elems[0] + m_copy[4] * rhs.elems[3] + m_copy[5] * rhs.elems[6];
        elems[4] = m_copy[3] * rhs.elems[1] + m_copy[4] * rhs.elems[4] + m_copy[5] * rhs.elems[7];
        elems[5] = m_copy[3] * rhs.elems[2] + m_copy[4] * rhs.elems[5] + m_copy[5] * rhs.elems[8];

        elems[6] = m_copy[6] * rhs.elems[0] + m_copy[7] * rhs.elems[3] + m_copy[8] * rhs.elems[6];
        elems[7] = m_copy[6] * rhs.elems[1] + m_copy[7] * rhs.elems[4] + m_copy[8] * rhs.elems[7];
        elems[8] = m_copy[6] * rhs.elems[2] + m_copy[7] * rhs.elems[5] + m_copy[8] * rhs.elems[8];
        return *this;
    }

    float &operator[](const int index) { return elems[index]; }

    Matrix3 &operator*=(const float &rhs)
    {
        for (int i = 0; i < 9; i++) {
            elems[i] *= rhs;
        }
        return *this;
    }

    static Matrix3 CreateRotation(Vector3 rot);
};

Matrix3 operator+(const Matrix3 &lhs, const Matrix3 &rhs);
Matrix3 operator-(const Matrix3 &lhs, const Matrix3 &rhs);
Matrix3 operator*(const Matrix3 &lhs, const Matrix3 &rhs);
Matrix3 operator*(const Matrix3 &lhs, const float &rhs);

Vector3 operator*(const Matrix3 &mat, const Vector3 &vec);
