-- The legacy ring chain fed from the table-returning helpers.
-- neighbors() returns a Lua table; bondNetworkByIndex, getPrimitiveRings
-- and the ring analyses must accept it as well as a container userdata.
local dseams = require("dseams")
local core = dseams.core

local path = "input/traj/exampleTraj.lammpstrj"
local cloud = dseams.read(path, {type = 2})
assert(cloud.nop == 250, string.format("nop %d, want 250", cloud.nop))

local nList = dseams.neighbors(cloud, {cutoff = 3.5, type = 2})
assert(type(nList) == "table" and #nList == cloud.nop, "neighbour table shape")

local hbn = core.getHbondNetwork(path, cloud, nList, 1, 1)
assert(type(hbn) == "table" and #hbn == cloud.nop, "H-bond table shape")

local byIndex = core.bondNetworkByIndex(cloud, hbn)
assert(type(byIndex) == "table" and #byIndex == cloud.nop, "index table shape")
assert(byIndex[1][1] == 0, "first row must lead with its own index")

local rings = core.getPrimitiveRings(byIndex, 6)
assert(#rings > 0, "no primitive rings on the water frame")
for _, r in ipairs(rings) do
  assert(#r >= 3 and #r <= 6, "ring size outside 3..6")
end

local userdata_nlist = core.neighborList(3.5, cloud, 2)
local rings_ud = core.getPrimitiveRings(core.bondNetworkByIndex(cloud, userdata_nlist), 6)
assert(#rings_ud == #core.ringNetwork(core.neighbourListByIndex(cloud, nList), 6),
       "table and userdata paths disagree on the ring count")

local out = os.tmpname()
os.remove(out)
os.execute("mkdir -p " .. out)
core.prismAnalysis(out .. "/", rings, byIndex, cloud, 6, 1, 1, 1, false)
local f = io.open(out .. "/topoINT/nPrisms.dat", "r")
assert(f, "prismAnalysis wrote no nPrisms.dat")
f:close()
print(string.format("legacy_chain rings=%d", #rings))
