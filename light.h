#ifndef LIGHT_H
#define LIGHT_H

#include "matrix.h"

struct point_light{
  int *color;
  double *point;
} point_light;

struct r_properties{
  double *ka;
  double *kd;
  double *ks;
} r_properties;

//View vector is always 0, 0, -1
color find_light(struct matrix *polygons, struct point_light *light_source, struct r_properties *propertiesh, int i);
double *ambient( int *ca, double *ka);
double *diffuse(struct matrix *polygons, struct point_light *light_source, int *cd, double *kd, int i);
double *specular(struct matrix *polygons, struct point_light *light_source, int *cspec, double *ks, int i);
#endif
