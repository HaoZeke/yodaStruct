;; Fennel bindings for dseams. (local dseams (require :dseams.fnl))
;; or (require :dseams-fnl) after the library is on fennel.path.

(local impl (require :dseams))

(fn read [path opts]
  (impl.read path opts))

(fn neighbors [cloud opts]
  (impl.neighbors cloud opts))

(fn neighbors-pair [cloud opts]
  (impl.neighbors_pair cloud opts))

(fn cn [cloud opts]
  (impl.cn cloud opts))

(fn rdf [cloud opts]
  (impl.rdf cloud opts))

(fn running-cn [cloud opts]
  (impl.running_cn cloud opts))

(fn knn [cloud opts]
  (impl.knn cloud opts))

(fn chill-plus [cloud opts]
  (impl.chill_plus cloud opts))

(fn chill [cloud opts]
  (impl.chill cloud opts))

(fn cages [cloud opts]
  (impl.cages cloud opts))

(fn hbonds [cloud opts]
  (impl.hbonds cloud opts))

(fn density [cloud opts]
  (impl.density cloud opts))

(fn site-table [spec]
  (impl.site_table spec))

(fn pairs [cloud opts]
  (impl.pairs cloud opts))

(fn domain [cloud opts]
  (impl.domain cloud opts))

{:read read
 :neighbors neighbors
 :neighbors-pair neighbors-pair
 :cn cn
 :rdf rdf
 :running-cn running-cn
 :knn knn
 :chill-plus chill-plus
 :chill chill
 :cages cages
 :hbonds hbonds
 :density density
 :site-table site-table
 :pairs pairs
 :domain domain
 :core impl.core}
