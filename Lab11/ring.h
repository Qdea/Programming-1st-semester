#ifndef _FIGURE_H
#define _FIGURE_H
#define _USE_MATH_DEFINES
#include <math.h>
struct point {
	double x;
	double y;
};

struct ring {
	struct point o;
	struct point a;
	struct point b;
};

double distance(struct point a, struct point b)
{
	return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

double ring_perimeter(struct ring f)
{
	double r1 = distance(f.o, f.a);
	double r2 = distance(f.o, f.b);

	return 2 * M_PI * (r1 + r2);
}
double ring_square(struct ring f)
{
	double r1 = distance(f.o, f.a);
	double r2 = distance(f.o, f.b);

	return M_PI * fabs(r2 * r2 - r1 * r1);
}
#endif
