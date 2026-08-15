-- Idiomatic table-first helpers on top of the C++ registrations.
-- require("yoda") after yodaStruct has called luaApi::registerAll.

local yoda = {}

local function suffix(path)
  return (path:match("%.([^%./]+)$") or ""):lower()
end

local function opts(t)
  return t or {}
end

function yoda.read(path, o)
  o = opts(o)
  local frame = o.frame or 1
  local ext = suffix(path)
  if ext == "xyz" then
    return readXYZ(path)
  end
  if ext == "con" then
    if readCon == nil then
      error("readCon is not in this build (readcon-core missing)")
    end
    return readCon(path, frame)
  end
  if ext == "pdb" or ext == "gro" or ext == "dcd" then
    if readChemfiles == nil then
      error("readChemfiles is not in this build (chemfiles missing)")
    end
    return readChemfiles(path, frame, o.type or -1)
  end
  local typ = o.type
  if typ == nil then
    local cloud = readLammpsTrjO(path, frame, 2)
    if cloud.nop > 0 then
      return cloud
    end
    return readLammpsTrjO(path, frame, 1)
  end
  return readLammpsTrjO(path, frame, typ)
end

function yoda.neighbors(cloud, o)
  o = opts(o)
  return neighListO(o.cutoff or 3.5, cloud, o.type or 1)
end

function yoda.knn(cloud, o)
  o = opts(o)
  local mutual = true
  if o.mutual == false then
    mutual = false
  end
  return kNearestNeighbourList(cloud, o.k or 4, o.cutoff or 5.0, o.type or 1,
                               mutual)
end

function yoda.chill_plus(cloud, o)
  o = opts(o)
  local nl = yoda.neighbors(cloud, o)
  getCorrelPlus(cloud, nl, false)
  return getIceTypePlusNoPrint(cloud, nl, false)
end

function yoda.chill(cloud, o)
  o = opts(o)
  local nl = yoda.neighbors(cloud, o)
  getCorrel(cloud, nl, false)
  return getIceTypeNoPrint(cloud, nl, false)
end

function yoda.cages(cloud, o)
  o = opts(o)
  local typ = o.type or 1
  local k = o.k or 4
  local cut = o.cutoff or 5.0
  local mutual = yoda.knn(cloud, {k = k, cutoff = cut, type = typ, mutual = true})
  local union = yoda.knn(cloud, {k = k, cutoff = cut, type = typ, mutual = false})
  local six_s = {}
  local six_u = {}
  for _, ring in ipairs(ringNetwork(neighbourListByIndex(cloud, mutual), 6)) do
    if #ring == 6 then
      six_s[#six_s + 1] = ring
    end
  end
  for _, ring in ipairs(ringNetwork(neighbourListByIndex(cloud, union), 6)) do
    if #ring == 6 then
      six_u[#six_u + 1] = ring
    end
  end
  return seededCageAffiliation(six_s, neighbourListByIndex(cloud, mutual), six_u,
                               neighbourListByIndex(cloud, union))
end

return yoda
