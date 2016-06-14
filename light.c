#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "gmath.h"
#include "light.h"

color find_light(struct matrix *polygons, struct point_light *light_source, struct r_properties *properties, int i){
  color x;
  float *ca = (float *)calloc(3, sizeof(double));
  ca[0] = 10;
  ca[1] = 10;
  ca[2] = 10;
  float *k_a = properties->ka;
  float *k_d = properties->kd;
  float *k_s = properties->ks;
  float *amb = ambient(ca, k_a);
  int *light_color = light_source->color;
  float * diff = diffuse(polygons, light_source, light_color, k_d, i);
  float * spec = specular(polygons, light_source, light_color, k_s, i);
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

float *ambient( int *ca, float *ka){
  float *c = (float *)calloc(3, sizeof(double));
  int i = 0;
  for(i=0;i<3;i++){
    c[i] = ca[i] * ka[i];
  }
  return c;
}

// View Vector is [0, 0, -1]
float *diffuse(struct matrix *polygons, struct point_light *light_source, int *cd, float *kd, int i){
  float *N = normalize(surface_normal(polygons, i));
  float *L_vector = (float *)calloc(3, sizeof(double));
  int j = 0;
  for(j=0;j<3;j++){
    L_vector[j] = light_source->point[j] - polygons->m[j][i];
  }
  float *L = normalize(L_vector);
  float *c = (float *)calloc(3, sizeof(double));
  for(j=0;j<3;j++){
    c[j] = kd[j] * cd[j] * dot_product(N, L);
  }
  return c;
}

float *specular(struct matrix *polygons, struct point_light *light_source, int *cspec, float *ks, int i){
  float *view = (float *)calloc(3, sizeof(double));
  view[0] = 0;
  view[1] = 0;
  view[2] = -1;
  float *N_vector = surface_normal(polygons, i);
  float *N = normalize(N_vector);
  float *L_vector = (float *)calloc(3, sizeof(double));
  int j = 0;
  for(j=0;j<3;j++){
    L_vector[j] = light_source->point[j] - polygons->m[j][i];
  }
  float *L = normalize(L_vector);
  float *c = (float *)calloc(3, sizeof(double));
  for(j=0;j<3;j++){
    c[j] = dot_product(vector_subtract( scalar_multiply(N_vector, 2 * dot_product(L, N)), L_vector), view) * ks[j] * cspec[j];
  }
  return c;
}
