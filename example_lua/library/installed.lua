local lua_module = assert(package.searchpath("dseams", package.path))
local core_module = assert(package.searchpath("dseams_core", package.cpath))
assert(lua_module:match("/stage/"), "dseams.lua did not load from the staged install")
assert(core_module:match("/stage/"), "dseams_core did not load from the staged install")

local dseams = require("dseams")
local cloud = dseams.read("input/traj/tiny-ions.lammpstrj", {all = true})
assert(cloud.nop == 4)
assert(#dseams.rdf(cloud, {type_i = 1, type_j = 2, cutoff = 5.0, bins = 10}).r == 10)
assert(#dseams.running_cn(
  cloud, {type_i = 1, type_j = 2, cutoff = 5.0, bins = 10}
).cn == 10)
assert(#dseams.density(cloud, {type = 1, bins = 2}).rho == 2)

local table = dseams.site_table("1=cationHead,2=anion")
assert(dseams.pairs(cloud, {table = table}).count == 2)
assert(dseams.domain(
  cloud,
  {table = table, kind = dseams.core.Kind.polar, cutoff = 1.5}
).largest == 2)

print(string.format(
  "dseams_installed module=%s core=%s",
  lua_module,
  core_module
))
