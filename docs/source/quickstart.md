# Quickstart

Call `require("dseams")` from Lua, or `(require :dseams)` from
Fennel. 2.2 builds no `yodaStruct` executable. Call `seams` for the
engine CLI, in [seams-core](https://github.com/d-SEAMS/seams-core).

`require("yoda")` still resolves to `dseams`.

## Meson

From the repository root. `LUA_PATH` finds `lua/dseams.lua`.
`LUA_CPATH` finds `dseams_core.so` in the build directory.

```bash
meson setup bbdir --wrap-mode=nofallback
meson compile -C bbdir
LUA_PATH="$PWD/lua/?.lua;;" LUA_CPATH="$PWD/bbdir/?.so;;" \
  lua example_lua/library/read.lua
```

That script is:

```lua
local dseams = require("dseams")
local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
assert(cloud.nop > 0, "empty cloud")
print(string.format("dseams_lib nop=%d", cloud.nop))
```

## Nix

`nix develop` sets `LUA_PATH` and `LUA_CPATH` to the installed
library.

```bash
nix build
nix develop
lua example_lua/library/read.lua
```

## Fennel

Fennel loads the same Lua module:

```fennel
(local dseams (require :dseams))
(local cloud (dseams.read "input/traj/exampleTraj.lammpstrj" {:type 2}))
(print (dseams.chill_plus cloud {:cutoff 3.5 :type 2}))
```

How to invoke the vendored compiler and the kebab-case wrappers in
`lua/dseams.fnl` is in [Fennel](howto/fennel.md).
