#ifndef _VECTOR_MATH_H_
#define _VECTOR_MATH_H_

#include <ctype.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VectorMath.h"
#include "tracer.h"

int line = 1;
const uint8_t INIT_NUM_OBJ = 64;

// next_c() wraps the getc() function and provides error checking and line
// number maintenance

int next_c(FILE *json) {
  int c = fgetc(json);
#ifdef DEBUG
  printf("next_c: '%c'\n", c);
#endif
  if (c == '\n') {
    line += 1;
  }
  if (c == EOF) {
    fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
    exit(1);
  }
  return c;
}

// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE *json, int d) {
  int c = next_c(json);
  if (c == d)
    return;
  fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
  exit(1);
}

// skip_ws() skips white space in the file.
void skip_ws(FILE *json) {
  int c = next_c(json);
  while (isspace(c)) {
    c = next_c(json);
  }
  ungetc(c, json);
}

// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char *next_string(FILE *json) {
  char buffer[129];
  int c = next_c(json);
  if (c != '"') {
    fprintf(stderr, "Error: Expected string on line %d.\n", line);
    exit(1);
  }
  c = next_c(json);
  int i = 0;
  while (c != '"') {
    if (i >= 128) {
      fprintf(stderr, "Error: Strings longer than 128 characters in length are "
                      "not supported.\n");
      exit(1);
    }
    if (c == '\\') {
      fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
      exit(1);
    }
    if (c < 32 || c > 126) {
      fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
      exit(1);
    }
    buffer[i] = c;
    i += 1;
    c = next_c(json);
  }
  buffer[i] = 0;
  return strdup(buffer);
}

double next_number(FILE *json) {
  double value;
  fscanf(json, "%lf", &value);
  // Error check this..
  return value;
}

double *next_vector(FILE *json) {
  double *v = malloc(3 * sizeof(double));
  expect_c(json, '[');
  skip_ws(json);
  v[0] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[1] = next_number(json);
  skip_ws(json);
  expect_c(json, ',');
  skip_ws(json);
  v[2] = next_number(json);
  skip_ws(json);
  expect_c(json, ']');
  return v;
}

/**
 *
 *
 *
 *
 */
Object **read_scene(char *filename) {
  int c;
  FILE *json = fopen(filename, "r");
  if (json == NULL) {
    fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
    exit(1);
  }

  Object **obj_array;
  obj_array = malloc(sizeof(Object *) * INIT_NUM_OBJ);

  skip_ws(json);

  // Find the beginning of the list
  expect_c(json, '[');
  skip_ws(json);

  size_t curr_obj = 0;
  while (1) {
    obj_array[curr_obj] = malloc(sizeof(Object));

    c = fgetc(json);
    if (c == ']') {
      fprintf(stderr, "Error: This is the worst scene file EVER.\n");
      fclose(json);
      return NULL;
    }
    if (c == '{') {
      skip_ws(json);

      // Parse the object
      char *key = next_string(json);
      if (strcmp(key, "type") != 0) {
        fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n",
                line);
        exit(1);
      }

      skip_ws(json);
      expect_c(json, ':');
      skip_ws(json);

      char *value = next_string(json);

      if (strcmp(value, "camera") == 0) {
        obj_array[curr_obj]->type = CAMERA;
      } else if (strcmp(value, "sphere") == 0) {
        obj_array[curr_obj]->type = SPHERE;
      } else if (strcmp(value, "plane") == 0) {
        obj_array[curr_obj]->type = PLANE;
      } else {
        fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n",
                value, line);
        exit(1);
      }

      skip_ws(json);

      while (1) {
        c = next_c(json);
        if (c == '}') {
          // stop parsing this object
          break;
        } else if (c == ',') {
          // read another field
          skip_ws(json);
          char *key = next_string(json);
          skip_ws(json);
          expect_c(json, ':');
          skip_ws(json);

          // Start storing values

          // Scaler types
          if (strcmp(key, "width") == 0) {
            double value = next_number(json);
            if (value < 0) {
              fprintf(stderr, "Error: Width cannot be less than 0. Found %lf "
                              "on line number %d.\n",
                      value, line);
              exit(1);
            }

            obj_array[curr_obj]->Camera.width = value;

          } else if (strcmp(key, "height") == 0) {
            double value = next_number(json);
            if (value < 0) {
              fprintf(stderr, "Error: Height cannot be less than 0. Found %lf "
                              "on line number %d.\n",
                      value, line);
              exit(1);
            }

            obj_array[curr_obj]->Camera.height = value;
          } else if (strcmp(key, "radius") == 0) {
            double value = next_number(json);
            if (value < 0) {
              fprintf(stderr, "Error: Radius cannot be less than 0. Found %lf "
                              "on line number %d.\n",
                      value, line);
              exit(1);
            }

            obj_array[curr_obj]->Sphere.radius = value;
          }

          // Vector types
          else if (strcmp(key, "color") == 0) {
            double *value = next_vector(json);
            obj_array[curr_obj]->color.r = value[0];
            if (value[0] < 0 || value[1] < 0 || value[2] < 0) {
              fprintf(stderr, "Error: color values cannot be less than 0. "
                              "On line number %d.\n",
                      line);
              exit(1);
            }

            obj_array[curr_obj]->color.g = value[1];
            obj_array[curr_obj]->color.b = value[2];
          } else if (strcmp(key, "position") == 0) {
            double *value = next_vector(json);

            if (obj_array[curr_obj]->type == PLANE) {
              v3_cpy(obj_array[curr_obj]->Plane.position, value);
            } else if (obj_array[curr_obj]->type == SPHERE) {
              v3_cpy(obj_array[curr_obj]->Sphere.position, value);
            } else {
              fprintf(stderr, "Error: Unknown type, \"%d\", on line %d.\n",
                      obj_array[curr_obj]->type, line);
              exit(1);
            }

          } else if (strcmp(key, "normal") == 0) {
            double *value = next_vector(json);
            v3_cpy(obj_array[curr_obj]->Plane.normal, value);
          } else {
            fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
                    key, line);
            exit(1);
          }

          skip_ws(json);
        } else {
          fprintf(stderr, "Error: Unexpected value on line %d\n", line);
          exit(1);
        }
      }

      skip_ws(json);
      c = next_c(json);
      if (c == ',') {
        skip_ws(json);
      } else if (c == ']') {
        fclose(json);
        obj_array[curr_obj + 1] = 0;
        return obj_array;
      } else {
        fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
        exit(1);
      }
    }
    curr_obj++;
  }
}

#endif
