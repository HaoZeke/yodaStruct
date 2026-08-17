-- dseams: Lua front end for the d-SEAMS engine.
-- require("dseams"). The compiled surface is dseams_core.

local core = require("dseams_core")
local dseams = {core = core}

local function suffix(path)
  return (path:match("%.([^%./]+)$") or ""):lower()
end

local function opts(t)
  return t or {}
end

-- No region. A shrinking slice is core.readLammpsTrjreduced.
function dseams.read(path, o)
  o = opts(o)
  local frame = o.frame or 1
  local ext = suffix(path)
  if ext == "xyz" then
    return core.readXYZ(path)
  end
  if ext == "con" then
    if core.readCon == nil then
      error("readCon is not in this build (readcon-core missing)")
    end
    return core.readCon(path, frame)
  end
  if ext == "pdb" or ext == "gro" or ext == "dcd" then
    if core.readChemfiles == nil then
      error("readChemfiles is not in this build (chemfiles missing)")
    end
    return core.readChemfiles(path, frame, o.type or -1)
  end
  local typ = o.type
  if typ == nil then
    local cloud = core.readLammpsTrjO(path, frame, 2)
    if cloud.nop > 0 then
      return cloud
    end
    return core.readLammpsTrjO(path, frame, 1)
  end
  return core.readLammpsTrjO(path, frame, typ)
end

function dseams.neighbors(cloud, o)
  o = opts(o)
  return core.neighListO(o.cutoff or 3.5, cloud, o.type or 1)
end

function dseams.neighbors_pair(cloud, o)
  o = opts(o)
  return core.neighListPair(o.cutoff or 3.5, cloud, o.type_i or 1,
                            o.type_j or 2)
end

function dseams.cn(cloud, o)
  o = opts(o)
  local rmax = o.cutoff or 4.5
  local bins = o.bins or math.max(1, math.floor(rmax / 0.1))
  return core.calcCN(cloud, o.type_i or 1, o.type_j or 2, rmax, bins, rmax)
end

function dseams.knn(cloud, o)
  o = opts(o)
  local mutual = true
  if o.mutual == false then
    mutual = false
  end
  return core.kNearestNeighbourList(cloud, o.k or 4, o.cutoff or 5.0,
                                    o.type or 1, mutual)
end

function dseams.chill_plus(cloud, o)
  o = opts(o)
  local nl = dseams.neighbors(cloud, o)
  core.getCorrelPlus(cloud, nl, false)
  return core.getIceTypePlusNoPrint(cloud, nl, false)
end

function dseams.chill(cloud, o)
  o = opts(o)
  local nl = dseams.neighbors(cloud, o)
  core.getCorrel(cloud, nl, false)
  return core.getIceTypeNoPrint(cloud, nl, false)
end

function dseams.cages(cloud, o)
  o = opts(o)
  local typ = o.type or 1
  local k = o.k or 4
  local cut = o.cutoff or 5.0
  local mutual = dseams.knn(cloud, {k = k, cutoff = cut, type = typ, mutual = true})
  local union = dseams.knn(cloud, {k = k, cutoff = cut, type = typ, mutual = false})
  local idx_s = core.neighbourListByIndex(cloud, mutual)
  local idx_u = core.neighbourListByIndex(cloud, union)
  local six_s = {}
  local six_u = {}
  for _, ring in ipairs(core.ringNetwork(idx_s, 6)) do
    if #ring == 6 then
      six_s[#six_s + 1] = ring
    end
  end
  for _, ring in ipairs(core.ringNetwork(idx_u, 6)) do
    if #ring == 6 then
      six_u[#six_u + 1] = ring
    end
  end
  return core.seededCageAffiliation(six_s, idx_s, six_u, idx_u)
end

return dseams
