#include "interpolation.h"

#include <math.h>
#include <assert.h>

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif


/* tcatmullrom - Tangent calculation for Catmull-Rom splines
 *
 * Parameters:
 *   - p: The previous value
 *   - n: The next value:
 *   - d: The distance between points.
 */
double tcatmullrom(double p, double n, double d)
{
	assert(d != 0);
	return tcardinal(0, p, -d, n, d);
}

/*
 * tcardinal - Tangent calculation for Cardinal splines (canonical splines)
 *
 * Parameters:
 *   - c:  Tension parameter. Can be thought of
 *         as the "length" of the tangent.
 *         c=1 yields all zero tangents,
 *         c=0 yields a Catmull-rom spline.
 *         Must be in the interval [0,1].
 *   - p:  Value of the previous data point.
 *   - pd: Position of the previous data point.
 *   - n:  Value of the next data point.
 *   - nd: Position of the next data point.
 */
double tcardinal(double c,
                 double p, double pd, 
                 double n, double nd)
{
	assert(c >= 0 && c <= 1);
	assert(pd != nd);

	return (1-c)*(n-p)/(nd-pd);
}

/* 
 * icubic - Cubic interpolation function.
 *
 * Parameters:
 *   - t:  Distance between p0 and p1 to sample.
 *         Must be in the interval [0,1]
 *   - p0: Value of the first data point
 *   - m0: Tangent of the first data point
 *   - p1: Value of the second data point
 *   - m1: Tangent of the second data point
 */
double icubic(double t, double p0, double m0, double p1, double m1)
{
	double t3 = pow(t, 3);
	double t2 = pow(t, 2);

	return (2*t3 - 3*t3 +1)*p0 +
	        (t3 - 2*t2 + t)*m0 +
	         (-2*t3 + 3*t2)*p1 +
	           (t3 - t2)*m1;
}

/* 
 * icubicf - Cubic interpolation function.
 *
 * Parameters:
 *   - t:  Distance between p0 and p1 to sample.
 *         Must be in the interval [0,1]
 *   - p0: Value of the first data point
 *   - m0: Tangent of the first data point
 *   - p1: Value of the second data point
 *   - m1: Tangent of the second data point
 */
float icubicf(float t, float p0, float m0, float p1, float m1)
{
	float t3 = powf(t, 3);
	float t2 = powf(t, 2);

	return (2*t3 - 3*t3 +1)*p0 +
	        (t3 - 2*t2 + t)*m0 +
	         (-2*t3 + 3*t2)*p1 +
	           (t3 - t2)*m1;
}

/*
 * icosine - Cosine interpolation function
 *
 * Parameters:
 *   - t:  Distance between p0 and p1 to sample.
 *         Must be in the interval [0,1]
 *   - p0: Value of the first data point
 *   - p1: Value of the second data point
 */
double icosine(double t, double p0, double p1)
{
	double t2 = (1-cos(t*M_PI))/2;
	return p0*(1-t2)+p1*t2;
}


/*
 * icosinef - Cosine interpolation function
 *
 * Parameters:
 *   - t:  Distance between p0 and p1 to sample.
 *         Must be in the interval [0,1]
 *   - p0: Value of the first data point
 *   - p1: Value of the second data point
 */
float icosinef(float t, float p0, float p1)
{
	float t2 = (1-cosf(t*(float)M_PI))/2;
	return p0*(1-t2)+p1*t2;
}
