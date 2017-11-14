(ns magic-powder.core
  (:require [defprecated.core :as depr])
  (:import java.io.Closeable
           java.util.concurrent.locks.ReentrantReadWriteLock
           [magic_powder ClosedException Hashtable]))

;; Check for reflection.
(set! *warn-on-reflection* true)

(declare hash-table?)

(defmacro with-lock [lock & body]
  `(let [lock# ~lock]
     (try
       (.lock lock#)
       ~@body
       (finally
         (.unlock lock#)))))

(defmacro with-read-lock [ht & body]
  `(with-lock (.readLock ^ReentrantReadWriteLock (:rwlock ~ht))
     ~@body))

(defrecord HashTable [htpa ^ReentrantReadWriteLock rwlock]
  java.io.Closeable
  (close [ht]
    (with-lock (.writeLock rwlock)
      (when @(:htpa ht)
        (Hashtable/unmapHashtable @(:htpa ht))
        (reset! (:htpa ht) nil)))))

(defn close [ht]
  {:pre [(hash-table? ht)]}
  (.close ^HashTable ht))

(defn- check-closed [ht]
  (when (nil? @(:htpa ht))
    (throw (ClosedException. "The hash table is closed."))))

(defn insert-bytes! [ht k v]
  {:pre [(hash-table? ht)
         (string? k)]}
  (with-read-lock ht
    (check-closed ht)
    (Hashtable/insert_bytes @(:htpa ht) k (byte-array v))))

(depr/defn insert-bytes [ht k v]
  (insert-bytes! ht k v))

(defn insert-doubles! [ht k v]
  {:pre [(hash-table? ht)
         (string? k)]}
  (with-read-lock ht
    (check-closed ht)
    (Hashtable/insert_doubles @(:htpa ht) k (double-array v))))

(depr/defn insert-doubles [ht k v]
  (insert-doubles! ht k v))

(defn get-bytes [ht k]
  {:pre [(hash-table? ht)
         (string? k)]}
  (with-read-lock ht
    (check-closed ht)
    (Hashtable/get_bytes @(:htpa ht) k)))

(defn get-doubles [ht k]
  {:pre [(hash-table? ht)
         (string? k)]}
  (with-read-lock ht
    (check-closed ht)
    (Hashtable/get_doubles @(:htpa ht) k)))

(defn hash-table? [x]
  (instance? HashTable x))

(defn open? [ht]
  {:pre [(hash-table? ht)]}
  (some? @(:htpa ht)))

(defn create-hash-table
  [key-size value-size capacity nbuckets & [filename]]
  (HashTable. (atom (Hashtable/makeHashtable key-size value-size capacity nbuckets
                                             filename))
              (ReentrantReadWriteLock.)))

(defn open-hash-table
  [filename]
  (HashTable. (atom (Hashtable/mapHashtable filename))
              (ReentrantReadWriteLock.)))
