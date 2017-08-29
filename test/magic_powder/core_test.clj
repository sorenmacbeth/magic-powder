(ns magic-powder.core-test
  (:use midje.sweet
        clojure.pprint)
  (:require [clojure.java.io :as io]
            [clojure.test :refer :all]
            [magic-powder.core :refer :all]
            [midje.util]
            [midje.repl]))

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
      (insert-bytes ht "foo"
              (.getBytes "quux" "UTF-8"))))
  (fact "works for string key and double array value"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (insert-doubles ht "foo" (double-array [3.14 2.7]))
      => anything)))

(facts "getting"
  (fact "missing key from empty hash table"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (get ht "foo")
      => nil))
  (fact "get bytes"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (insert-bytes ht "foo" (.getBytes "bar" "UTF-8"))
      (subs (String. (get-bytes ht "foo") "UTF-8")
            0 3)
      => "bar"))
  (fact "get doubles"
    (with-open [ht (create-hash-table 10 16 100 200)]
      (insert-doubles ht "foo" (double-array [3.14 2.7]))
      (vec (get-doubles ht "foo"))
      => [3.14 2.7])))
