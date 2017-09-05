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

Writing to or reading from a closed hash table causes a
magic_powder.ClosedException.


## Build

Magic-powder contains a shared library written in C. The Clojure code
uses the Java Native Interface (JNI) to call the C code.

The various Leiningen commands (e.g., `lein uberjar`) should build the
C library first and store the shared library under
`target/resources`. At run time, the shared library is read from the
resources, written to a temporary file, and loaded.

Because the shared library consists of native code for the platform on
which it was compiled, the uberjar built this way will only work on
that platform.

### Build for multiple platforms

If the environment variable `MAGIC_POWDER_BUILD_FAT_UBERJAR` is set,
then the various Leiningen commands will run `make fat` instead of
just `make`. This will attempt to build shared libraries for both
macOS and Linux. This will only work if run on macOS with docker.


## License

Copyright © 2017 Yieldbot, Inc.

Distributed under the Eclipse Public License either version 1.0 or (at
your option) any later version.
