local fennel = dofile("src/include/external/fennel/fennel.lua")
local dseams = fennel.dofile("lua/dseams.fnl")

assert(type(dseams.rdf) == "function")
assert(type(dseams["running-cn"]) == "function")
assert(type(dseams.hbonds) == "function")
assert(type(dseams.density) == "function")
assert(type(dseams.pairs) == "function")
assert(type(dseams.domain) == "function")

local cloud = dseams.read("input/traj/tiny-ions.lammpstrj", {all = true})
local profile = dseams.density(cloud, {type = 1, bins = 2, axis = "z"})
assert(#profile.centres == 2 and #profile.rho == 2)

local table = dseams["site-table"]("1=cationHead,2=anion")
assert(dseams.pairs(cloud, {table = table}).count == 2)
assert(dseams.domain(
  cloud,
  {table = table, kind = dseams.core.Kind.polar, cutoff = 1.5}
).largest == 2)

print("dseams_fennel_workflows density=2 pairs=2 domain=2")
