;; Fennel-facing names for the same helpers as lua/yoda.lua.
;; (local yoda (require :yoda-fnl)) after the CLI has registered the C++ surface.

(local impl (require :yoda))

(fn read [path opts]
  (impl.read path opts))

(fn neighbors [cloud opts]
  (impl.neighbors cloud opts))

(fn knn [cloud opts]
  (impl.knn cloud opts))

(fn chill-plus [cloud opts]
  (impl.chill_plus cloud opts))

(fn chill [cloud opts]
  (impl.chill cloud opts))

(fn cages [cloud opts]
  (impl.cages cloud opts))

{:read read
 :neighbors neighbors
 :knn knn
 :chill-plus chill-plus
 :chill chill
 :cages cages}
