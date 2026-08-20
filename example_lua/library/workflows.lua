local dseams = require("dseams")

local path = "input/traj/tiny-ions.lammpstrj"
local cloud = dseams.read(path, {all = true})
assert(cloud.nop == 4, string.format("all-atom read returned %d", cloud.nop))

local rdf = dseams.rdf(cloud, {type_i = 1, type_j = 2, cutoff = 5.0, bins = 10})
assert(#rdf.r == 10 and #rdf.g == 10, "RDF shape mismatch")

local running = dseams.running_cn(
  cloud, {type_i = 1, type_j = 2, cutoff = 5.0, bins = 10}
)
assert(#running.r == 10 and #running.cn == 10, "running CN shape mismatch")

local density = dseams.density(cloud, {type = 1, bins = 2, axis = "z"})
assert(density.axis == "z", "density axis mismatch")
assert(#density.centres == 2 and #density.rho == 2, "density shape mismatch")
assert(math.abs(density.rho[1] - 0.002) < 1e-12, "density normalization mismatch")

local table = dseams.site_table("1=cationHead,2=anion")
local pairs = dseams.pairs(cloud, {table = table})
assert(pairs.count == 2, string.format("pair count %d, want 2", pairs.count))
assert(pairs.n_cation == 2 and pairs.n_anion == 2, "ion counts mismatch")
assert(#pairs.pairs == 2, "pair list shape mismatch")

local domain = dseams.domain(
  cloud,
  {table = table, kind = dseams.core.Kind.polar, cutoff = 1.5}
)
assert(domain.site_kind == "polar", "domain kind mismatch")
assert(domain.n == 4 and domain.largest == 2, "domain component mismatch")
assert(math.abs(domain.percolation - 0.5) < 1e-12, "domain ratio mismatch")

assert(type(dseams.core.densityByType) == "function")
assert(type(dseams.core.densityByKind) == "function")
assert(type(dseams.core.mutualNearestUnlike) == "function")
assert(type(dseams.core.domainStats) == "function")

local water_path = "input/traj/exampleTraj.lammpstrj"
local water = dseams.read(water_path, {type = 2})
local hbonds = dseams.hbonds(
  water,
  {path = water_path, frame = 1, h_type = 1, cutoff = 3.5, type = 2}
)
assert(type(hbonds) == "table" and #hbonds == water.nop, "H-bond shape mismatch")

local ice = dseams.read("input/traj/mW_cubic.lammpstrj", {type = 1})
dseams.chill(ice, {type = 1, cutoff = 3.5})
local chill_plus = dseams.chill_plus(ice, {type = 1, cutoff = 3.5})
local cubic = 0
for _, label in ipairs(chill_plus) do
  if label == "cubic" then
    cubic = cubic + 1
  end
end
assert(cubic == ice.nop, string.format("CHILL+ cubic count %d, want %d", cubic, ice.nop))

local ok, message = pcall(function()
  dseams.read(path, {type = 9})
end)
assert(not ok, "empty type selection should fail")
assert(tostring(message):match("no atoms"), "empty selection error lacks context")

print(string.format(
  "dseams_workflows nop=%d pairs=%d domain=%d density=%d",
  cloud.nop,
  pairs.count,
  domain.largest,
  #density.rho
))
