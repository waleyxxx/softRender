#include "utils.h"

// x��ֵ�б���[min.max]֮��
int  Util::clamp(int x, int min, int max)
{
	return (x < min) ? min : ((x > max) ? max : x); 
}

 // �����ֵ��t Ϊ [0, 1] ֮�����ֵ
float  Util::interp(float x1, float x2, float t)
{
	return x1 + (x2 - x1) * t; 
}