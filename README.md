# luadseams (`dseams`)

<p align="center">
  <img src="docs/source/_static/logo/dseams-icon.png" alt="dseams" width="96">
</p>

[![built with nix](https://builtwithnix.org/badge.svg)](https://builtwithnix.org)

Lua and Fennel library for the [d-SEAMS](https://dseams.info) C++ engine
([`seams-core`](https://github.com/d-SEAMS/seams-core)).

This repository is a **library**. `require("dseams")` in Lua,
`(require :dseams)` in Fennel. The engine CLI is `seams` in
`seams-core`. Python is the
[`pydseamslib`](https://github.com/d-SEAMS/PydSEAMSlib) package
(`import pydseams`).
Neighbour search is [`linkcell`](https://github.com/d-SEAMS/linkcell).

Released versions live in [CHANGELOG.md](CHANGELOG.md) (Keep a Changelog).
Unreleased notes are files under `changelog.d/`. The cut is
`towncrier build --version X.Y.Z`.

```lua
local dseams = require("dseams")
local cloud = dseams.read("water.lammpstrj")
print(dseams.chill_plus(cloud, {cutoff = 3.5}))
```

```fennel
(local dseams (require :dseams))
(local cloud (dseams.read "water.lammpstrj"))
(print (dseams.chill_plus cloud {:cutoff 3.5}))
```

`dseams.core` is the compiled registrations. Helpers stay in Lua, like
`pydseams` helpers stay in Python.

The public workflow surface covers all-atom and selected trajectory reads,
RDF and running-CN profiles, CHILL/CHILL+, cages, hydrogen bonds, Cartesian
density profiles, mutual ion pairs, and mapped-site domains. See the
[Lua reference](https://d-seams.github.io/yodaStruct/reference/lua.html) for
option tables and return values; the Fennel wrapper exposes the same workflows
with kebab-case names.

Build:

```bash
meson setup bbdir --wrap-mode=nofallback
meson compile -C bbdir
LUA_PATH="$PWD/lua/?.lua;;" LUA_CPATH="$PWD/bbdir/?.so;;" \
  lua example_lua/library/read.lua
```

Nix flake (meson, `require("dseams")`):

```bash
nix build
nix develop
lua example_lua/library/read.lua
```

`require("yoda")` still resolves to `dseams`.

`require("dseams")` applies the engine twelve-factor table
(`SEAMS_CONFIG` or `./seams.env`, then the environment). Occupancy
knobs (`LINKCELL_TPP`, `LINKCELL_BLOCK`, `SEAMS_RESIDENT`,
`SEAMS_CELL`) are getenv at the kernel. The leftover `yodaStruct -c
conf.yml` path is a script descriptor, not that table.

Docs: `docs/orgmode/` (ox-rst) and `docs/source/` (Shibuya).

# License

[MIT](LICENSE). Fennel is `src/include/external/fennel/LICENSE-fennel`.
sol2 is `src/include/external/sol/LICENSE.txt`.
