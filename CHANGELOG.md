# Changelog

## Unreleased

`calcRDF3D` binds `rdf::partialRdf`. It takes a cloud, two type
IDs, `rmax`, and `nbins`, and returns `{r, g}` tables.
`calcRunningCN` binds `rdf::runningCN` on that histogram with
`rhoJ = nJ / volume` and returns `{r, cn}`.

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
