#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "gmath.h"



/*======== double * calculate_normal() ==========
  Inputs:   double ax
            double ay
	    double az
	    double bx
	    double by
	    double bz
  Returns: A double arry of size 3 representing the
           cross product of <ax, ay, az> and <bx, by, bz>

  04/17/12 16:46:30
  jonalf
  ====================*/
double * calculate_normal( double ax, double ay, double az,
			   double bx, double by, double bz ) {

  double *normal;
  normal = (double *)malloc(3 * sizeof(double));

  normal[0] = ay*bz - az*by;
  normal[1] = az*bx - ax*bz;
  normal[2] = ax*by - ay*bx;

  return normal;
}

double * normalize(double *v){
	double length = sqrt(v[0] * v[0] + v[1] *v[1] + v[2] * v[2]);
	double *nv = (double *)calloc(3, sizeof(double));
	int i = 0;
	for(i = 0; i<3; i++){
		nv[i] = v[i]/length;
	}
	return nv;
}
/*======== double calculate_dot() ==========
  Inputs:   struct matrix *points
            int i
  Returns: The dot product of a surface normal and
           a view vector

  calculates the dot product of the surface normal to
  triangle points[i], points[i+1], points[i+2] and a
  view vector (use <0, 0, -1> to start.

  04/17/12 16:38:34
  jonalf
  ====================*/
double calculate_dot( struct matrix *points, int i ) {

  double ax, ay, az, bx, by, bz;
  double *normal;
  double vx, vy, vz;
  double dot;

  //calculate A and B vectors
  ax = points->m[0][i+1] - points->m[0][i];
  ay = points->m[1][i+1] - points->m[1][i];
  az = points->m[2][i+1] - points->m[2][i];

  bx = points->m[0][i+2] - points->m[0][i];
  by = points->m[1][i+2] - points->m[1][i];
  bz = points->m[2][i+2] - points->m[2][i];

  //get the surface normal
  normal = calculate_normal( ax, ay, az, bx, by, bz );

  //set up view vector
  vx = 0;
  vy = 0;
  vz = -1;

  //calculate dot product
  dot = normal[0] * vx + normal[1] * vy + normal[2] * vz;

  free(normal);
  return dot;
}

double dot_product(double *p1, double *p2){
	return p1[0] * p2[0] + p1[1]*p2[1] + p1[2]*p2[2];
}

double *scalar_multiply(double *p1, double p2){
	double *scal = (double *)calloc(3, sizeof(double));
	int i = 0;
	for(i=0;i<3;i++){
		scal[i] = p1[i] * p2;
	}
	return scal;
}

double *surface_normal(struct matrix *points, int i){
	double ax, ay, az, bx, by, bz;
  double *normal;
  double vx, vy, vz;
  double dot;

  //calculate A and B vectors
  ax = points->m[0][i+1] - points->m[0][i];
  ay = points->m[1][i+1] - points->m[1][i];
  az = points->m[2][i+1] - points->m[2][i];

  bx = points->m[0][i+2] - points->m[0][i];
  by = points->m[1][i+2] - points->m[1][i];
  bz = points->m[2][i+2] - points->m[2][i];

  //get the surface normal
  normal = calculate_normal( ax, ay, az, bx, by, bz );
	return normal;
}

double *vector_subtract(double *p1, double *p2){
	double *res = (double *)calloc(3, sizeof(double));
	int i = 0;
	for(i=0;i<3;i++){
		res[i] = p1[i] - p2[i];
	}
	return res;
}
