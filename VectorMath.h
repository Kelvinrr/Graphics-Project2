#ifndef _VECTORMATH_H_
#define _VECTORMATH_H_

#include <math.h>

typedef double *V3;

static inline double sqr(double v) { return v * v; }

static inline void v3_add(V3 a, V3 b, V3 c) {
  c[0] = a[0] + b[0];
  c[1] = a[1] + b[1];
  c[2] = a[2] + b[2];
}

static inline void v3_subtract(V3 a, V3 b, V3 c) {
  c[0] = a[0] - b[0];
  c[1] = a[1] - b[1];
  c[2] = a[2] - b[2];
}

static inline void v3_scale(V3 a, double s, V3 c) {
  c[0] = s * a[0];
  c[1] = s * a[1];
  c[2] = s * a[2];
}

static inline double v3_dot(V3 a, V3 b) {
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static inline void v3_cross(V3 a, V3 b, V3 c) {
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
}

static inline void v3_cpy(V3 to, V3 from) {
  to[0] = from[0];
  to[1] = from[1];
  to[2] = from[2];
}

static inline void normalize(double *v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}

/**
 * Simple function that calculates the intersection with a
 * sphere. Based on the formula found in the text
 * provided by the class */
static inline double sphere_intersection(double *Ro, double *Rd, double *center,
                                         double r) {
  // Algorithm:
  //   A*t^2 + B*t + C = 0
  //  a = dx*dx + dy*dy + dz*dz;
  //  b = 2*dx*(x0-cx) +  2*dy*(y0-cy) +  2*dz*(z0-cz);
  //  c = cx*cx + cy*cy + cz*cz + x0*x0 + y0*y0 + z0*z0 +
  //  -2*(cx*x0 + cy*y0 + cz*z0) - R*R;
  //   t0, t1 = (- B + (B^2 - 4*C)^1/2) / 2 where t0 is for (-) and t1 is for
  //   (+)

  double A = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));

  double B = 2 * Rd[0] * (Ro[0] - center[0]) + 2 * Rd[1] * (Ro[1] - center[1]) +
             2 * Rd[2] * (Ro[2] - center[2]);

  double C = sqr(center[0]) + sqr(center[1]) + sqr(center[2]) + sqr(Ro[0]) +
             sqr(Ro[1]) + sqr(Ro[2]) +
             -2 * (center[0] * Ro[0] + center[1] * Ro[1] + center[2] * Ro[2]) -
             sqr(r);

  double det = sqr(B) - 4 * A * C;
  if (det < 0)
    return -1;

  double t0 = (-B - sqrt(det)) / (2 * A);
  if (t0 > 0)
    return t0;

  double t1 = (-B + sqrt(det)) / (2 * A);
  if (t1 > 0)
    return t1;

  return -1;
}

/**
 * Simple function that calculates the intersection with a
 * plane. Based on the formula found in the text
 * provided by the class */
static inline double plane_intersection(double *Ro, double *Rd,
                                        double *position, double *normal) {

  V3 temp = malloc(sizeof(double) * 3);
  v3_subtract(Ro, position, temp);

  double distance = v3_dot(normal, temp);
  double denominator = v3_dot(normal, Rd);

  distance = -(distance / denominator);

  if (distance > 0)
    return distance;

  return 0;
}

#endif
