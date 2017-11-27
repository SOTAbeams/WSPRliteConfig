#ifndef common_MathUtil_h
#define common_MathUtil_h

#include <cstdint>

namespace MathUtil
{

// Peform linear interpolation
// (ax,ay) and (bx,by) are the known data points, (cx,cy) is the interpolated point.
// Returns cy.
double linInterpolate(double ax, double ay, double bx, double by, double cx);

}

#endif
