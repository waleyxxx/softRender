#include "Matrix.h"

Matrix4x4 Matrix4x4::translate(float x, float y, float z)
{
	Matrix4x4 ret;
	ret.identity();
	ret[3][0] = x;
	ret[3][1] = y;
	ret[3][2] = z;

	return ret;
}

Matrix4x4 Matrix4x4::scaling(float x, float y, float z)
{
	Matrix4x4 ret;
	ret.identity();
	ret[0][0] = x;
	ret[1][1] = y;
	ret[2][2] = z;

	return ret;
}

Vector Matrix4x4::transform(const Vector& vec, const Matrix4x4& mtrix)
{
	Vector ret;
	ret[0] = vec * mtrix.getColumn(0);
	ret[1] = vec * mtrix.getColumn(1);
	ret[2] = vec * mtrix.getColumn(2);
	ret[3] = vec * mtrix.getColumn(3);

	return ret;
}

Matrix4x4 Matrix4x4::rotation(const Vector& v, float theta)
{
	Matrix4x4 ret;

	float qsin = (float)sin(theta * 0.5f);
	float qcos = (float)cos(theta * 0.5f);
	Vector vec = v;
	float w = qcos;
	vec.normalize();
	float x = vec[0] * qsin;
	float y = vec[1] * qsin;
	float z = vec[2] * qsin;
	ret.m[0][0] = 1 - 2 * y * y - 2 * z * z;
	ret.m[1][0] = 2 * x * y - 2 * w * z;
	ret.m[2][0] = 2 * x * z + 2 * w * y;
	ret.m[0][1] = 2 * x * y + 2 * w * z;
	ret.m[1][1] = 1 - 2 * x * x - 2 * z * z;
	ret.m[2][1] = 2 * y * z - 2 * w * x;
	ret.m[0][2] = 2 * x * z - 2 * w * y;
	ret.m[1][2] = 2 * y * z + 2 * w * x;
	ret.m[2][2] = 1 - 2 * x * x - 2 * y * y;
	ret.m[0][3] = ret.m[1][3] = ret.m[2][3] = 0.0f;
	ret.m[3][0] = ret.m[3][1] = ret.m[3][2] = 0.0f;
	ret.m[3][3] = 1.0f;

	return ret;
}

Matrix4x4 Matrix4x4::view(const Vector& eye, const Vector& lookAt, const Vector& up)
{
	Matrix4x4 ret;
	ret.identity();

	Vector zaxis = lookAt - eye;
	zaxis.normalize();

	Vector xaxis = up.crossProduct(zaxis);
	xaxis.normalize();

	Vector yaxis = zaxis.crossProduct(xaxis);
	
	ret[0][0] = xaxis[0];
	ret[1][0] = xaxis[1];
	ret[2][0] = xaxis[2];
	ret[3][0] = -(xaxis*eye);

	ret[0][1] = yaxis[0];
	ret[1][1] = yaxis[1];
	ret[2][1] = yaxis[2];
	ret[3][1] = -(yaxis*eye);

	ret[0][2] = zaxis[0];
	ret[1][2] = zaxis[1];
	ret[2][2] = zaxis[2];
	ret[3][2] = -(zaxis*eye);

	ret[0][3] = ret[1][3] = ret[2][3] = 0;
	ret[3][3] = 1;

	return ret;
}

Matrix4x4 Matrix4x4::project(float aspectRatio, float fov, float zn, float zf)
{
	Matrix4x4 ret;
	float fax = 1.0f / (float)tan(fov * 0.5f);
	ret.m[0][0] = (float)(fax / aspectRatio);
	ret.m[1][1] = (float)(fax);
	ret.m[2][2] = zf / (zf - zn);
	ret.m[3][2] = -zn * zf / (zf - zn);
	ret.m[2][3] = 1;

	return ret;
}

float Matrix4x4::determinant3x3(float x1, float x2, float x3, 
							 float y1, float y2, float y3, 
							 float z1, float z2, float z3)
{
	float ret = x1*y2*z3 + z1*x2*y3 + x3*y1*z2 - (z1*y2*x3 + x1*z2*y3 + z3*y1*x2);
	
	return ret;
}

// 计算特定行列的代数余子式
float Matrix4x4::cofactor(int row, int column)
{
	float x[3][3] = { 0 };

	int r = 0;
	int n = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (i == row) continue;

		for (int j = 0; j < 4; ++j)
		{
			if (j == column) continue;
			
			x[r][n++] = m[i][j];
		}

		++r;
		n = 0;
	}

	float ret = powf(-1, float(row + column))*determinant3x3(x[0][0], x[0][1], x[0][2],
															 x[1][0], x[1][1], x[1][2],
															 x[2][0], x[2][1], x[2][2]);
	return ret;
}

// 伴随矩阵
Matrix4x4 Matrix4x4::adjointMatrix()
{
	Matrix4x4 ret;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ret[i][j] = cofactor(i, j);
		}
	}

	ret = ret.transpose();

	return ret;
}