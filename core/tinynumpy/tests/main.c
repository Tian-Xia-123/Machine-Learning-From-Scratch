#include "tinynumpy.h"
#include <stdio.h>

/**
 * Helper function to print a 2D array for visualization
 */
void print_2d_array(ndarray *arr) {
  if (arr->ndim != 2) {
    printf("ERROR: only 2D arrays are supported for printing.\n");
    return;
  }
  printf("Array: (Shape: %d x %d):\n", arr->shape[0], arr->shape[1]);
  for (int i = 0; i < arr->shape[0]; i++) {
    for (int j = 0; j < arr->shape[1]; j++) {
      int indices[2] = {i, j};
      printf("%6.1f ", np_get(arr, indices));
    }
    printf("\n");
  }
}

int main() {
  // --- 1. Test Array Creation ---
  printf("--- Testing np_array ---\n");
  int shape[2] = {4, 5};
  ndarray *arr = np_array(2, shape);

  if (arr == NULL) {
    printf("Failed to create array.\n");
    return 1;
  }
  printf("Array created. Size: %d, ndim: %d\n\n", arr->size, arr->ndim);

  // --- 2. Test np_set ---
  printf("--- Testing np_set ---\n");
  // Fill arr with 1.0
  for (int i = 0; i < shape[0]; i++) {
    for (int j = 0; j < shape[1]; j++) {
      int indices[2] = {i, j};
      double val = 1.0;
      np_set(arr, indices, val);
    }
  }
  print_2d_array(arr);
  printf("\n");

  // --- 3. Test Bounds Checking ---
  printf("--- Testing Bounds Checking ---\n");
  int bad_indices[2] = {2, 7}; // Column 7 is out of bounds for a 5-column array
  printf("Attempting to access [2, 7]...\n");
  np_set(arr, bad_indices, 1.0);
  printf("\n");

  // --- 4. Test np_add ---
  printf("--- Testing np_add ---\n");
  ndarray *arr2 = np_array(2, shape);

  // Fill arr2 with 1.0
  for (int i = 0; i < shape[0]; i++) {
    for (int j = 0; j < shape[1]; j++) {
      int indices[2] = {i, j};
      double val = 1.0;
      np_set(arr2, indices, val);
    }
  }

  ndarray *sum_arr = np_add(arr, arr2);
  print_2d_array(sum_arr);
  printf("\n");

  // --- 5. Clean up ---
  printf("--- Cleaning up memory ---\n");
  np_free(arr);
  np_free(arr2);
  np_free(sum_arr);
  printf("Done.\n");

  return 0;
}