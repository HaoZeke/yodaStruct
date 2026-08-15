# The 2020 executable and the 2.2 library

Call `require("dseams")`. The 2020 Deferred Structural Elucidation
Analysis for Molecular Simulations (`d-SEAMS`) release ran as an
executable named `yodaStruct`. 2.2 builds no such binary. Call
`seams` for the engine CLI, in
[seams-core](https://github.com/d-SEAMS/seams-core).

## The 2020 executable

The Journal of Chemical Information and Modeling paper (Goswami,
Goswami, and Singh, 2020, doi:10.1021/acs.jcim.0c00031) released
the same code as `yodaStruct`. A run looked like:

```bash
yodaStruct -c lua_inputs/config.yml
```

The binary parsed YAML, created a Lua state, registered C++ functions
as globals (`readFrameOnlyOne`, `neighborList`, and others), and
evaluated `vars.lua` plus `functions.lua`. The script never called
`require`. It lived inside the driver. The process owned the program.

This tree, at 2.0, rebuilt that driver as a Lua and Fennel front end:
`./bbdir/yodaStruct -c example.yml`. The same YAML schema, the same
globals, plus vendored Fennel for `.fnl` scripts.

`seams-core` already owns the engine CLI (`seams`). Two binaries for
one engine split the driver story.

## The 2.2 library

2.2 drops the driver from this tree. meson builds the shared module
`dseams_core`. `lua/dseams.lua` does `require("dseams_core")` and
exports `read`, `neighbors`, `knn`, `chill_plus`, `chill`, and
`cages`. A normal `lua` or Fennel process loads it:

```lua
local dseams = require("dseams")
```

`require("yoda")` returns the same table. Python already works this
way: [pydseams](https://github.com/d-SEAMS/PydSEAMSlib) loads as a
module. Helpers stay in the scripting language. The compiled module
exports the registration surface (`dseams_core`, `pydseams._core`).

## Why the driver left this tree

A library composes. A binary does not. You cannot `require` a process,
embed it in a Fennel REPL, or call it from a script that already owns
`main`.

One CLI belongs with the engine. Call `seams` in `seams-core`.
This repository does not grow a second one.

The 2020 YAML workflow (config plus globals) belongs to `seams`.
Run an ordinary file with `lua` after setting `LUA_PATH` and
`LUA_CPATH`, or after `nix develop`.

## What to call

| want | call |
|------|------|
| Lua / Fennel library | `require("dseams")` in this repository |
| old Lua name | `require("yoda")` (alias) |
| engine CLI | `seams` in `seams-core` |
| Python library | `pydseams` |
