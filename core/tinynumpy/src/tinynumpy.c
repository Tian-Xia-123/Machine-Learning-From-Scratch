#include "tinynumpy.h"
#include <stdio.h>
#include <stdlib.h>

ndarray *np_array(int ndim, int *shape) {
  if (ndim < 0 || shape == NULL) {
    fprintf(stderr, "tinynumpy ERROR: Invalid ndim or NULL shape\n");
    return NULL;
  }

  // 1. Allocate the struct itself
  ndarray *arr = malloc(sizeof(*arr));
  if (arr == NULL) {
    fprintf(stderr, "tinynumpy ERROR: Memory allocation failed for struct\n");
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
      fprintf(stderr, "tinynumpy ERROR: Dimension size must be positive\n");
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
    fprintf(stderr, "tinynumpy ERROR: NULL pointer in np_get\n");
    return -1;
  }
  size_t offset = 0;
  for (int i = 0; i < arr->ndim; i++) {
    if (indices[i] < 0 || indices[i] >= arr->shape[i]) {
      fprintf(stderr,
              "tinynumpy IndexError: Axis %d index %d is out of bounds\n", i,
              indices[i]);
      return -2;
    }

    offset += (size_t)indices[i] * arr->strides[i];
  }
  return arr->data[offset];
}

void np_set(ndarray *arr, int *indices, double value) {
  if (!arr || !indices) {
    fprintf(stderr, "tinynumpy ERROR: NULL pointer in np_set\n");
    return;
  }
  size_t offset = 0;
  for (int i = 0; i < arr->ndim; i++) {
    if (indices[i] < 0 || indices[i] >= arr->shape[i]) {
      fprintf(stderr,
              "tinynumpy IndexError: Axis %d index %d is out of bounds\n", i,
              indices[i]);
      return;
    }

    offset += (size_t)indices[i] * arr->strides[i];
  }
  arr->data[offset] = value;
}

ndarray *np_add(ndarray *a, ndarray *b) {
  if (!a || !b) {
    fprintf(stderr, "tinynumpy ERROR: NULL pointer in np_add\n");
    return NULL;
  }

  if (a->ndim != b->ndim) {
    fprintf(stderr, "tinynumpy ERROR: Dimension count mismatch\n");
    return NULL;
  }

  for (int i = 0; i < a->ndim; i++) {
    if (a->shape[i] != b->shape[i]) {
      fprintf(stderr, "tinynumpy ERROR: Shape mismatch at axis %d (%d vs %d)\n",
              i, a->shape[i], b->shape[i]);
      return NULL;
    }
  }

  ndarray *res = np_array(a->ndim, a->shape);
  if (res == NULL) {
    fprintf(stderr, "tinynumpy ERROR: Memory allocation failed for struct\n");
    return NULL;
  }

  for (int i = 0; i < a->size; i++) {
    res->data[i] = a->data[i] + b->data[i];
  }
  return res;
}