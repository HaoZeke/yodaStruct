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

local function require_nonempty(path, cloud, selection)
  if cloud.nop == 0 then
    local detail = selection and (" for " .. selection) or ""
    error(string.format("%s has no atoms%s", path, detail), 3)
  end
  return cloud
end

-- No region. A shrinking slice is core.readLammpsTrjreduced.
function dseams.read(path, o)
  o = opts(o)
  local frame = o.frame or 1
  local ext = suffix(path)
  if ext == "xyz" then
    return require_nonempty(path, core.readXYZ(path))
  end
  if ext == "con" then
    if core.readCon == nil then
      error("readCon is not in this build (readcon-core missing)")
    end
    return require_nonempty(path, core.readCon(path, frame))
  end
  if ext == "pdb" or ext == "gro" or ext == "dcd" then
    if core.readChemfiles == nil then
      error("readChemfiles is not in this build (chemfiles missing)")
    end
    return require_nonempty(path, core.readChemfiles(path, frame, o.type or -1))
  end
  if o.all == true then
    return require_nonempty(path, core.readLammpsTrj(path, frame), "all atoms")
  end
  local typ = o.type
  if typ == nil then
    local cloud = core.readLammpsTrjO(path, frame, 2)
    if cloud.nop > 0 then
      return cloud
    end
    return require_nonempty(path, core.readLammpsTrjO(path, frame, 1),
                            "types 2 or 1")
  end
  return require_nonempty(path, core.readLammpsTrjO(path, frame, typ),
                          "type " .. tostring(typ))
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

function dseams.rdf(cloud, o)
  o = opts(o)
  local rmax = o.cutoff or 12.0
  local bins = o.bins or math.max(1, math.floor(rmax / 0.05))
  return core.calcRDF3D(cloud, o.type_i or 1, o.type_j or 2, rmax, bins)
end

function dseams.running_cn(cloud, o)
  o = opts(o)
  local rmax = o.cutoff or 12.0
  local bins = o.bins or math.max(1, math.floor(rmax / 0.05))
  return core.calcRunningCN(cloud, o.type_i or 1, o.type_j or 2, rmax, bins)
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
  return core.seededCageAffiliation(six_s, idx_s, six_u, idx_u, o.complete or false)
end

-- Label-independent topology keys of a bonded graph given as rows by
-- index (core.neighbourListByIndex). o.hops (default 2), o.max_ring (7),
-- o.colours an optional list of one integer class per row (atom type,
-- species): vertices of different colours never match.
function dseams.fingerprint(rows, o)
  o = opts(o)
  if o.colours then
    return core.topologyFingerprint(rows, o.hops or 2, o.max_ring or 7, o.colours)
  end
  return core.topologyFingerprint(rows, o.hops or 2, o.max_ring or 7)
end

-- Key library text from a frame's rows under a label (o.hops, o.max_ring,
-- o.colours as in fingerprint; o.library an existing library text to extend).
function dseams.topology_library(rows, label, o)
  o = opts(o)
  return core.topologyLibrary(rows, label, o.hops or 2, o.max_ring or 7, o.colours, o.library)
end

-- Name every atom by a key library text: {labels=, counts=, matched=}.
function dseams.classify_topology(rows, library, o)
  o = opts(o)
  return core.classifyTopology(rows, library, o.hops or 2, o.max_ring or 7, o.colours)
end

-- Ions read against a per-atom ice flag list: o.type is the water type
-- (default 1), o.cutoff the first shell radius (default 3.5).
function dseams.ion_environment(cloud, ice, ions, o)
  o = opts(o)
  return core.ionEnvironment(cloud, ice, ions, o.type or 1, o.cutoff or 3.5)
end

function dseams.hbonds(cloud, o)
  o = opts(o)
  local nl = dseams.neighbors(cloud, o)
  if o.h_cloud ~= nil then
    return core.getHbondNetworkFromClouds(cloud, o.h_cloud, nl, o.dist,
                                          o.angle)
  end
  if o.path == nil then
    error("hbonds needs path= or h_cloud=", 2)
  end
  return core.getHbondNetwork(o.path, cloud, nl, o.frame or 1,
                              o.h_type or 1, o.dist, o.angle)
end

local axis_indices = {x = 0, y = 1, z = 2}

function dseams.density(cloud, o)
  o = opts(o)
  local axis = o.axis or "z"
  local axis_index = type(axis) == "number" and axis or axis_indices[axis]
  if axis_index == nil or axis_index < 0 or axis_index > 2 then
    error('density axis must be "x", "y", "z", 0, 1, or 2', 2)
  end
  local span = cloud:box()[axis_index + 1] or 0.0
  local bins = o.bins or math.max(1, math.floor(span / 0.1 + 0.5))
  if bins < 1 then
    error("density bins must be positive", 2)
  end
  if (o.table == nil) ~= (o.kind == nil) then
    error("density table and kind must be supplied together", 2)
  end
  if o.table ~= nil then
    return core.densityByKind(cloud, o.table, o.kind, bins, axis_index)
  end
  return core.densityByType(cloud, o.type or 0, bins, axis_index)
end

function dseams.site_table(spec)
  return core.parseSiteSpec(spec)
end

function dseams.pairs(cloud, o)
  o = opts(o)
  if o.table == nil then
    error("pairs needs table=", 2)
  end
  return core.contactPairs(cloud, o.table)
end

function dseams.domain(cloud, o)
  o = opts(o)
  if o.table == nil or o.kind == nil then
    error("domain needs table= and kind=", 2)
  end
  return core.domainStats(cloud, o.table, o.kind, o.cutoff or 3.5)
end

return dseams
