#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include "javahashtable.h"
#include "hashtable.h"

JNIEXPORT jlong JNICALL Java_magic_1powder_Hashtable_makeHashtable(JNIEnv *env, jclass class,
                                                     jint key_size, jint value_size,
                                                     jint capacity, jint nbuckets,
                                                     jstring jfilename) {
  assert(env);
  const char *filename = jfilename ? (*env)->GetStringUTFChars(env, jfilename, NULL) : NULL;
  struct mp_hashtable *ht = mp_make_hashtable(key_size, value_size, capacity, nbuckets, filename);
  if (jfilename)
    (*env)->ReleaseStringUTFChars(env, jfilename, filename);
  if (!ht) {
    jclass jc = (*env)->FindClass(env, "java/io/IOException");
    assert (jc);
    (*env)->ThrowNew(env, jc, strerror(errno));
    return 0;
  }
  return (jlong)ht;
}

JNIEXPORT jlong JNICALL Java_magic_1powder_Hashtable_mapHashtable(JNIEnv *env, jclass class,
                                                   jstring jfilename) {
  const char *filename = (*env)->GetStringUTFChars(env, jfilename, NULL);
  struct mp_hashtable *ht = mp_map_hashtable(filename);
  (*env)->ReleaseStringUTFChars(env, jfilename, filename);
  if (!ht) {
    jclass jc = (*env)->FindClass(env, "java/io/IOException");
    assert (jc);
    (*env)->ThrowNew(env, jc, strerror(errno));
    return 0;
  }
  return (jlong)ht;
}

JNIEXPORT void JNICALL Java_magic_1powder_Hashtable_unmapHashtable(JNIEnv *env, jclass class,
                                                     jlong hashtable) {
  jint rv = mp_unmap_hashtable((struct mp_hashtable *)hashtable);
  if (rv == -1) {
    jclass jc = (*env)->FindClass(env, "java/io/IOException");
    assert (jc);
    (*env)->ThrowNew(env, jc, strerror(errno));
    return;
  }
}

static void throw_no_capacity_exception(JNIEnv *env)
{
    jclass jc = (*env)->FindClass(env, "magic_powder/NoCapacityException");
    assert(jc);
    (*env)->ThrowNew(env, jc, "The hash table is out of space for items.");
    return;
}

static void throw_no_buckets_exception(JNIEnv *env)
{
    jclass jc = (*env)->FindClass(env, "magic_powder/NoCapacityException");
    assert(jc);
    (*env)->ThrowNew(env, jc, "The hash table is out buckets.");
    return;
}

void check_insert_return_value(JNIEnv *env, int r)
{
  switch (r) {
  case 0:
    return;
  case MP_ENOCAPACITY:
    throw_no_capacity_exception(env);
  case MP_ENOBUCKETS:
    throw_no_buckets_exception(env);
  default:
    abort();
  }
}

static void pad_key(JNIEnv *env, char *fixed_size_key, jstring jkey,
                    int key_size)
{
  const char *key = (*env)->GetStringUTFChars(env, jkey, NULL);
  int nk = strlen(key);
  if (nk > key_size) {
    (*env)->ReleaseStringUTFChars(env, jkey, key);
    jclass jc = (*env)->FindClass(env, "magic_powder/InvalidKeyException");
    assert(jc);
    (*env)->ThrowNew(env, jc, "The key string was longer than the hash table's key size.");
    return;
  }
  strncpy(fixed_size_key, key, key_size);
  (*env)->ReleaseStringUTFChars(env, jkey, key);
}

JNIEXPORT void JNICALL Java_magic_1powder_Hashtable_insert_1bytes
(JNIEnv *env, jclass class, jlong hashtable, jstring key,
 jbyteArray value)
{
  struct mp_hashtable *ht = (struct mp_hashtable *)hashtable;
  char fixed_size_key[MP_MAX_KEY_SIZE] = { 0 };
  pad_key(env, fixed_size_key, key, mp_get_key_size(ht));

  jbyte *v = (*env)->GetByteArrayElements(env, value, NULL);
  jint r = mp_insert(ht, fixed_size_key, v);
  (*env)->ReleaseByteArrayElements(env, key, v, JNI_ABORT);
  check_insert_return_value(env, r);
}

JNIEXPORT void JNICALL Java_magic_1powder_Hashtable_insert_1doubles
(JNIEnv *env, jclass class, jlong hashtable, jstring key,
 jdoubleArray jvalues)
{
  struct mp_hashtable *ht = (struct mp_hashtable *)hashtable;
  char fixed_size_key[MP_MAX_KEY_SIZE] = { 0 };
  pad_key(env, fixed_size_key, key, mp_get_key_size(ht));

  jdouble *values = (*env)->GetDoubleArrayElements(env, jvalues, NULL);
  int nv = (*env)->GetArrayLength(env, jvalues);
  int value_size = mp_get_value_size(ht);
  if (value_size / sizeof(jdouble) != nv) {
    (*env)->ReleaseDoubleArrayElements(env, jvalues, values, JNI_ABORT);
    jclass jc = (*env)->FindClass(env, "magic_powder/InvalidValueException");
    assert(jc);
    char message[255];
    sprintf(message,
            "The value's %d doubles @ %d bytes each did not fit exactly the hash table's value size of %d bytes.",
            nv, (int)sizeof(jdouble), value_size);
    (*env)->ThrowNew(env, jc, message);
    return;
  }
  int r = mp_insert(ht, fixed_size_key, values);
  (*env)->ReleaseDoubleArrayElements(env, jvalues, values, JNI_ABORT);
  check_insert_return_value(env, r);
}

JNIEXPORT jbyteArray JNICALL Java_magic_1powder_Hashtable_get_1bytes
(JNIEnv *env, jclass class, jlong hashtable, jbyteArray key)
{
  struct mp_hashtable *ht = (struct mp_hashtable *)hashtable;
  char fixed_size_key[MP_MAX_KEY_SIZE] = { 0 };
  pad_key(env, fixed_size_key, key, mp_get_key_size(ht));
  jbyte *v = mp_get(ht, fixed_size_key);
  if (v) {
    int value_size = mp_get_value_size((struct mp_hashtable *)hashtable);
    jbyteArray value = (*env)->NewByteArray(env, value_size);
    (*env)->SetByteArrayRegion(env, value, 0, value_size, v);
    return value;
  } else
    return NULL;
}

JNIEXPORT jbyteArray JNICALL Java_magic_1powder_Hashtable_get_1doubles
(JNIEnv *env, jclass class, jlong hashtable, jbyteArray key)
{
  struct mp_hashtable *ht = (struct mp_hashtable *)hashtable;
  char fixed_size_key[MP_MAX_KEY_SIZE] = { 0 };
  pad_key(env, fixed_size_key, key, mp_get_key_size(ht));
  jdouble *v = mp_get(ht, fixed_size_key);
  if (v) {
    int value_size = mp_get_value_size(ht);
    int nv = value_size / sizeof(jdouble);
    jdoubleArray value = (*env)->NewDoubleArray(env, nv);
    (*env)->SetDoubleArrayRegion(env, value, 0, nv, v);
    return value;
  } else
    return NULL;
}

JNIEXPORT jdoubleArray JNICALL Java_magic_1powder_Hashtable_get_1s_1f(JNIEnv *env, jclass class,
                                                        jlong hashtable, jstring jkey)
{
  struct mp_hashtable *ht = (struct mp_hashtable *)hashtable;

  const char *key = (*env)->GetStringUTFChars(env, jkey, NULL);
  int nk = strlen(key);
  int key_size = mp_get_key_size(ht);
  if (nk > key_size) {
    (*env)->ReleaseStringUTFChars(env, jkey, key);
    fprintf(stderr, "String exceeds key size of %d: %s\b", key_size, key);
    return NULL;
  }
  char fixed_size_key[MP_MAX_KEY_SIZE] = { 0 };
  strncpy(fixed_size_key, key, key_size);
  (*env)->ReleaseStringUTFChars(env, jkey, key);

  jdouble *v = mp_get(ht, fixed_size_key);
  if (!v)
    return NULL;

  int value_size = mp_get_value_size(ht);
  int nv = value_size / sizeof(double);
  jdoubleArray value = (*env)->NewDoubleArray(env, nv);
  (*env)->SetDoubleArrayRegion(env, value, 0, nv, v);
  return value;
}
