(ns magic-powder.core
  (:import java.io.Closeable
           [magic_powder ClosedException Hashtable]))

(declare hash-table?)

(defrecord HashTable [htpa]
  java.io.Closeable
  (close [ht]
    (locking (:htpa ht)
      (when @(:htpa ht)
        (Hashtable/unmapHashtable @(:htpa ht))
        (reset! (:htpa ht) nil)))))

(defn close [ht]
  {:pre [(hash-table? ht)]}
  (.close ht))

(defn- check-closed [ht]
  (when (nil? @(:htpa ht))
    (throw (ClosedException. "The hash table is closed."))))

(defn insert-bytes [ht k v]
  {:pre [(hash-table? ht)
         (string? k)]}
  (check-closed ht)
  (Hashtable/insert_bytes @(:htpa ht) k (byte-array v)))

(defn insert-doubles [ht k v]
  {:pre [(hash-table? ht)
         (string? k)]}
  (check-closed ht)
  (Hashtable/insert_doubles @(:htpa ht) k (double-array v)))

(defn get-bytes [ht k]
  {:pre [(hash-table? ht)
         (string? k)]}
  (check-closed ht)
  (Hashtable/get_bytes @(:htpa ht) k))

(defn get-doubles [ht k]
  {:pre [(hash-table? ht)
         (string? k)]}
  (check-closed ht)
  (Hashtable/get_doubles @(:htpa ht) k))

(defn hash-table? [x]
  (instance? HashTable x))

(defn open? [ht]
  {:pre [(hash-table? ht)]}
  (some? @(:htpa ht)))

(defn create-hash-table
  [key-size value-size capacity nbuckets & [filename]]
  (HashTable. (atom (Hashtable/makeHashtable key-size value-size capacity nbuckets
                                             filename))))

(defn open-hash-table
  [filename]
  (HashTable. (atom (Hashtable/mapHashtable filename))))
