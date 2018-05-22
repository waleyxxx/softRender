#include "Vector.h"
#include "utils.h"

Vector Vector::interp(const Vector& start, const Vector& end, float f)
{
	Vector ret;

	ret.x = Util::interp(start.x, end.x, f);
	ret.y = Util::interp(start.y, end.y, f);
	ret.z = Util::interp(start.z, end.z, f);
	ret.w = Util::interp(start.w, end.w, f);

	return ret;
}

Vector Vector::calcTriangleNormal(Vector a, Vector b, Vector c)
{
	Vector ab = b - a;
	Vector ac = c - a;
	return ab.crossProduct(ac);
}

Vector Vector::average(Vector* v, int n)
{
	Vector ret;

	for (int i = 0; i < n; ++i)
	{
		ret += v[i];
	}

	ret.normalize();

	return ret;
}