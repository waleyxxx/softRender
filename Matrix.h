#pragma once
#include <memory.h>
#include "Vector.h"
#include "assert.h"
#include "utils.h"

// 4x4 Matrix4x4
class Matrix4x4
{
	float m[4][4];
public:
	Matrix4x4()
	{
		zero();
	}
	~Matrix4x4(){};

	void zero()
	{
		memset(m, 0, 16 * sizeof(float));
	}

	void identity()
	{
		zero();
		m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
	}

	Vector getRow(int row) const
	{
		return Vector(m[row][0], m[row][1], m[row][2], m[row][3]);
	}

	Vector getColumn(int column) const
	{
		return Vector(m[0][column], m[1][column], m[2][column], m[3][column]);
	}

	float* operator[](int row)
	{
		return m[row];
	}

	Matrix4x4 operator+(Matrix4x4 r)
	{
		Matrix4x4 ret;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret[i][j] = m[i][j] + r[i][j];
			}
		}

		return ret;
	}

	Matrix4x4 operator-(Matrix4x4 r)
	{
		Matrix4x4 ret;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret[i][j] = m[i][j] - r[i][j];
			}
		}
		
		return ret;
	}

	Matrix4x4 operator*(const Matrix4x4& r)
	{
		Matrix4x4 ret;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret[i][j] = getRow(i) * r.getColumn(j);
			}
		}
	
		return ret;
	}

	Matrix4x4 operator*(const float scale)
	{
		Matrix4x4 ret;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret[i][j] = m[i][j]*scale;
			}
		}

		return ret;
	}

	Matrix4x4 operator/(const float scale)
	{
		Matrix4x4 ret;
		float inv = 1.f / scale;
		
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret[i][j] = m[i][j] * inv;
			}
		}

		return ret;
	}


	float determinant()
	{
		float ret = m[0][0] * cofactor(0, 0) + m[0][1] * cofactor(0, 1) + m[0][2] * cofactor(0, 2) + m[0][3] * cofactor(0, 3);
		return ret;
	}

	Matrix4x4 inverse()
	{
		Matrix4x4 ret;

		float det = determinant();
		if (fabsf(det)<FLT_EPSILON) // 只有秩大于0的矩阵，才存在逆矩阵.
		{
			assert(false);
		}

		ret = adjointMatrix();

		return ret / det;
	}

	Matrix4x4 transpose()
	{
		Matrix4x4 ret;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ret[j][i] = m[i][j];
			}
		}

		return ret;
	}

	static Vector transform(const Vector& vec, const Matrix4x4& mtrix);
	static Matrix4x4 translate(float x, float y, float z);
	static Matrix4x4 scaling(float x, float y, float z);
	static Matrix4x4 rotation(const Vector& v, float theta);
	static Matrix4x4 view(const Vector& eye, const Vector& lookAt, const Vector& up);
	static Matrix4x4 project(float aspectRatio, float fov, float zn, float zf);

private:

	// 3x3行列式的秩
	float determinant3x3(float x1, float x2, float x3,
						 float y1, float y2, float y3,
						 float z1, float z2, float z3);

	// 代数余子式
	float cofactor(int row, int column);

	// 伴随矩阵
	Matrix4x4 adjointMatrix();
};

