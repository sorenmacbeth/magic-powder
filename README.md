# magic-powder

Magic-powder is a Clojure library for creating and using a hashtable
that is backed by a memory-mapped file. You can create the hashtable
in one process, copy the file to another machine, and map the file
into another process there.

Mapping an existing hashtable is simply an mmap() call to map the file
into the process's address space. Compared to deserializing a clojure
map or nippy map, this uses very little CPU, and therefore little
interference with latency-sensitive processes.

Limitations:
* Keys are strings with a max size that must be specified when
  creating the hash table.
* Values are arrays of either bytes or doubles. The size must be
  specified when creating the hash table.
* The hash table's capacity (max. number of key–value pairs) and the
  number of hash buckets must be specified when creating the hash
  table and cannot be changed.
* There are no functions for modifying or deleting a key–value pair
  after it has been added.

Given these limitations, magic-powder works best in situations where
the hash table is created in batch, shipped, and then used in
read-only mode.

## Usage

``` clojure
;; project.clj
[yieldbot/magic-powder "0.1.0-SNAPSHOT"]

(require '[magic-powder :as mp])

;; Write bytes
(with-open [ht (create-hash-table 3 4 100 200
                                  "/tmp/bytes.ht")]
  (insert-bytes ht "foo"
                   (.getBytes "quux" "UTF-8")))

;; Read bytes
(with-open [ht (open-hash-table "/tmp/bytes.ht")]
  (vec (get-bytes ht "foo")))
;; => [113 117 117 120]

;; Write doubles
(with-open [ht (create-hash-table 3 16 100 200
                                  "/tmp/doubles.ht")]
  (insert-bytes ht "foo" [3.14 2.718]))
```

## Copyright

Copyright © 2017 Yieldbot, Inc.
