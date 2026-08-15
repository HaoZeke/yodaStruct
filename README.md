# yodaStruct

Lua and Fennel front end for the [d-SEAMS](https://dseams.info) C++ engine
([`seams-core`](https://github.com/d-SEAMS/seams-core)).

This repository is the `yodaStruct` CLI. The C++ library lives in
`seams-core`. Python bindings live in
[`PydSEAMSlib`](https://github.com/d-SEAMS/PydSEAMSlib). Do not grow a
second engine here.

Needs Meson >= 1.3, a C++20 compiler, Eigen 3.4, BLAS, LAPACK, and Lua
5.3 or 5.4. The engine is pulled as a wrap-git of `seams-core`. Use
`--wrap-mode=nofallback` unless you have rustc and cbindgen for the
optional `.con` reader.

```bash
meson setup bbdir --wrap-mode=nofallback
meson compile -C bbdir
./bbdir/yodaStruct -c example_lua/full_api/config.yml
```

Scripts ending in `.fnl` go through the vendored Fennel compiler
(`src/include/external/fennel/fennel.lua`). Set `YODA_FENNEL_PATH` if the
binary is installed away from the source tree. Set `YODA_LUA_PATH` to the
directory that holds `yoda.lua` / `yoda.fnl` for an installed binary.

```lua
local yoda = require("yoda")
local cloud = yoda.read("input/traj/exampleTraj.lammpstrj", {type = 2})
print(yoda.chill_plus(cloud, {cutoff = 3.5, type = 2}))
```

```fennel
(local yoda (require :yoda-fnl))
(local cloud (yoda.read "input/traj/exampleTraj.lammpstrj" {:type 2}))
(print (yoda.chill-plus cloud {:cutoff 3.5 :type 2}))
```

`example_lua/idiomatic/` and `example_lua/idiomatic_fnl/` are the short
forms. Suffix dispatch covers LAMMPS, XYZ, `.con`, and chemfiles
(PDB/GRO/DCD) when those readers are linked.

Registered Lua functions are listed in [`docs/luaFunctions.md`](docs/luaFunctions.md).
Examples live in `example_lua/`. The small trajectories they read are
under `input/`.

# License

[MIT](LICENSE). Fennel is `src/include/external/fennel/LICENSE-fennel`.
sol2 is `src/include/external/sol/LICENSE.txt`. cxxopts is MIT in
`src/include/external/cxxopts.hpp`. rang is Unlicense in
`src/include/external/LICENSE-rang`.
