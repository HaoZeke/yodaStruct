# Quickstart

```bash
meson setup bbdir --wrap-mode=nofallback
meson compile -C bbdir
LUA_PATH="$PWD/lua/?.lua;;" LUA_CPATH="$PWD/bbdir/?.so;;" \
  lua example_lua/library/read.lua
```

Nix:

```bash
nix build
nix develop
lua example_lua/library/read.lua
```

```fennel
(local dseams (require :dseams))
(local cloud (dseams.read "water.lammpstrj"))
(print (dseams.chill_plus cloud {:cutoff 3.5}))
```
