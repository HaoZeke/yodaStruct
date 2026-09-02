-- Topology keys and ions read against the cage assignment, through the
-- library API: a cubic-diamond lattice written as a LAMMPS dump with one
-- site turned into an ion (type 3), read back with every atom.
local dseams = require("dseams")
local core = dseams.core

local bond = 2.75
local a = 4.0 * bond / math.sqrt(3.0)
local reps = 3
local fcc = { { 0, 0, 0 }, { 0.5, 0.5, 0 }, { 0.5, 0, 0.5 }, { 0, 0.5, 0.5 } }
local positions = {}
for i = 0, reps - 1 do
  for j = 0, reps - 1 do
    for k = 0, reps - 1 do
      for _, b in ipairs(fcc) do
        positions[#positions + 1] = { (i + b[1]) * a, (j + b[2]) * a, (k + b[3]) * a }
        positions[#positions + 1] = { (i + b[1] + 0.25) * a, (j + b[2] + 0.25) * a, (k + b[3] + 0.25) * a }
      end
    end
  end
end
local box = reps * a

local path = os.tmpname()
local fh = assert(io.open(path, "w"))
fh:write("ITEM: TIMESTEP\n0\nITEM: NUMBER OF ATOMS\n", #positions, "\n")
fh:write("ITEM: BOX BOUNDS pp pp pp\n")
for _ = 1, 3 do fh:write(string.format("0.0 %.6f\n", box)) end
fh:write("ITEM: ATOMS id type x y z\n")
for idx, p in ipairs(positions) do
  local t = (idx == 1) and 3 or 1
  fh:write(string.format("%d %d %.6f %.6f %.6f\n", idx, t, p[1] % box, p[2] % box, p[3] % box))
end
fh:close()

local cloud = core.readLammpsTrj(path, 1)
os.remove(path)
assert(cloud.nop == #positions, "every atom read")

-- water-only graph and its rows by index
local nlist = core.neighListO(3.5, cloud, 1)
local rows = core.neighbourListByIndex(cloud, nlist)
local fp = dseams.fingerprint(rows, {hops = 2})
assert(fp.method == "nauty" or fp.method == "wl", fp.method)
local nclasses = 0
for _ in pairs(fp.classes) do nclasses = nclasses + 1 end
assert(nclasses >= 2, "a vacancy makes more than one class: " .. nclasses)
local lk = core.localTopologyKey(rows, 40, 2)
assert(lk.vertices > 0 and #lk.key == 16, "local key")

-- seeded cages on the water, then the ion against them
local strict = core.neighbourListByIndex(cloud, core.kNearestNeighbourList(cloud, 4, 5.0, 1, true))
local union = core.neighbourListByIndex(cloud, core.kNearestNeighbourList(cloud, 4, 5.0, 1, false))
local function six(rings)
  local out = {}
  for _, r in ipairs(rings) do if #r == 6 then out[#out + 1] = r end end
  return out
end
local aff = core.seededCageAffiliation(six(core.ringNetwork(strict, 6)), strict,
                                         six(core.ringNetwork(union, 6)), union, true)
local ice = {}
for i = 1, cloud.nop do ice[i] = (aff.hc[i] or aff.ddc[i]) and true or false end
local env = dseams.ion_environment(cloud, ice, { 0 }, {type = 1, cutoff = 3.5})
assert(env.nIce + env.nFront + env.nLiquid == 1, "one ion")
assert(env.shell[1] == 4, "four water neighbours at a lattice site, got " .. tostring(env.shell[1]))
assert(env.state[1] == "ice", "an ion at a lattice site of a labelled lattice is in ice: " .. env.state[1])
print(string.format("topology.lua ok: key=%s classes=%d method=%s ion=%s", fp.key, nclasses, fp.method, env.state[1]))
