#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "json_parser.h"

#include "VectorMath.h"
#include "pplib.h"
#include "tracer.h"

int main(int argc, char *argv[]) {

  // check if args are within range
  if (argc < 5) {
    fprintf(stderr, "Error: Not enough arguements\n");
    exit(1);
  }

  // get the width and height
  int img_width = strtol(argv[1], (char **)NULL, 10);
  int img_height = strtol(argv[2], (char **)NULL, 10);
  char *input_json = argv[3];

  // open the output file for reading
  FILE *output_ppm = fopen(argv[4], "wb");
  if (!output_ppm) {
    fprintf(stderr, "ERROR: Failed to open file %s\n", argv[4]);
    fclose(output_ppm);
    return -1;
  }

  // init the color objexts for later
  Pixel color = {.r = 0, .g = 0, .b = 0};
  Pixel bg_color = {.r = 0, .g = 0, .b = 0};

  // get the array of object pointers
  Object **objects = read_scene(input_json);
  double h = 2;
  double w = 2;
  int num_cams = 0;

  // look for the camera
  for (int i = 0; objects[i] != 0; i++) {
    if (objects[i]->type == 0) {
      w = objects[i]->Camera.width;
      h = objects[i]->Camera.height;
      num_cams++;
    }
  }

  // if no camera was found or more than one camera was found
  // throw an error
  if (num_cams != 1) {
    fprintf(stderr, "ERROR: Incorrect number of cameras specified, must have "
                    "1. Found: %d\n",
            num_cams);
  }

  // image variables
  double cx = 0;
  double cy = 0;
  int N = img_width;
  int M = img_height;

  // create the image buffer
  Pixel *buffer = malloc(M * N * sizeof(Pixel));

  double pixheight = h / M;
  double pixwidth = w / N;

  for (int y = 0; y < M; y += 1) {
    for (int x = 0; x < N; x += 1) {
      double Ro[3] = {0, 0, 0};
      double Rd[3] = {cx - (w / 2) + pixwidth * (x + 0.5),
                      -(cy - (h / 2) + pixheight * (y + 0.5)), 1};
      normalize(Rd);

      double best_t = INFINITY;
      size_t closest_object_idx = 0;
      // find the nearest object and distance

      for (int i = 0; objects[i] != 0; i++) {
        double t = 0;
        switch (objects[i]->type) {
        case PLANE:
          t = plane_intersection(Ro, Rd, objects[i]->Plane.position,
                                 objects[i]->Plane.normal);
          break;
        case SPHERE:
          t = sphere_intersection(Ro, Rd, objects[i]->Sphere.position,
                                  objects[i]->Sphere.radius);
          break;
        case CAMERA:
          break;
        default: // error
          exit(1);
        } // end of switch

        if (t > 0 && t < best_t) {
          best_t = t;
          closest_object_idx = i;
        }
      } // end of loop

      color.r = objects[closest_object_idx]->color.r * 255;
      color.g = objects[closest_object_idx]->color.g * 255;
      color.b = objects[closest_object_idx]->color.b * 255;

      if (best_t > 0 && best_t != INFINITY) {
        buffer[y * N + x] = color;
      } else {
        buffer[y * N + x] = bg_color;
      }
    }
  }

  buffer_to_bin(buffer, N, M, output_ppm);
  return 0;
}
