#include <math.h>

/* Math functions that are available under C but not FORTRAN */
void cj0_(float	* x)
{
	*x = (float) j0((double) *x);
}

void cj1_(float	* x)
{
	*x = (float) j1((double) *x);
}

void *cceil_(float	* x)
{

	*x = (float) ceil((double) *x);

}

void *cfloor_(float	* x)
{

	*x = (float) floor((double) *x);

}

void *crint_(float	* x)
{

	*x = (float) rint((double) *x);

}

