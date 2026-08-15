# Changelog

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
