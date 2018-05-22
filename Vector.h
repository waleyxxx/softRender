#pragma once
#include <math.h>
#include <memory.h>

class Vector
{
public:
	union 
	{
		struct { float x, y, z, w;};
		struct { float m[4]; };
	};

	Vector()
	{
		m[0] = m[1] = m[2] = m[3] = 0;
	}
	~Vector(){}

	Vector(float x, float y, float z, float w = 0)
	{
		m[0] = x;
		m[1] = y;
		m[2] = z;
		m[3] = w;
	}

	Vector(const Vector& v)
	{
		m[0] = v[0];
		m[1] = v[1];
		m[2] = v[2];
		m[3] = v[3];
	}

	Vector& operator=(const Vector& v)
	{
		m[0] = v[0];
		m[1] = v[1];
		m[2] = v[2];
		m[3] = v[3];

		return *this;
	}

	bool operator==(const Vector& v)
	{
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	float& operator[](int index)
	{
		return m[index];
	}

	float operator[](int index) const
	{
		return m[index];
	}

	float operator*(const Vector& v) const
	{
		return m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3] * v[3];
	}

	Vector operator*(float f) const
	{
		Vector ret;
		ret[0] = m[0] * f;
		ret[1] = m[1] * f;
		ret[2] = m[2] * f;
		ret[3] = m[3] * f;

		return ret;
	}

	Vector& operator*=(float f)
	{
		m[0] *= f;
		m[1] *= f;
		m[2] *= f;
		m[3] *= f;

		return *this;
	}

	Vector operator/(float f) const
	{
		Vector ret;
		ret[0] = m[0] / f;
		ret[1] = m[1] / f;
		ret[2] = m[2] / f;
		ret[3] = m[3] / f;

		return ret;
	}

	Vector& operator/=(float f) 
	{
		m[0] /= f;
		m[1] /= f;
		m[2] /= f;
		m[3] /= f;

		return *this;
	}

	Vector operator+(const Vector& v) const
	{
		Vector ret;
		ret[0] = m[0] + v[0];
		ret[1] = m[1] + v[1];
		ret[2] = m[2] + v[2];
		ret[3] = m[3] + v[3];

		return ret;
	}


	Vector& operator+=(const Vector& v)
	{
		m[0] += v[0];
		m[1] += v[1];
		m[2] += v[2];
		m[3] += v[3];

		return *this;
	}
	Vector operator-(const Vector& v) const
	{
		Vector ret;
		ret[0] = m[0] - v[0];
		ret[1] = m[1] - v[1];
		ret[2] = m[2] - v[2];
		ret[3] = m[3] - v[3];

		return ret;
	}

	Vector& operator-=(const Vector& v)
	{
		m[0] -= v[0];
		m[1] -= v[1];
		m[2] -= v[2];
		m[3] -= v[3];

		return *this;
	}

	Vector operator-() const
	{
		Vector ret;
		ret[0] = -m[0];
		ret[1] = -m[1];
		ret[2] = -m[2];
		ret[3] = -m[3];

		return ret;
	}

	float len()
	{
		return sqrt(m[0] * m[0] + m[1] * m[1] + m[2] * m[2]);
	}

	float len_square()
	{
		return m[0] * m[0] + m[1] * m[1] + m[2] * m[2];
	}

	Vector& normalize()
	{
		float inv = 1.0f / len();
		m[0] *= inv;
		m[1] *= inv;
		m[2] *= inv;

		return *this;
	}

	Vector crossProduct(const Vector& v) const
	{
		Vector ret;
		ret[0] = m[1] * v[2] - m[2] * v[1];
		ret[1] = m[2] * v[0] - m[0] * v[2];
		ret[2] = m[0] * v[1] - m[1] * v[0];

		return ret;
	}

	static Vector interp(const Vector& start, const Vector& end, float f);

	static Vector calcTriangleNormal(Vector a, Vector b, Vector c);

	static Vector average(Vector* v, int n);
};

