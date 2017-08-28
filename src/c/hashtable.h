#ifndef MP_HASHTABLE_H
#define MP_HASHTABLE_H

struct mp_hashtable;

#define MP_MAX_KEY_SIZE 100
#define MP_MAX_VALUE_SIZE 1000

struct mp_hashtable *mp_make_hashtable(int key_size, int value_size,
                                       int capacity, int nbuckets,
                                       const char *filename);
struct mp_hashtable *mp_map_hashtable(const char *filename);
int mp_unmap_hashtable(struct mp_hashtable *);

int mp_insert(struct mp_hashtable *ht, void *key, void *value);
void *mp_get(struct mp_hashtable *ht, void *key);

int mp_get_key_size(struct mp_hashtable *ht);
int mp_get_value_size(struct mp_hashtable *ht);

#define MP_ENOCAPACITY -1
#define MP_ENOBUCKETS -2
#define MP_EINVALIDKEY -3
#define MP_EALLOCATIONFAILED -4
#define MP_EINVALIDVALUE -5

#endif
