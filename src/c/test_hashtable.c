#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "hashtable.h"

int test_make_hashtable_file(void)
{
  const char *filename = "/tmp/testhashtable.ht";
  struct mp_hashtable *ht = mp_make_hashtable(24, 20, 10000, 5000, filename);
  if (!ht) {
    unlink(filename);
    return -1;
  }
  int *ints = (int *)ht;
  if (ints[0] != 24 ||
      ints[1] != 20 ||
      ints[2] != 10000 ||
      ints[3] != 0 ||
      ints[4] != 5000) {
    fprintf(stderr, "Created hashtable had unexpected header values:\n");
    for (int i = 0; i < 5; i++)
      fprintf(stderr, "- %d\n", ints[i]);
    /* Don't unmap hashtable since the header values neeeder for the
     * file size are bogus. */
    unlink(filename);
    return -1;
  }
  if (mp_unmap_hashtable(ht) == -1) {
    unlink(filename);
    return -1;
  }
  unlink(filename);
  return 0;
}

int test_make_hashtable_mem(void)
{
  struct mp_hashtable *ht = mp_make_hashtable(24, 20, 10000, 5000, NULL);
  if (!ht)
    return -1;
  int *ints = (int *)ht;
  if (ints[0] != 24 ||
      ints[1] != 20 ||
      ints[2] != 10000 ||
      ints[3] != 0 ||
      ints[4] != 5000) {
    fprintf(stderr, "Created in-memory hashtable had unexpected header values:\n");
    for (int i = 0; i < 5; i++)
      fprintf(stderr, "- %d\n", ints[i]);
    /* Don't unmap hashtable since the header values neeeder for the
     * file size are bogus. */
    return -1;
  }
  if (mp_unmap_hashtable(ht) == -1)
    return -1;
  return 0;
}

int test_map_hashtable(void)
{
  const char *filename = "/tmp/testhashtable.ht";
  struct mp_hashtable *ht;
  ht = mp_make_hashtable(24, 20, 10000, 5000, filename);
  if (!ht) {
    unlink(filename);
    return -1;
  }
  if (mp_unmap_hashtable(ht) == -1) {
    unlink(filename);
    return -1;
  }
  ht = mp_map_hashtable(filename);
  if (!ht) {
    unlink(filename);
    return -1;
  }
  int *ints = (int *)ht;
  if (ints[0] != 24 ||
      ints[1] != 20 ||
      ints[2] != 10000 ||
      ints[3] != 0 ||
      ints[4] != 5000) {
    fprintf(stderr, "Mapped hashtable had unexpected header values:\n");
    for (int i = 0; i < 5; i++)
      fprintf(stderr, "- %d\n", ints[i]);
    return -1;
  }
  if (mp_unmap_hashtable(ht) == -1) {
    unlink(filename);
    return -1;
  }
  unlink(filename);
  return 0;
}

int test_insert(void)
{
  struct mp_hashtable *ht;
  ht = mp_make_hashtable(24, 2 * sizeof(float), 10000, 5000, NULL);
  if (!ht)
    return -1;
  float value[2];
  value[0] = 2.4928040606765896;
  value[1] = 2213.2489533122025;
  int r = mp_insert(ht, "5d5d4f5f5e06cd00d5a5d03b", value);
  if (r != 0) {
    fprintf(stderr, "insert failed with code %d.\n", r);
    mp_unmap_hashtable(ht);
    return -1;
  }
  if (mp_unmap_hashtable(ht) == -1)
    return -1;
  return 0;
}

int test_get_notfound(void)
{
  struct mp_hashtable *ht;
  ht = mp_make_hashtable(24, 2 * sizeof(float), 10000, 5000, NULL);
  if (!ht)
    return -1;
  if (mp_get(ht, "5d5d4f5f5e06cd00d5a5d03b")) {
    fprintf(stderr, "get did not return the expected NULL.\n");
    mp_unmap_hashtable(ht);
    return -1;
  }
  if (mp_unmap_hashtable(ht) == -1)
    return -1;
  return 0;
}

int test_get_found(void)
{
  struct mp_hashtable *ht;
  ht = mp_make_hashtable(24, 2 * sizeof(float), 10000, 5000, NULL);
  if (!ht)
    return -1;
  float value[2];
  value[0] = 2.4928040606765896;
  value[1] = 2213.2489533122025;
  int r = mp_insert(ht, "5d5d4f5f5e06cd00d5a5d03b", value);
  if (r != 0) {
    fprintf(stderr, "insert failed with code %d.\n", r);
    mp_unmap_hashtable(ht);
    return -1;
  }

  const float *retrieved = mp_get(ht, "5d5d4f5f5e06cd00d5a5d03b");
  if (!retrieved) {
    fprintf(stderr, "get returned NULL.\n");
    mp_unmap_hashtable(ht);
    return -1;
  }
  if (retrieved[0] != value[0] ||
      retrieved[1] != value[1]) {
    fprintf(stderr, "Get returned the wrong value:\n");
    fprintf(stderr, "- %f instead of %f\n", retrieved[0], value[0]);
    fprintf(stderr, "- %f instead of %f\n", retrieved[1], value[1]);
    mp_unmap_hashtable(ht);
    return -1;
  }
  if (mp_unmap_hashtable(ht) == -1)
    return -1;
  return 0;
}

int test_insert_and_get_many(void)
{
  struct mp_hashtable *ht;
  ht = mp_make_hashtable(2 * sizeof(int), 2 * sizeof(int), 1000, 2000, NULL);
  if (!ht)
    return -1;

  unsigned seed = time(NULL);
  int key[2], value[2];
  const int *retrieved;
  int n = 1000;
  int i;

  srand(seed);
  for (i = 0; i < n; i++) {
    key[0] = rand(); key[1] = rand(); value[0] = rand(); value[1] = rand();
    int r = mp_insert(ht, key, value);
    if (r != 0) {
      fprintf(stderr, "insert failed with code %d.\n", r);
      mp_unmap_hashtable(ht);
      return -1;
    }
  }

  srand(seed);
  for (i = 0; i < n; i++) {
    key[0] = rand(); key[1] = rand(); value[0] = rand(); value[1] = rand();
    retrieved = mp_get(ht, key);
    if (!retrieved) {
      fprintf(stderr, "get returned NULL.\n");
      mp_unmap_hashtable(ht);
      return -1;
    }
    if (retrieved[0] != value[0] ||
        retrieved[1] != value[1]) {
      fprintf(stderr, "Get returned the wrong value:\n");
      fprintf(stderr, "- %d instead of %d\n", retrieved[0], value[0]);
      fprintf(stderr, "- %d instead of %d\n", retrieved[1], value[1]);
      mp_unmap_hashtable(ht);
      return -1;
    }
  }

  if (mp_unmap_hashtable(ht) == -1)
    return -1;
  return 0;
}

void test(int (*f)(void), const char *fname)
{
  int r = f();
  if (r)
    printf("%s returned %d\n", fname, r);
}

#define TEST(f) test(f, #f)

int main(int argc, char *argv[])
{
  TEST(test_make_hashtable_file);
  TEST(test_make_hashtable_mem);
  TEST(test_map_hashtable);
  TEST(test_insert);
  TEST(test_get_notfound);
  TEST(test_get_found);
  TEST(test_insert_and_get_many);
}
