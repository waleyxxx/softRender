#pragma once

#include <assert.h>

/* smallest such that 1.0+FLT_EPSILON != 1.0 */
#ifndef FLT_EPSILON

#define FLT_EPSILON     1.192092896e-07F

#endif 

#ifndef PI

#define PI 3.1415926f

#endif


typedef unsigned int  uint32;


#define SAFE_DELETE( p ) { if( p ) { delete p; p = NULL; }  }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] p; p = NULL; }  }
		
	
class Util
{
public:
	static  int clamp(int x, int min, int max);
	static float interp(float x1, float x2, float t);
};
