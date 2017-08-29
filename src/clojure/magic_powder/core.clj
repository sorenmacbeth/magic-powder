(ns magic-powder.core
  (:import
   [magic_powder Hashtable]))

(def ^:const byte-array-class (class (byte-array 0)))

(def ^:const double-array-class (class (double-array 0)))

(defn- byte-array?
  [a]
  (instance? byte-array-class a))

(defn- double-array?
  [a]
  (instance? double-array-class a))

(defprotocol IHashTable
  (close [ht])
  (insert-bytes [ht k v])
  (get-bytes [ht k])
  (insert-doubles [ht k v])
  (get-doubles [ht k]))

(defrecord HashTable [htp]
  IHashTable
  (close [ht] (Hashtable/unmapHashtable htp))
  (insert-bytes [ht k v] (Hashtable/insert_bytes htp k v))
  (insert-doubles [ht k v] (Hashtable/insert_doubles htp k v))
  (get-bytes [ht k] (Hashtable/get_bytes htp k))
  (get-doubles [ht k] (Hashtable/get_doubles htp k)))

(defn hash-table? [x]
  (instance? HashTable x))

(defn create-hash-table
  [key-size value-size capacity nbuckets & [filename]]
  (HashTable. (Hashtable/makeHashtable key-size value-size capacity nbuckets
                                       filename)))

(defn open-hash-table

(defmacro with-hash-table [[var-name & args] & body]
  `(let [~var-name (open-hash-table ~@args)]
     (try
       ~@body
       (finally
         (close ~var-name)))))
  [filename]
  (HashTable. (Hashtable/mapHashtable filename)))
