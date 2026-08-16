local dseams = require("dseams")
local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
assert(cloud.nop > 0, "empty cloud")

local nbins = 20
local rdf = dseams.core.calcRDF3D(cloud, 2, 2, 6.0, nbins)
local run = dseams.core.calcRunningCN(cloud, 2, 2, 6.0, nbins)
assert(type(run) == "table", "calcRunningCN should return a table")
assert(type(run.r) == "table" and type(run.cn) == "table",
       "calcRunningCN should return r and cn tables")
assert(#run.r == nbins and #run.cn == nbins,
       string.format("bin count r=%d cn=%d want %d", #run.r, #run.cn, nbins))

for i = 1, nbins do
  assert(type(run.r[i]) == "number" and type(run.cn[i]) == "number")
  assert(run.r[i] == run.r[i] and run.cn[i] == run.cn[i], "NaN in running CN")
  assert(run.r[i] == rdf.r[i], "running CN r disagrees with calcRDF3D")
  if i > 1 then
    assert(run.r[i] > run.r[i - 1], "r is not increasing")
    assert(run.cn[i] + 1e-12 >= run.cn[i - 1], "running CN decreased")
  end
end
assert(run.cn[1] >= 0.0, "running CN is negative")
assert(run.cn[nbins] > 0.0, "O-O running CN at 6 A is identically zero")

print(string.format("dseams_runningcn nop=%d nbins=%d cn_last=%.4f r1=%.4f",
                    cloud.nop, nbins, run.cn[nbins], run.r[1]))
