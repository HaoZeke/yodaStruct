# dseams

Lua and Fennel library for the [d-SEAMS](https://dseams.info) C++ engine.

This repository is a **library**. There is no `yodaStruct` executable.
`require("dseams")` in Lua, `(require :dseams)` in Fennel. The engine
CLI is `seams` in [seams-core](https://github.com/d-SEAMS/seams-core).
Python is [pydseams](https://github.com/d-SEAMS/PydSEAMSlib).

```lua
local dseams = require("dseams")
local cloud = dseams.read("water.lammpstrj")
print(dseams.chill_plus(cloud, {cutoff = 3.5}))
```

`dseams.core` is the compiled registrations (`dseams_core.so`).
Helpers stay in Lua. `require("yoda")` still resolves to `dseams`.

```{toctree}
:maxdepth: 1
:caption: Getting Started

quickstart
```

```{toctree}
:maxdepth: 1
:caption: Tutorials

tutorials/read-and-classify
```

```{toctree}
:maxdepth: 1
:caption: How-To

howto/fennel
```

```{toctree}
:maxdepth: 1
:caption: Explanation

explanation/library-not-cli
```

```{toctree}
:maxdepth: 2
:caption: Reference

reference/lua
```

The long compiled-registration list is `docs/luaFunctions.md`
(appendix, not a Diataxis page).
