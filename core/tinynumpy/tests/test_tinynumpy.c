#include "tinynumpy.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

#define RUN_TEST(test_func)                                                    \
  do {                                                                         \
    printf("Running " #test_func "... ");                                      \
    test_func();                                                               \
    printf("\033[0;32m[PASS]\033[0m\n");                                       \
  } while (0)

void test_np_array() {
  int shape[] = {2, 3};
  ndarray *a = np_array(2, shape);

  assert(a != NULL);
  assert(a->ndim == 2);
  assert(a->size == 6);
  assert(a->shape[0] == 2 && a->shape[1] == 3);

  np_free(a);
}

void test_np_zeros() {
  int shape[] = {3, 4, 5};
  ndarray *a = np_zeros(3, shape);

  assert(a != NULL);
  assert(a->ndim == 3);
  assert(a->size == 60);
  assert(a->shape[0] == 3 && a->shape[1] == 4 && a->shape[2] == 5);
  for (int i = 0; i < a->size; i++) {
    assert(fabs(a->data[i]) < 1e-9);
  }

  np_free(a);
}

void test_np_ones() {
  int shape[] = {2, 3, 6, 4};
  ndarray *a = np_ones(4, shape);

  assert(a != NULL);
  assert(a->ndim == 4);
  assert(a->size == 144);
  assert(a->shape[0] == 2 && a->shape[1] == 3 && a->shape[2] == 6 &&
         a->shape[3] == 4);
  for (int i = 0; i < a->size; i++) {
    assert(fabs(a->data[i] - 1.0) < 1e-9);
  }

  np_free(a);
}

void test_np_arange() {
  ndarray *a = np_arange(0.0, 5.0, 1);

  assert(a != NULL);
  assert(a->ndim == 1);
  assert(a->size == 5);
  assert(a->shape[0] == 5);
  for (int i = 0; i < a->size; i++) {
    assert(fabs(a->data[i] - i * 1.0) < 1e-9);
  }

  np_free(a);
}

void test_np_get() {
  int shape[] = {2, 3, 6};
  ndarray *a = np_array(3, shape);
  for (int i = 0; i < a->size; i++) {
    a->data[i] = (double)i;
  }

  int indices[3] = {1, 2, 3};
  assert(fabs(np_get(a, indices) - 33.0) < 1e-9);

  np_free(a);
}

void test_np_set() {
  int shape[] = {2, 3, 6};
  ndarray *a = np_array(3, shape);

  int indices[3] = {1, 2, 3};
  np_set(a, indices, 6.5);
  assert(fabs(np_get(a, indices) - 6.5) < 1e-9);

  np_free(a);
}

void test_np_add() {
  int shape[] = {6, 10, 5};
  ndarray *a = np_array(3, shape);
  ndarray *b = np_array(3, shape);
  for (int i = 0; i < a->size; i++) {
    a->data[i] = 1.0;
    b->data[i] = 2.0;
  }

  ndarray *res = np_add(a, b);
  for (int i = 0; i < a->size; i++) {
    assert(fabs(res->data[i] - 3.0) < 1e-9);
  }

  np_free(a);
  np_free(b);
  np_free(res);
}

void test_np_subtract() {
  int shape[] = {6, 10, 5};
  ndarray *a = np_array(3, shape);
  ndarray *b = np_array(3, shape);
  for (int i = 0; i < a->size; i++) {
    a->data[i] = 20.5;
    b->data[i] = 5.1;
  }

  ndarray *res = np_subtract(a, b);
  for (int i = 0; i < a->size; i++) {
    assert(fabs(res->data[i] - 15.4) < 1e-9);
  }

  np_free(a);
  np_free(b);
  np_free(res);
}

void test_np_matmul() {
  int shape_a[2] = {2, 3};
  int shape_b[2] = {3, 2};

  ndarray *a = np_array(2, shape_a);
  ndarray *b = np_array(2, shape_b);

  for (int i = 0; i < a->size; i++) {
    a->data[i] = i + 1.0;
    b->data[i] = i + 7.0;
  }

  ndarray *res = np_matmul(a, b);
  assert(fabs(res->data[0] - 58.0) < 1e-9);
  assert(fabs(res->data[1] - 64.0) < 1e-9);
  assert(fabs(res->data[2] - 139.0) < 1e-9);
  assert(fabs(res->data[3] - 154.0) < 1e-9);

  np_free(a);
  np_free(b);
  np_free(res);
}

int main() {
  printf("\n--- Starting TinyNumPy Tests ---\n\n");

  RUN_TEST(test_np_array);
  RUN_TEST(test_np_zeros);
  RUN_TEST(test_np_ones);
  RUN_TEST(test_np_arange);
  RUN_TEST(test_np_get);
  RUN_TEST(test_np_set);
  RUN_TEST(test_np_add);
  RUN_TEST(test_np_subtract);
  RUN_TEST(test_np_matmul);

  printf("\n\033[1;32mAll tests completed successfully!\033[0m ✨\n");
  return 0;
}