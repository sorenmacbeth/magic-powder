(ns magic-powder.core-test
  (:require [clojure.java.io :as io]
            [clojure.pprint :refer :all]
            [clojure.test :refer :all]
            [magic-powder.core :refer :all]
            midje.repl
            [midje.sweet :refer :all]
            midje.util)
  (:import [magic_powder ClosedException NoCapacityException]))

(facts "making, mapping, and unmapping"
  (fact "hash tables can be made in memory"
    (with-open [ht (create-hash-table 10 16 100 200)]
      ht => hash-table?))
  (fact "hash tables can be made on disk"
    (with-open [ht (create-hash-table 10 16 100 200 "/tmp/foo.ht")]
      ht => hash-table?)))

(facts "inserting"
  (fact "bytes"
    (with-open [ht (create-hash-table 3 10 100 200)]
      (insert-bytes! ht "foo"
                     (.getBytes "quux" "UTF-8"))))
  (fact "works for string key and double array value"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (insert-doubles! ht "foo" (double-array [3.14 2.7]))
      => anything))
  (fact "when out of capacity throws an exception"
    (with-open [ht (create-hash-table 10 16 1 10)]
      (insert-doubles! ht "foo" (double-array [3.14 2.7]))
      => anything
      (insert-doubles! ht "bar" (double-array [3.14 2.7]))
      => (throws NoCapacityException)))
  (fact "when out of buckets throws an exception"
    (with-open [ht (create-hash-table 10 16 10 1)]
      (insert-doubles! ht "foo" (double-array [3.14 2.7]))
      => anything
      (insert-doubles! ht "bar" (double-array [3.14 2.7]))
      => (throws NoCapacityException))))

(facts "getting"
  (fact "missing key from empty hash table"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (get ht "foo")
      => nil))
  (fact "get bytes"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (insert-bytes! ht "foo" (.getBytes "bar" "UTF-8"))
      (subs (String. (get-bytes ht "foo") "UTF-8")
            0 3)
      => "bar"))
  (fact "get doubles"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (insert-doubles! ht "foo" (double-array [3.14 2.7]))
      (vec (get-doubles ht "foo"))
      => [3.14 2.7])))

(facts "format validation"
  (fact "magic number"
    (io/copy (byte-array [0x7d 0xc8 0xcd 0x5b 1 0 0 0 8 0 0 0 16 0 0 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0])
             (io/file "/tmp/bar.ht"))
    (open-hash-table "/tmp/bar.ht")
    => (throws java.io.IOException "File has incorrect magic number"))
  (fact "format version"
    (io/copy (byte-array [0x7c 0xc8 0xcd 0x5b 2 0 0 0 8 0 0 0 16 0 0 0 1 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0])
             (io/file "/tmp/baz.ht"))
    (open-hash-table "/tmp/baz.ht")
    => (throws java.io.IOException "File has incorrect version number")))

(facts "closed hashtables"
  (fact "are open? before they're closed"
    (with-open [ht (create-hash-table 10 20 1 2)]
      (open? ht))
    => truthy)
  (fact "are not open? after they're closed"
    (open? (with-open [ht (create-hash-table 10 20 1 2)]
             ht))
    => falsey)
  (fact "can be closed again"
    (let [ht (create-hash-table 10 20 1 2)]
      (.close ht)
      (.close ht))
    => falsey)
  (fact "throw ClosedException"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (.close ht)
      (insert-bytes! ht "foo" (.getBytes "quux" "UTF-8"))
      => (throws ClosedException)
      (insert-doubles! ht "foo" (double-array [3.14 2.7]))
      => (throws ClosedException)
      (get-bytes ht "foo")
      => (throws ClosedException)
      (get-doubles ht "foo")
      => (throws ClosedException))))
