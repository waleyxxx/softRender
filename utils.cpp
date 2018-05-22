#include "utils.h"

// x的值夹逼在[min.max]之间
int  Util::clamp(int x, int min, int max)
{
	return (x < min) ? min : ((x > max) ? max : x); 
}

 // 计算插值：t 为 [0, 1] 之间的数值
float  Util::interp(float x1, float x2, float t)
{
	return x1 + (x2 - x1) * t; 
}