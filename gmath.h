#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"

double * calculate_normal( double a1, double a2, double a3,
			   double b1, double b2, double b3 );
double calculate_dot( struct matrix *points, int i );

double *normalize(double *v);

double dot_product(double *p1, double *p2);

double *scalar_multiply(double *p1, double p2);

double *surface_normal(struct matrix *points, int i);

double *vector_subtract(double *p1, double *p2);
#endif
