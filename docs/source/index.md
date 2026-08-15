# dseams

Lua and Fennel library for the [d-SEAMS](https://dseams.info) C++ engine.

`require("dseams")` in Lua, `(require :dseams)` in Fennel. The engine
CLI is `seams` in [seams-core](https://github.com/d-SEAMS/seams-core).
Python is [pydseams](https://github.com/d-SEAMS/PydSEAMSlib).

```lua
local dseams = require("dseams")
local cloud = dseams.read("water.lammpstrj")
print(dseams.chill_plus(cloud, {cutoff = 3.5}))
```

Author the narrative in `docs/orgmode/` and export with
`emacs --batch --load export.el`.

```{toctree}
:maxdepth: 1
:caption: Getting Started

quickstart
```

```{toctree}
:maxdepth: 2
:caption: Reference

reference/lua
```
