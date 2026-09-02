# Changelog

## 2.8.1 (2026-09-02)

- Engine wrap and flake input pinned at seams-core v2.8.0; the 2.8.0 tag carried an empty revision.

## 2.8.0 (2026-09-02)

- `topologyFingerprint`, `localTopologyKey`, `topologyLibrary`,
  `classifyTopology` and `ionEnvironment` on the compiled surface;
  `dseams.fingerprint`, `dseams.topology_library`, `dseams.classify_topology`,
  `dseams.ion_environment` and `dseams.cages({complete = true})` in the
  library API; `example_lua/library/topology.lua` as a meson test.
- Lua API reference rewritten for the by-value getters, the completion
  argument and named keys on every table.
- Engine pinned at seams-core v2.8.0 with readcon-core v0.14.10.

## 2.7.0 (2026-09-02)

- `seededCageAffiliation` takes an optional fifth argument that turns on
  the engine's ring completion (fill the last vertex of a six-ring whose
  other vertices carry a label).
- Nested-table getters (`bondNetworkByIndex`, `getPrimitiveRings`,
  `prismAnalysis`, `bulkRingNumberAnalysis`) take their arguments by value,
  so Lua tables passed straight from the helpers no longer crash the
  binding; `example_lua/library/legacy_chain.lua` exercises the chain.
- Engine pinned at seams-core v2.7.0.

## Unreleased

`dseams.read` has no region and calls `readLammpsTrjO` (keeps every
atom of the type, sets `inSlice`). A dump slice that shrinks `nop`
is `dseams.core.readLammpsTrjreduced`. An axis with `lo == hi` is
unconstrained.

## 2.6.0

`subprojects/seams-core.wrap` is `v2.6.0`.

## 2.5.0

`subprojects/seams-core.wrap` is `v2.5.0`. `calcRunningCN` binds
`rdf::runningCN` (`{r, cn}`, `rhoJ = nJ / volume`). Ice-score
`--family`, `seams cn --ions`, `seams pairs`, `seams domains`,
and `seams density-z` live on the engine CLI.

## 2.4.1

`subprojects/seams-core.wrap` stays `v2.4.0`. `calcRDF3D` binds
`rdf::partialRdf`. `calcCN` / `dseams.cn` bind
`rdf::coordinationNumber`. `neighListPair`, `parseSiteSpec`,
`SiteTable`, `ionCloud`, `getHbondNetworkFromDonors`, and
`donatedHydrogenBond` are on `dseams.core`. The Meson build writes
a `lua.hpp` from the pkg-config 5.3/5.4 headers so sol does not
pick a distro 5.5 `lua.hpp`.

## 2.4.0

`subprojects/seams-core.wrap` is `v2.4.0`. Lua still has
`neighListO`, `populateHbonds` / `populateHbondsWithInputClouds`
(`getHbondNetwork` / `getHbondNetworkFromClouds`), and `calcRDF`
(`rdf2D`). This library does not bind `site::`, `rdf::`, or
`populateHbondsFromDonors`.

## 2.3.1

`subprojects/seams-core.wrap` is `v2.3.1`. Remaining cutoff
builders on the engine use vesin.

## 2.3.0

`subprojects/seams-core.wrap` is `v2.3.0`. `require("dseams")`
applies the engine twelve-factor table when the header is present.

The docs mark is the hexagonal ice cage with a primitive ring, as SVG.

## 2.2.5

`subprojects/seams-core.wrap` is `v2.2.5` (linkcell v0.2.4).
`dseams.knn` is unchanged.

## 2.2.4

`subprojects/seams-core.wrap` is `v2.2.4` (linked-cell k-nearest).
`dseams.knn` is unchanged. The compiled-registration page is
Doxygen of `lua_api.hpp`, not a hand list.

## 2.2.2

Shibuya docs from `docs/orgmode/`. The Lua surface is documented
there. The compiled-registration page is Doxygen of `lua_api.hpp`.

## 2.2.1

Flake-based Nix package for the `dseams` Lua library.

## 2.2.0

This repository is the Lua/Fennel **library** `dseams` (`luadseams`).
`require("dseams")` loads Lua helpers on `dseams_core`. There is no
`yodaStruct` executable. The engine CLI is `seams` in seams-core.

## 2.1.0

`require("yoda")` and `(require :yoda-fnl)` are the table-first helpers:
`read`, `neighbors`, `knn`, `chill_plus` / `chill-plus`, `cages`. Suffix
dispatch covers LAMMPS, XYZ, `.con`, and chemfiles formats when linked.
`getIceTypePlusNoPrint` is registered so CHILL+ does not write a file.

## 2.0.1

- `prismAnalysis` takes `atomID` by reference so later frames keep the
  first-frame ID.
- `templates/{hc,ddc}.xyz` and the selection/clathrate example dumps
  ship in this tree.
- `shellSeparation` is registered. Fennel installs with the binary.

## 2.0.0

First release of the `yodaStruct` CLI as its own repository. Lua and
Fennel front end for the d-SEAMS C++ engine. Fennel 1.5.3 is vendored
for `.fnl` scripts.

The engine is [seams-core](https://github.com/d-SEAMS/seams-core).
Python is [PydSEAMSlib](https://github.com/d-SEAMS/PydSEAMSlib).
