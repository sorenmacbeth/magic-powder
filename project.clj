(defproject yieldbot/magic-powder "0.1.0-SNAPSHOT"
  :repositories [["snapshots"
                  {:url "https://artifactory.yb0t.cc/artifactory/yieldbot-maven-snapshots"
                   :username :env/LEIN_USER
                   :password :env
                   :sign-releases false}]
                 ["releases"
                  {:url "https://artifactory.yb0t.cc/artifactory/yieldbot-maven-releases"
                   :username :env/LEIN_USER
                   :password :env
                   :sign-releases false}]]
  :dependencies [[org.clojure/clojure "1.8.0"]]
  :source-paths ["src/clojure"]
  :java-source-paths ["src/java"]
  :resource-paths ["target/resources"]
  :prep-tasks [["shell" "make"]
               "javac" "compile"]
  :profiles {:dev {:dependencies [[midje "1.8.3"]]
                   :plugins [[lein-midje "3.1.3"]
                             [lein-shell "0.5.0"]]}
             :uberjar {:aot :all}})
