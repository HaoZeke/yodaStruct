# Read a dump and classify

You will read the LAMMPS dump that ships in this repository and ask
two different questions of the same oxygen cloud.

- `dseams.chill_plus` labels each oxygen from its four-neighbour
  bond order (CHILL+).
- `dseams.cages` flags whether that oxygen sits in a hexagonal
  cage (HC) or a double-diamond cage (DDC).

Those calls do not answer the same thing. The script prints both
tables. This repository does not pin the CHILL+ histogram, so you
read the counts you get.

Work from the repository root. The dump path is relative.

## Build

Meson. `LUA_PATH` finds `lua/dseams.lua`. `LUA_CPATH` finds
`dseams_core.so`.

```bash
meson setup bbdir --wrap-mode=nofallback
meson compile -C bbdir
export LUA_PATH="$PWD/lua/?.lua;;"
export LUA_CPATH="$PWD/bbdir/?.so;;"
```

Nix sets the same two variables to the installed library:

```bash
nix build
nix develop
```

If you already ran the [Quickstart](../quickstart.md), the
exports (or the Nix shell) are enough. Skip the compile.

## Read the dump

`input/traj/exampleTraj.lammpstrj` is one frame of 750 atoms: 500
of LAMMPS type 1 (hydrogen) and 250 of type 2 (oxygen). Ice
helpers classify oxygen.

The shipped reader is `example_lua/library/read.lua`:

```lua
local dseams = require("dseams")
local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
assert(cloud.nop > 0, "empty cloud")
print(string.format("dseams_lib nop=%d", cloud.nop))
```

```bash
lua example_lua/library/read.lua
```

You should see:

```text
dseams_lib nop=250
```

`dseams.read` keeps one atom type. Pass `{type = 2}` on this dump.
`chill_plus` and `cages` build neighbour graphs with
`opts.type or 1`, so they need `type = 2` as well. A cloud of
oxygens plus a type-1 neighbour filter is an empty graph.

## Classify

Save this as `/tmp/read-and-classify.lua`. It is the rest of the
tutorial.

```lua
local dseams = require("dseams")

local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
assert(cloud.nop > 0, "empty cloud")

local box = cloud:box()
print(string.format("nop=%d frame=%d box={%.2f, %.2f, %.2f}",
  cloud.nop, cloud.currentFrame, box[1], box[2], box[3]))

local types = dseams.chill_plus(cloud, {cutoff = 3.5, type = 2})
assert(#types == cloud.nop)

local counts = {}
for i = 1, #types do
  counts[types[i]] = (counts[types[i]] or 0) + 1
end
local parts = {}
for name, n in pairs(counts) do
  parts[#parts + 1] = string.format("%s=%d", name, n)
end
table.sort(parts)
print("chill_plus " .. table.concat(parts, " "))

local aff = dseams.cages(cloud, {type = 2})
assert(#aff.hc == cloud.nop and #aff.ddc == cloud.nop)

local function nflag(flags)
  local n = 0
  for i = 1, #flags do
    if flags[i] and flags[i] ~= 0 then
      n = n + 1
    end
  end
  return n
end
print(string.format("cages hc=%d ddc=%d nop=%d",
  nflag(aff.hc), nflag(aff.ddc), cloud.nop))
```

```bash
lua /tmp/read-and-classify.lua
```

The first line is the 250 oxygens, frame 1, and the box
(40 x 40 x 180 Angstrom, a long z). The ice-nanotube examples in
`example_lua/iceNanotube/` use this dump.

### CHILL+

`dseams.chill_plus` builds a 3.5 Angstrom cutoff neighbour list,
runs `getCorrelPlus`, and returns
`getIceTypePlusNoPrint`. No file is written. The cloud is mutated.
The return value is a 1-based array of state names, length
`cloud.nop`.

Names you can see: `cubic`, `hexagonal`, `water`, `interfacial`,
`clathrate`, `interClathrate`, `unclassified`.

CHILL+ looks at the four-neighbour shell. A bond with correlation
<= -0.8 is staggered; a bond in [-0.35, 0.25] is eclipsed.

- 4 staggered: cubic
- 3 staggered and 1 eclipsed: hexagonal
- 4 eclipsed: clathrate
- 3 eclipsed: interClathrate
- mixed ice-like bonds: interfacial
- anything else, or not 4-coordinated: water

Print the histogram. Add the counts. They sum to 250. This tree
does not assert a particular split. A confined water dump can
land mostly in `water` and `interfacial`. That is a
classification, not a failed run.

### Cages

`dseams.cages` builds two 4-nearest graphs (mutual = strict,
union = permissive; candidate cutoff 5.0), keeps six-membered
rings, and returns `seededCageAffiliation`: a table
`{hc = ..., ddc = ...}` of per-atom flags, each length
`cloud.nop`.

- `hc[i]` is true when oxygen `i` belongs to an accepted
  hexagonal cage.
- `ddc[i]` is true when it belongs to an accepted double-diamond
  cage.

HC and DDC are bulk ice motifs (two basal six-rings plus
prisms; one equatorial six-ring plus six peripherals). Seeded
affiliation accepts a permissive-graph atom only when its
affiliated component contains a mutual-graph seed. An empty
strict pass accepts nothing.

Print `hc` and `ddc` counts. Zero is a valid answer: this dump
is confined water, not a bulk ice lattice.

## Two questions

`chill_plus` is a local bond-order label. `cages` is six-ring
membership. A cubic oxygen need not sit in a DDC. An HC oxygen
need not be labeled hexagonal. Read both tables; do not expect
them to match.

The helpers live in `lua/dseams.lua`. The compiled names they
call are on `dseams.core`; see
[Lua surface](../reference/lua.md).

## Fennel

`(require :dseams)` is the same Lua table. Names stay
snake_case. The vendored compiler is
`src/include/external/fennel/fennel.lua`.

```fennel
(local dseams (require :dseams))
(local cloud (dseams.read "input/traj/exampleTraj.lammpstrj" {:type 2}))
(local types (dseams.chill_plus cloud {:cutoff 3.5 :type 2}))
(local aff (dseams.cages cloud {:type 2}))
(print (string.format "nop=%d ntypes=%d nhc=%d nddc=%d"
                      cloud.nop (length types) (length aff.hc)
                      (length aff.ddc)))
```

Save the snippet as `/tmp/read-and-classify.fnl`, or run the
shipped file `example_lua/library/chill.fnl` (that one stops
after `chill_plus`):

```bash
lua src/include/external/fennel/fennel.lua /tmp/read-and-classify.fnl
lua src/include/external/fennel/fennel.lua example_lua/library/chill.fnl
```

How Fennel finds `dseams`, and how `lua/dseams.fnl` adds
`chill-plus`, is in [Fennel](../howto/fennel.md).
