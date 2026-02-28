#include "tinynumpy.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

ndarray *np_array(int ndim, int *shape) {
  if (ndim < 0 || shape == NULL) {
    fprintf(stderr, "Error: invalid ndim or NULL shape\n");
    return NULL;
  }

  // 1. Allocate the struct itself
  ndarray *arr = malloc(sizeof(*arr));
  if (arr == NULL) {
    perror("malloc failed");
    return NULL;
  }

  arr->ndim = ndim;
  arr->size = 1;

  // 2. Allocate and copy shape
  arr->shape = malloc(ndim * sizeof(int));
  if (arr->shape == NULL) {
    free(arr); // Clean up previously allocated memory
    return NULL;
  }

  for (int i = 0; i < ndim; i++) {
    if (shape[i] <= 0) {
      fprintf(stderr, "Error: dimension size must be positive\n");
      free(arr->shape);
      free(arr);
      return NULL;
    }
    arr->shape[i] = shape[i];
    arr->size *= shape[i]; // Calculate total size
  }

  // 3. Allocate strides (Row-major)
  arr->strides = malloc(ndim * sizeof(int));
  if (arr->strides == NULL) {
    free(arr->shape);
    free(arr);
    return NULL;
  }

  int current_stride = 1;
  for (int i = ndim - 1; i >= 0; i--) {
    arr->strides[i] = current_stride;
    current_stride *= shape[i];
  }

  // 4. Allocate the data buffer
  arr->data = calloc(arr->size, sizeof(double));
  if (arr->data == NULL) {
    free(arr->shape);
    free(arr->strides);
    free(arr);
    return NULL;
  }

  return arr;
}

ndarray *np_zeros(int ndim, int *shape) { return np_array(ndim, shape); }

ndarray *np_ones(int ndim, int *shape) {
  ndarray *arr = np_array(ndim, shape);
  if (arr == NULL)
    return NULL;

  for (int i = 0; i < arr->size; i++) {
    arr->data[i] = 1.0;
  }
  return arr;
}

ndarray *np_arange(double start, double stop, double step) {
  if (step > 0 && start >= stop) {
    fprintf(stderr, "Error: start must be less than stop for positive "
                    "step.\n");
    return NULL;
  } else if (step < 0 && start <= stop) {
    fprintf(stderr, "Error: start must be greater than stop for negative "
                    "step.\n");
    return NULL;
  } else if (step == 0) {
    fprintf(stderr, "Error: step cannot be zero");
    return NULL;
  }

  double epsilon = 1e-10;

  int size = (int)floor((stop - start - epsilon) / step) + 1;
  int shape[1] = {size};
  ndarray *res = np_array(1, shape);
  res->data[0] = start;
  for (int i = 1; i < size; i++) {
    res->data[i] = start + i * step;
  }

  return res;
}

void np_free(ndarray *arr) {
  if (arr == NULL) {
    return;
  }

  free(arr->data);
  free(arr->shape);
  free(arr->strides);
  free(arr);
}

double np_get(ndarray *arr, int *indices) {
  if (!arr || !indices) {
    fprintf(stderr, "Error: NULL pointer in np_get\n");
    return -1;
  }
  size_t offset = 0;
  for (int i = 0; i < arr->ndim; i++) {
    if (indices[i] < 0 || indices[i] >= arr->shape[i]) {
      fprintf(stderr, "Error: axis %d index %d is out of bounds\n", i,
              indices[i]);
      return -2;
    }

    offset += (size_t)indices[i] * arr->strides[i];
  }
  return arr->data[offset];
}

void np_set(ndarray *arr, int *indices, double value) {
  if (!arr || !indices) {
    fprintf(stderr, "Error: NULL pointer in np_set\n");
    return;
  }
  size_t offset = 0;
  for (int i = 0; i < arr->ndim; i++) {
    if (indices[i] < 0 || indices[i] >= arr->shape[i]) {
      fprintf(stderr, "Error: axis %d index %d is out of bounds\n", i,
              indices[i]);
      return;
    }

    offset += (size_t)indices[i] * arr->strides[i];
  }
  arr->data[offset] = value;
}

ndarray *np_add(ndarray *a, ndarray *b) {
  if (!a || !b) {
    fprintf(stderr, "Error: NULL pointer in np_add\n");
    return NULL;
  }

  if (a->ndim != b->ndim) {
    fprintf(stderr, "Error: dimension count mismatch\n");
    return NULL;
  }

  for (int i = 0; i < a->ndim; i++) {
    if (a->shape[i] != b->shape[i]) {
      fprintf(stderr, "Error: shape mismatch at axis %d (%d vs %d)\n", i,
              a->shape[i], b->shape[i]);
      return NULL;
    }
  }

  ndarray *res = np_array(a->ndim, a->shape);
  if (res == NULL) {
    return NULL;
  }

  for (int i = 0; i < a->size; i++) {
    res->data[i] = a->data[i] + b->data[i];
  }
  return res;
}