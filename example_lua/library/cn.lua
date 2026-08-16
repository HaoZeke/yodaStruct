local dseams = require("dseams")
local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
assert(cloud.nop > 0, "empty cloud")

local cn = dseams.cn(cloud, {type_i = 2, type_j = 2, cutoff = 3.5, bins = 35})
assert(type(cn) == "number", "cn should be a number")
assert(cn == cn, "cn is NaN")
assert(cn > 0.0, "O-O CN at 3.5 A should be positive")

local table = dseams.core.parseSiteSpec("2=waterO")
assert(table:ofType(2) ~= table:ofType(1), "type 1 is not chemistry")

print(string.format("dseams_cn nop=%d cn=%.4f", cloud.nop, cn))
