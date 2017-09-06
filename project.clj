(defproject yieldbot/magic-powder "0.2.0-SNAPSHOT"
  :description "Clojure library for off-heap hash table backed by a memory-mapped file"
  :url "http://github.com/yieldbot/magic-powder"
  :license {:name "Eclipse Public License"
            :url "http://www.eclipse.org/legal/epl-v10.html"}
  :dependencies [[org.clojure/clojure "1.8.0"]]
  :source-paths ["src/clojure"]
  :java-source-paths ["src/java"]
  :resource-paths ["target/resources"]
  :prep-tasks [["shell" "make"]
               "javac" "compile"]
  :profiles {:dev {:dependencies [[midje "1.8.3"]]
                   :plugins [[lein-midje "3.1.3"]
                             [lein-shell "0.5.0"]]}
             :uberjar {:aot :all}}
  :release-tasks [["vcs" "assert-committed"]
                  ["change" "version" "leiningen.release/bump-version" "release"]
                  ["vcs" "commit"]
                  ["vcs" "tag"]
                  ["deploy" "clojars"]
                  ["change" "version" "leiningen.release/bump-version"]
                  ["vcs" "commit"]
                  ["vcs" "push"]])
