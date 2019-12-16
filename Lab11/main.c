#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "ring.h"


void main()
{
	struct ring ring1;

	printf("Enter center of ring (x , y): ");
	scanf("%lf %lf", &ring1.o.x, &ring1.o.y);

	printf("Enter point on the first circle (x , y): ");
	scanf("%lf %lf", &ring1.a.x, &ring1.a.y);

	printf("Enter point on the second circle (x , y): ");
	scanf("%lf %lf", &ring1.b.x, &ring1.b.y);

	printf("Ring perimeter: %.2lf\n", ring_perimeter(ring1));
	printf("Ring square: %.2lf\n", ring_square(ring1));
}
