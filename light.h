#ifndef LIGHT_H
#define LIGHT_H

#include "matrix.h"

struct point_light{
  int *color;
  float *point;
} point_light;

struct r_properties{
  float *ka;
  float *kd;
  float *ks;
} r_properties;

//View vector is always 0, 0, -1
color find_light(struct matrix *polygons, struct point_light *light_source, struct r_properties *properties, int i);
float *ambient( int *ca, float *ka);
float *diffuse(struct matrix *polygons, struct point_light *light_source, int *cd, float *kd, int i);
float *specular(struct matrix *polygons, struct point_light *light_source, int *cspec, float*ks, int i);
#endif
