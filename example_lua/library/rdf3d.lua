local dseams = require("dseams")
local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
assert(cloud.nop > 0, "empty cloud")

local nbins = 20
local rdf = dseams.core.calcRDF3D(cloud, 2, 2, 6.0, nbins)
assert(type(rdf) == "table", "calcRDF3D should return a table")
assert(type(rdf.r) == "table" and type(rdf.g) == "table",
       "calcRDF3D should return r and g tables")
assert(#rdf.r == nbins and #rdf.g == nbins,
       string.format("bin count r=%d g=%d want %d", #rdf.r, #rdf.g, nbins))

local gmax = 0.0
for i = 1, nbins do
  assert(type(rdf.r[i]) == "number" and type(rdf.g[i]) == "number")
  assert(rdf.r[i] == rdf.r[i] and rdf.g[i] == rdf.g[i], "NaN in RDF")
  if i > 1 then
    assert(rdf.r[i] > rdf.r[i - 1], "r is not increasing")
  end
  if rdf.g[i] > gmax then
    gmax = rdf.g[i]
  end
end
assert(gmax > 0.0, "O-O g(r) is identically zero")

print(string.format("dseams_rdf3d nop=%d nbins=%d gmax=%.4f r1=%.4f",
                    cloud.nop, nbins, gmax, rdf.r[1]))
