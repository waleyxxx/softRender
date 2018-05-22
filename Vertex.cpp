#include "Vertex.h"

CColor CColor::WHITE = CColor(1, 1, 1);

CColor CColor::interp(const CColor& start, const CColor& end, float f)
{
	CColor ret;

	ret.r = Util::interp(start.r, end.r, f);
	ret.g = Util::interp(start.g, end.g, f);
	ret.b = Util::interp(start.b, end.b, f);

	return ret;
}