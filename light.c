#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "gmath.h"
#include "light.h"

color find_light(struct matrix *polygons, struct point_light *light_source, struct r_properties *propertiesh, int i){
  color x;
  int j = 0;
  int *ca = (int *)calloc(3, sizeof(int));
  ca[0] = 210;
  ca[1] = 40;
  ca[2] = 150;
  struct point_light *light = (struct point_light *)calloc(1, sizeof(struct point_light));
  light->color = (int *)calloc(3, sizeof(int));
  light->point = (double *)calloc(3, sizeof(double));
  light->color[0] = 210;
  light->color[1] = 40;
  light->color[2] = 150;
  light->point[0] = 0;
  light->point[1] = 0;
  light->point[2] = 1;
  printf("Seffault?\n");
  struct r_properties *properties = (struct r_properties *)calloc(1, sizeof(struct r_properties));
  properties->ka = (double *)calloc(3, sizeof(double));
  properties->kd = (double *)calloc(3, sizeof(double));
  properties->ks = (double *)calloc(3, sizeof(double));
  for(j=0;j<3;j++){
    properties->ka[j] = 0.5;
    properties->kd[j] = 0.1;
    properties->ks[j] = 0.4;
  }
  double *k_a = properties->ka;
  double *k_d = properties->kd;
  double *k_s = properties->ks;
  double *amb = ambient(ca, k_a);
  double * diff = diffuse(polygons, light, light->color, k_d, i);
  double * spec = specular(polygons, light, light->color, k_s, i);
  x.red = amb[0] + diff[0] + spec[0];
  x.green = amb[1] + diff[1] + spec[1];
  x.blue = amb[2] + diff[2] + spec[2];
  if(x.red > 255){
    x.red = 255;
  }
  if(x.green > 255){
    x.green = 255;
  }
  if(x.blue > 255){
    x.blue  = 255;
  }
  return x;

}

double *ambient( int *ca, double *ka){
  double *c = (double *)calloc(3, sizeof(double));
  int i = 0;
  for(i=0;i<3;i++){
    c[i] = ca[i] * ka[i];
  }
  return c;
}

// View Vector is [0, 0, -1]
double *diffuse(struct matrix *polygons, struct point_light *light_source, int *cd, double *kd, int i){
  double *N = normalize(surface_normal(polygons, i));
  double *L_vector = (double *)calloc(3, sizeof(double));
  int j = 0;
  for(j=0;j<3;j++){
    L_vector[j] = light_source->point[j] - polygons->m[j][i];
  }
  double *L = normalize(L_vector);
  double *c = (double *)calloc(3, sizeof(double));
  for(j=0;j<3;j++){
    c[j] = kd[j] * cd[j] * dot_product(N, L);
  }
  return c;
}

double *specular(struct matrix *polygons, struct point_light *light_source, int *cspec, double *ks, int i){
  double *view = (double *)calloc(3, sizeof(double));
  view[0] = 0;
  view[1] = 0;
  view[2] = -1;
  double *N_vector = surface_normal(polygons, i);
  double *N = normalize(N_vector);
  double *L_vector = (double *)calloc(3, sizeof(double));
  int j = 0;
  for(j=0;j<3;j++){
    L_vector[j] = light_source->point[j] - polygons->m[j][i];
  }
  double *L = normalize(L_vector);
  double *c = (double *)calloc(3, sizeof(double));
  for(j=0;j<3;j++){
    c[j] = dot_product(vector_subtract( scalar_multiply(N_vector, 2 * dot_product(L, N)), L_vector), view) * ks[j] * cspec[j];
  }
  return c;
}
