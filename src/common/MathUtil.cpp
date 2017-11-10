#include "MathUtil.hpp"

#include <cmath>

double MathUtil::linInterpolate(double ax, double ay, double bx, double by, double cx)
{
	// Avoid div by zero - use mean if x coords are identical
	double dx = bx-ax;
	if (dx==0.0) {
		return (ay + by) / 2.0;
	}
	// Swap a and b if they are the wrong way round
	if (ax > bx) {
		double tmp;
		tmp = ax;
		ax = bx;
		bx = tmp;
		tmp = ay;
		ay = by;
		by = tmp;
	}
	if (cx < ax) {
		return ay;
	}
	if (cx > bx) {
		return by;
	}
	double r = std::abs((cx - ax) / dx);
	return r * by + (1.0 - r) * ay;
}
