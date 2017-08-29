#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "hashtable.h"

struct mp_hashtable {
  int key_size;
  int value_size;
  int capacity;
  int fill;
  int nbuckets;
};

typedef int hashcode_t;

static size_t compute_file_size(int key_size, int value_size, int capacity,
                         int nbuckets)
{
  return (sizeof(struct mp_hashtable) +
          nbuckets * sizeof(hashcode_t) +
          capacity * (key_size + value_size));
}

struct mp_hashtable *mp_make_hashtable(int key_size, int value_size,
                                       int capacity, int nbuckets,
                                       const char *filename)
{
  off_t file_size = compute_file_size(key_size, value_size,
                                      capacity, nbuckets);
  int fd;
  if (filename) {
    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
      perror("open");
      return NULL;
    }
    if (ftruncate(fd, file_size) == -1) {
      perror("ftruncate");
      if (close(fd) == -1)
        perror("close");
      return NULL;
    }
  } else {
    fd = -1;
  }
  /* We need MAP_SHARED here to ensure that modified contents are
   * written back to the file after the hashtable is unmapped. */
  struct mp_hashtable *ht = mmap(NULL, file_size, PROT_READ | PROT_WRITE,
                              (fd == -1 ? MAP_ANON : MAP_FILE) | MAP_SHARED,
                              fd, 0);
  if ((void *)ht == MAP_FAILED) {
    perror("mmap");
    if (filename)
      if (unlink(filename) == -1)
        perror("unlink");
    if (fd != -1)
      if (close(fd) == -1)
        perror("close");
    return NULL;
  }
  if (fd != -1)
    if (close(fd) == -1) {
      perror("close");
      if (munmap(ht, file_size) == -1)
        perror("munmap");
      return NULL;
    }
  ht->key_size = key_size;
  ht->value_size = value_size;
  ht->capacity = capacity;
  ht->fill = 0; /* Redundant, should be 0 already. */
  ht->nbuckets = nbuckets;
  return ht;
}

struct mp_hashtable *mp_map_hashtable(const char *filename)
{
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return NULL;
  }
  /* Stat the file to determine its size. */
  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    if (close(fd) == -1)
      perror("close");
    return NULL;
  }
  struct mp_hashtable *ht = mmap(NULL, sb.st_size, PROT_READ,
                              MAP_FILE | MAP_SHARED, fd, 0);
  if ((void *)ht == MAP_FAILED) {
    perror("mmap");
    if (close(fd) == -1)
      perror("close");
    return NULL;
  }
  if (close(fd) == -1) {
    perror("close");
    return NULL;
  }
  return ht;
}

int mp_unmap_hashtable(struct mp_hashtable *ht)
{
  off_t file_size = compute_file_size(ht->key_size, ht->value_size,
                                      ht->capacity, ht->nbuckets);
  if (munmap(ht, file_size) == -1) {
    perror("munmap");
    return -1;
  }
  return 0;
}

static hashcode_t hash(int key_size, char *key)
{
  int h = 0;
  while (key_size--)
    h = 31 * h + *key++;
  return h;
}

static hashcode_t *get_buckets(struct mp_hashtable *ht)
{
  return (int *)((char *)ht + sizeof(struct mp_hashtable));
}

static char *get_item(struct mp_hashtable *ht, int index)
{
  return ((char *)ht +
          sizeof(struct mp_hashtable) +
          ht->nbuckets * sizeof(hashcode_t) +
          index * (ht->key_size + ht->value_size));
}

static int get_bucket_index(struct mp_hashtable *ht, void *key)
{
  return (unsigned)hash(ht->key_size, key) % ht->nbuckets;
}

int mp_insert(struct mp_hashtable *ht, void *key, void *value)
{
  if (ht->fill == ht->capacity)
    return MP_ENOCAPACITY;
  int *buckets = get_buckets(ht);
  int initial_bucket_index = get_bucket_index(ht, key);
  int b = initial_bucket_index;
  while (buckets[b]) {
    b = (b + 1) % ht->nbuckets;
    if (b == initial_bucket_index)
      return MP_ENOBUCKETS;
  }
  /* Store the index into the item array, but make it 1-indexed so
   * that the initial value of zero can continue to mean empty. */
  buckets[b] = ht->fill + 1;

  char *item = get_item(ht, ht->fill);
  memcpy(item, key, ht->key_size);
  memcpy(item + ht->key_size, value, ht->value_size);

  ht->fill++;
  return 0;
}

const void *mp_get(struct mp_hashtable *ht, void *key)
{
  int *buckets = get_buckets(ht);
  int initial_bucket_index = get_bucket_index(ht, key);
  int b = initial_bucket_index;
  char *item;
  for (;;) {
    if (buckets[b] == 0)
      return NULL; /* Not found */
    /* Subtract one because the item index is 1-indexed. */
    item = get_item(ht, buckets[b] - 1);
    if (memcmp(item, key, ht->key_size) == 0)
      break; /* Found */
    b = (b + 1) % ht->nbuckets;
    if (b == initial_bucket_index)
      return NULL; /* Not found and no empty buckets. */
  }
  return item + ht->key_size;
}

int mp_get_key_size(struct mp_hashtable *ht)
{
  return ht->key_size;
}

int mp_get_value_size(struct mp_hashtable *ht)
{
  return ht->value_size;
}
