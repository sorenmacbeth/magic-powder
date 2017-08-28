(defproject yieldbot/magic-powder "0.1.0-SNAPSHOT"
  :dependencies [[org.clojure/clojure "1.8.0"]]
  :source-paths ["src/clojure" "test/magic_powder"]
  :java-source-paths ["src/java"]
  :profiles {:dev {:dependencies [[midje "1.8.3"]]
                   :plugins [[lein-midje "3.1.3"]]}
             :uberjar {:aot :all}})
