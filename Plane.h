#pragma once
#include "utils.h"
#include "Vector.h"

class Plane
{
	Vector n;
	float d;
public:

	enum  SpatialRelation
	{
		RELATION_COPLANAR,
		RELATION_POSITIVE_HAFL_SPACE,
		RELATION_NEGATIVE_HAFL_SPACE 
	};

	~Plane(){};

	Plane(const Vector& normal, float distance) :n(normal), d(distance)
	{
		
	}

	Plane(float a, float b, float c, float distance) :n(a, b, c), d(distance)
	{

	}

	Plane(const Vector& normal, const Vector& p)
	{
		n = normal;
		d = -normal*p;
	}

	Plane(const Vector p1, const Vector p2, const Vector p3)
	{
		Vector p1p2 = p2 - p1;
		Vector p1p3 = p3 - p1;
		n = p1p2.crossProduct(p1p3);
		d = -(p1*n);
	}

	void normalize()
	{
		float len = n.len();
		n.normalize();
		d = d / len;
	}

	SpatialRelation planeDotCoord(Vector p)
	{
		float f = n*p+d;
		if (fabsf(f) < FLT_EPSILON)
		{
			return RELATION_COPLANAR;
		}

		return f > FLT_EPSILON ? RELATION_POSITIVE_HAFL_SPACE : RELATION_NEGATIVE_HAFL_SPACE;
	}

};

