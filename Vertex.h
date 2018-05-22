#pragma once
#include "utils.h"
#include "Vector.h"

struct CColor
{
	float r;
	float g;
	float b;

	CColor() :r(0), g(0), b(0)
	{
	}

	CColor(int rgb)
	{
		int x1 = (rgb >> 16) & 255;
		int x2 = (rgb >> 8) & 255;
		int x3 = rgb & 255;

		r = x1/255.f;
		g = x2/255.f;
		b = x3/255.f;
	}

	CColor(float _r, float _g, float _b) :r(_r), g(_g), b(_b)
	{
	}

	CColor operator+(const CColor& c) const
	{
		CColor ret;
		ret.r = r + c.r;
		ret.g = g + c.g;
		ret.b = g + c.b;

		return ret;
	}

	CColor& operator+=(const CColor& c)
	{
		r += c.r;
		g += c.g;
		b += c.b;

		return *this;
	}

	CColor& operator=(const CColor& c)
	{
		r = c.r;
		g = c.g;
		b = c.b;

		return *this;
	}

	CColor operator*(const CColor& c) const
	{
		CColor ret;
		ret.r = r*c.r;
		ret.g = g*c.g;
		ret.b = b*c.b;

		return ret;
	}


	CColor operator*(float f)
	{
		CColor ret;
		ret.r = r*f;
		ret.g = g*f;
		ret.b = b*f;

		return ret;
	}

	CColor& operator*=(float f)
	{
		r *= f;
		g *= f;
		b *= f;

		return *this;
	}

	static CColor interp(const CColor& start, const CColor& end, float f);

	static CColor WHITE;
};

struct TexCoord
{
	float u;
	float v;

	TexCoord() :u(0), v(0)
	{
	}

	TexCoord(float _u, float _v)
		:u(_u)
		, v(_v)
	{

	}
};

struct VertexPNT
{
	Vector pos;
	Vector normal;
	TexCoord tc;
	CColor diffuse;

	VertexPNT(){}
	VertexPNT(Vector _pos, Vector _normal, TexCoord _tc)
		: pos(_pos)
		, tc(_tc)
		, normal(_normal)
		, diffuse(1.f, 1.f, 1.f)
	{

	}

	VertexPNT(float x, float y, float z,
		float nx, float ny, float nz,
		float r, float g, float b,
		float tu, float tv)
		:pos(x,y,z,1)
		,normal(nx,ny,nz,0)
		,tc(tu, tv)
		, diffuse(r, g, b)
	{
	}
};


