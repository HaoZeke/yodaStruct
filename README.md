# yodaStruct

Lua and Fennel front end for the [d-SEAMS](https://dseams.info) C++ engine
([`seams-core`](https://github.com/d-SEAMS/seams-core)).

This repository is the `yodaStruct` CLI. The C++ library lives in
`seams-core`. Python bindings live in
[`PydSEAMSlib`](https://github.com/d-SEAMS/PydSEAMSlib). Do not grow a
second engine here.

```bash
meson setup bbdir
meson compile -C bbdir
./bbdir/yodaStruct -c example_lua/full_api/config.yml
```

Scripts ending in `.fnl` go through the vendored Fennel compiler
(`src/include/external/fennel/fennel.lua`). Set `YODA_FENNEL_PATH` if the
binary is installed away from the source tree.

Registered Lua functions are listed in [`docs/luaFunctions.md`](docs/luaFunctions.md).
Examples live in `example_lua/`. The small trajectories they read are
under `input/`.

# License

MIT. Fennel is under its own license in `src/include/external/fennel/`.
sol2 is under its own license in `src/include/external/sol/`.
