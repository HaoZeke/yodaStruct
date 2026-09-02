//-----------------------------------------------------------------------------------
// d-SEAMS - Deferred Structural Elucidation Analysis for Molecular Simulations
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------------

#include <lua_api.hpp>

#include <bond.hpp>
#include <bop.hpp>
#include <bulkTUM.hpp>
#include <cage_affiliation.hpp>
#include <cluster.hpp>
#include <density.hpp>
#include <franzblau.hpp>
#include <cage_enum.hpp>
#include <mol_sys.hpp>
#include <neighbours.hpp>
#include <rdf.hpp>
#include <rdf2d.hpp>
#include <ring.hpp>
#include <seams_input.hpp>
#include <seams_output.hpp>
#include <selection.hpp>
#include <site.hpp>
#include <structure_desc.hpp>
#include <topo_bulk.hpp>
#include <topo_fingerprint.hpp>
#include <topo_one_dim.hpp>
#include <topo_two_dim.hpp>
#include <voronoi_qlm.hpp>

#include <array>
#include <string>
#include <tuple>
#include <vector>

using luaApi::Cloud;

//! Human-readable name of a per-particle CHILL/CHILL+ state
const char *iceStateName(molSys::atom_state_type state) {
  switch (state) {
  case molSys::atom_state_type::cubic:
    return "cubic";
  case molSys::atom_state_type::hexagonal:
    return "hexagonal";
  case molSys::atom_state_type::water:
    return "water";
  case molSys::atom_state_type::interfacial:
    return "interfacial";
  case molSys::atom_state_type::clathrate:
    return "clathrate";
  case molSys::atom_state_type::interClathrate:
    return "interClathrate";
  case molSys::atom_state_type::reCubic:
    return "reCubic";
  case molSys::atom_state_type::reHex:
    return "reHex";
  case molSys::atom_state_type::unclassified:
    break;
  }
  return "unclassified";
}

//! Per-particle ice states of a cloud, as strings ready for a Lua table
std::vector<std::string> iceStateNames(const Cloud &yCloud) {
  std::vector<std::string> names;
  names.reserve(yCloud.pts.size());
  for (const auto &pt : yCloud.pts) {
    names.emplace_back(iceStateName(pt.iceType));
  }
  return names;
}

//! Human-readable name of a mapped site kind
const char *siteKindName(site::Kind kind) {
  switch (kind) {
  case site::Kind::unspecified:
    return "unspecified";
  case site::Kind::cationHead:
    return "cationHead";
  case site::Kind::anion:
    return "anion";
  case site::Kind::tail:
    return "tail";
  case site::Kind::donorH:
    return "donorH";
  case site::Kind::acceptor:
    return "acceptor";
  case site::Kind::polar:
    return "polar";
  case site::Kind::apolar:
    return "apolar";
  case site::Kind::waterO:
    return "waterO";
  case site::Kind::waterH:
    return "waterH";
  case site::Kind::solvent:
    return "solvent";
  }
  return "unspecified";
}

//! Cloud-index pairs as a Lua array of two-element arrays
sol::table packPairs(sol::state_view lua,
                     const std::vector<std::pair<int, int>> &pairs) {
  sol::table out = lua.create_table(static_cast<int>(pairs.size()), 0);
  for (std::size_t i = 0; i < pairs.size(); ++i) {
    sol::table row = lua.create_table(2, 0);
    row[1] = pairs[i].first;
    row[2] = pairs[i].second;
    out[i + 1] = row;
  }
  return out;
}

//! Packs a SteinhardtQl result into a {ql = ..., qlBar = ...} Lua table
sol::table packSteinhardt(sol::state_view lua, const chill::SteinhardtQl &q) {
  sol::table t = lua.create_table(0, 2);
  t["ql"] = sol::as_table(q.ql);
  t["qlBar"] = sol::as_table(q.qlBar);
  return t;
}

namespace luaApi {

constexpr std::array<double, 3> kZeroBounds{0.0, 0.0, 0.0};

Cloud readLammpsTrj(std::string filename, int targetFrame) {
  Cloud scratch;
  return sinp::readLammpsTrj(filename, targetFrame, scratch);
}

Cloud readLammpsTrjO(std::string filename, int targetFrame, int typeO,
                     sol::optional<bool> isSlice,
                     sol::optional<std::array<double, 3>> low,
                     sol::optional<std::array<double, 3>> high) {
  Cloud scratch;
  return sinp::readLammpsTrjO(
      filename, targetFrame, scratch, typeO, isSlice.value_or(false),
      low.value_or(kZeroBounds), high.value_or(kZeroBounds));
}

Cloud readLammpsTrjreduced(std::string filename, int targetFrame, int typeI,
                           sol::optional<bool> isSlice,
                           sol::optional<std::array<double, 3>> low,
                           sol::optional<std::array<double, 3>> high) {
  Cloud scratch;
  return sinp::readLammpsTrjreduced(
      filename, targetFrame, scratch, typeI, isSlice.value_or(false),
      low.value_or(kZeroBounds), high.value_or(kZeroBounds));
}

Cloud readXYZ(const std::string &filename) { return sinp::readXYZ(filename); }

#ifdef SEAMS_HAS_CHEMFILES
Cloud readChemfiles(std::string filename, int targetFrame,
                    sol::optional<int> typeFilter) {
  Cloud scratch;
  return sinp::readChemfiles(filename, targetFrame, scratch,
                             typeFilter.value_or(-1));
}
#endif

#ifdef SEAMS_HAS_READCON
Cloud readCon(std::string filename, int targetFrame) {
  Cloud scratch;
  return sinp::readCon(filename, targetFrame, scratch);
}
#endif

int writeDump(const Cloud &yCloud, std::string path, std::string outFile) {
  return sout::writeDump(yCloud, path, outFile);
}

int writeHistogram(const Cloud &yCloud,
                   const std::vector<std::vector<int>> &nList,
                   const std::vector<double> &avgQ6) {
  return sout::writeHisto(yCloud, nList, avgQ6);
}

std::vector<std::vector<int>> neighListO(double rcutoff, const Cloud &yCloud,
                                         int typeI) {
  return nneigh::neighListO(rcutoff, yCloud, typeI);
}

std::vector<std::vector<int>> getNewNeighbourListByIndex(const Cloud &yCloud,
                                                         double cutoff) {
  return nneigh::getNewNeighbourListByIndex(yCloud, cutoff);
}

std::vector<std::vector<int>>
neighbourListByIndex(const Cloud &yCloud, std::vector<std::vector<int>> nList) {
  return nneigh::neighbourListByIndex(yCloud, nList);
}

std::vector<std::vector<int>>
kNearestNeighbourList(const Cloud &yCloud, int k, double candidateCutoff,
                      int typeI, sol::optional<bool> mutual) {
  return nneigh::kNearestNeighbourList(yCloud, k, candidateCutoff, typeI,
                                       mutual.value_or(true));
}

std::tuple<double, double> shellSeparation(const Cloud &yCloud, int k,
                                           int typeI) {
  return nneigh::shellSeparation(yCloud, k, typeI);
}

std::vector<std::vector<int>>
getHbondNetwork(std::string filename, Cloud &yCloud,
                std::vector<std::vector<int>> nList, int targetFrame, int Htype,
                sol::optional<double> dist, sol::optional<double> angle) {
  return bond::populateHbonds(filename, yCloud, nList, targetFrame, Htype,
                              dist.value_or(2.42), angle.value_or(30.0));
}

std::vector<std::vector<int>> getHbondNetworkFromClouds(
    Cloud &yCloud, Cloud &hCloud, std::vector<std::vector<int>> nList,
    sol::optional<double> dist, sol::optional<double> angle) {
  return bond::populateHbondsWithInputClouds(
      yCloud, hCloud, nList, dist.value_or(2.42), angle.value_or(30.0));
}

std::vector<std::vector<int>> neighListPair(double rcutoff, const Cloud &yCloud,
                                            int typeI, int typeJ) {
  return nneigh::neighListPair(rcutoff, yCloud, typeI, typeJ);
}

std::vector<std::vector<int>>
getHbondNetworkFromDonors(Cloud &yCloud, Cloud &hCloud,
                          std::vector<std::vector<int>> nList,
                          std::vector<int> donorHs, sol::optional<double> dist,
                          sol::optional<double> angle) {
  return bond::populateHbondsFromDonors(yCloud, hCloud, nList, donorHs,
                                        dist.value_or(2.42),
                                        angle.value_or(30.0));
}

std::vector<std::vector<int>> ringNetwork(std::vector<std::vector<int>> nList,
                                          int maxDepth) {
  return primitive::ringNetwork(nList, maxDepth);
}

sol::table cageAffiliation(sol::this_state ts,
                           std::vector<std::vector<int>> rings,
                           std::vector<std::vector<int>> nList) {
  sol::state_view lua(ts);
  const auto a = ring::cageAffiliation(rings, nList);
  sol::table out = lua.create_table(0, 2);
  out["hc"] = sol::as_table(a.hc);
  out["ddc"] = sol::as_table(a.ddc);
  return out;
}

sol::table findBySignature(sol::this_state ts,
                           std::vector<std::vector<int>> rings,
                           std::vector<std::vector<int>> nList,
                           std::string spec) {
  sol::state_view lua(ts);
  const auto sig = cage::Signature::parse(spec);
  const auto found = cage::findBySignature(rings, nList, sig);
  sol::table out = lua.create_table(static_cast<int>(found.size()), 0);
  for (size_t i = 0; i < found.size(); ++i) {
    sol::table row = lua.create_table(0, 4);
    row["signature"] = found[i].signature.str();
    row["faces"] = sol::as_table(found[i].faces);
    row["vertices"] = sol::as_table(found[i].vertices);
    row["certificate"] = found[i].certificate;
    out[i + 1] = row;
  }
  return out;
}

sol::table seededCageAffiliation(sol::this_state ts,
                                 std::vector<std::vector<int>> strictRings,
                                 std::vector<std::vector<int>> strictNList,
                                 std::vector<std::vector<int>> permRings,
                                 std::vector<std::vector<int>> permNList,
                                 sol::optional<bool> ringAdjacentCompletion) {
  sol::state_view lua(ts);
  const auto a = ring::seededCageAffiliation(
      strictRings, strictNList, permRings, permNList,
      ringAdjacentCompletion.value_or(false));
  sol::table out = lua.create_table(0, 2);
  out["hc"] = sol::as_table(a.hc);
  out["ddc"] = sol::as_table(a.ddc);
  return out;
}

sol::table topologyFingerprint(sol::this_state ts, std::vector<std::vector<int>> rows,
                               sol::optional<int> hops, sol::optional<int> maxRingSize,
                               sol::optional<std::vector<int>> colours) {
  sol::state_view lua(ts);
  const auto fp = topo::fingerprint(rows, hops.value_or(2), maxRingSize.value_or(7),
                                    colours.value_or(std::vector<int>{}));
  sol::table out = lua.create_table(0, 6);
  out["key"] = fp.key;
  out["method"] = fp.method;
  out["hops"] = fp.hops;
  out["atomKeys"] = sol::as_table(fp.atomKeys);
  sol::table classes = lua.create_table();
  for (const auto &kv : fp.classes) {
    classes[kv.first] = kv.second;
  }
  out["classes"] = classes;
  out["ringCensus"] = sol::as_table(fp.ringCensus);
  return out;
}

sol::table localTopologyKey(sol::this_state ts, std::vector<std::vector<int>> rows, int atom,
                            sol::optional<int> hops, sol::optional<std::vector<int>> colours) {
  sol::state_view lua(ts);
  const auto lk = topo::localKey(rows, atom, hops.value_or(2),
                                 colours.value_or(std::vector<int>{}));
  sol::table out = lua.create_table(0, 4);
  out["key"] = lk.key;
  out["method"] = lk.method;
  out["vertices"] = lk.vertices;
  out["edges"] = lk.edges;
  return out;
}

// Key libraries travel as their text form so Lua holds a string.
std::string topologyLibrary(std::vector<std::vector<int>> rows, std::string label,
                            sol::optional<int> hops, sol::optional<int> maxRingSize,
                            sol::optional<std::vector<int>> colours,
                            sol::optional<std::string> existing) {
  topo::KeyLibrary lib;
  if (existing && !existing->empty()) {
    lib = topo::readLibrary(*existing);
  }
  const auto fp = topo::fingerprint(rows, hops.value_or(2), maxRingSize.value_or(7),
                                    colours.value_or(std::vector<int>{}));
  topo::addToLibrary(lib, fp, label);
  return topo::writeLibrary(lib);
}

// `library` is one library text, or a Lua sequence of library texts built
// at different hop counts; then the deepest library that holds an atom's
// key names it and `hops` is ignored.
sol::table classifyTopology(sol::this_state ts, std::vector<std::vector<int>> rows,
                            sol::object library, sol::optional<int> hops,
                            sol::optional<int> maxRingSize,
                            sol::optional<std::vector<int>> colours) {
  sol::state_view lua(ts);
  const auto cols = colours.value_or(std::vector<int>{});
  topo::LibraryMatch match;
  if (library.is<std::string>()) {
    const auto fp = topo::fingerprint(rows, hops.value_or(2), maxRingSize.value_or(7), cols);
    match = topo::matchLibrary(fp, topo::readLibrary(library.as<std::string>()));
  } else if (library.is<sol::table>()) {
    std::vector<topo::KeyLibrary> libs;
    for (const auto &kv : library.as<sol::table>()) {
      libs.push_back(topo::readLibrary(kv.second.as<std::string>()));
    }
    match = topo::matchLibraries(rows, libs, maxRingSize.value_or(7), cols);
  } else {
    throw std::invalid_argument("classifyTopology wants a library text or a table of them");
  }
  sol::table out = lua.create_table(0, 4);
  out["labels"] = sol::as_table(match.labels);
  sol::table counts = lua.create_table();
  for (const auto &kv : match.counts) {
    counts[kv.first.empty() ? std::string("unmatched") : kv.first] = kv.second;
  }
  out["counts"] = counts;
  out["depth"] = sol::as_table(match.depth);
  out["matched"] = match.matched;
  return out;
}

// Guests placed in enumerated cages (vertex index lists) by the periodic
// centroid of each cage.
sol::table guestOccupancy(sol::this_state ts, const Cloud &cloud,
                          std::vector<std::vector<int>> cages, std::vector<int> guests,
                          double radius) {
  sol::state_view lua(ts);
  const auto occ = site::guestOccupancy(cloud, cages, guests, radius);
  sol::table out = lua.create_table(0, 6);
  out["guestsPerCage"] = sol::as_table(occ.guestsPerCage);
  out["cageOfGuest"] = sol::as_table(occ.cageOfGuest);
  out["centreDistance"] = sol::as_table(occ.centreDistance);
  out["occupied"] = occ.occupied;
  out["multiply"] = occ.multiply;
  out["free"] = occ.free;
  return out;
}

std::vector<int> shellRingCensus(std::vector<std::vector<int>> rings, std::vector<int> shell,
                                 sol::optional<int> maxRingSize) {
  return site::shellRingCensus(rings, shell, maxRingSize.value_or(7));
}

std::vector<double> periodicCentroid(const Cloud &cloud, std::vector<int> atoms) {
  const auto c = site::periodicCentroid(cloud, atoms);
  return {c[0], c[1], c[2]};
}

sol::table ionEnvironment(sol::this_state ts, const Cloud &cloud, std::vector<bool> iceFlag,
                          std::vector<int> ionIndices, sol::optional<int> waterType,
                          sol::optional<double> cutoff) {
  sol::state_view lua(ts);
  const auto env = site::ionEnvironment(cloud, iceFlag, ionIndices, waterType.value_or(1),
                                        cutoff.value_or(3.5));
  sol::table out = lua.create_table(0, 7);
  out["ion"] = sol::as_table(env.ion);
  out["shell"] = sol::as_table(env.shell);
  out["iceFraction"] = sol::as_table(env.iceFraction);
  std::vector<std::string> states;
  states.reserve(env.state.size());
  for (const auto st : env.state) {
    states.push_back(st == site::IonState::ice ? "ice"
                     : st == site::IonState::front ? "front"
                                                   : "liquid");
  }
  out["state"] = sol::as_table(states);
  sol::table members = lua.create_table();
  for (std::size_t i = 0; i < env.members.size(); i++) {
    members[i + 1] = sol::as_table(env.members[i]);
  }
  out["members"] = members;
  out["nIce"] = env.nIce;
  out["nFront"] = env.nFront;
  out["nLiquid"] = env.nLiquid;
  return out;
}

namespace {

chill::BondClassifier ruleFromTable(const sol::table &t) {
  chill::BondClassifier rule = chill::chillRule();
  rule.staggeredMax = t.get_or("staggeredMax", rule.staggeredMax);
  rule.eclipsedMin = t.get_or("eclipsedMin", rule.eclipsedMin);
  rule.eclipsedMax = t.get_or("eclipsedMax", rule.eclipsedMax);
  rule.coordinationNumber =
      t.get_or("coordinationNumber", rule.coordinationNumber);
  return rule;
}

} // namespace

void classifyBonds(Cloud &yCloud, std::vector<std::vector<int>> nList,
                   const sol::object &ruleSpec, sol::optional<bool> isSlice) {
  const chill::BondClassifier rule =
      ruleSpec.is<std::string>()
          ? chill::bondClassifier(ruleSpec.as<std::string>())
          : ruleFromTable(ruleSpec.as<sol::table>());
  chill::classifyBonds(yCloud, nList, rule, isSlice.value_or(false));
}

void registerBondClassifier(std::string name, const sol::table &t) {
  chill::registerBondClassifier(name, ruleFromTable(t));
}

std::vector<std::string> bondClassifierNames() {
  return chill::bondClassifierNames();
}

void getCorrelPlus(Cloud &yCloud, std::vector<std::vector<int>> nList,
                   sol::optional<bool> isSlice,
                   sol::optional<int> coordinationNumber) {
  chill::getCorrelPlus(yCloud, nList, isSlice.value_or(false),
                       coordinationNumber.value_or(4));
}

std::vector<std::string>
getIceTypePlus(Cloud &yCloud, std::vector<std::vector<int>> nList,
               std::string path, int firstFrame, sol::optional<bool> isSlice,
               sol::optional<std::string> outputFileName) {
  chill::getIceTypePlus(yCloud, nList, path, firstFrame,
                        isSlice.value_or(false),
                        outputFileName.value_or("chillPlus.txt"));
  return iceStateNames(yCloud);
}

std::vector<std::string>
getIceTypePlusNoPrint(Cloud &yCloud, std::vector<std::vector<int>> nList,
                      sol::optional<bool> isSlice) {
  chill::getIceTypePlusNoPrint(yCloud, nList, isSlice.value_or(false));
  return iceStateNames(yCloud);
}

void getCorrel(Cloud &yCloud, std::vector<std::vector<int>> nList,
               sol::optional<bool> isSlice,
               sol::optional<int> coordinationNumber) {
  chill::getCorrel(yCloud, nList, isSlice.value_or(false),
                   coordinationNumber.value_or(4));
}

std::vector<std::string> getIceType(Cloud &yCloud,
                                    std::vector<std::vector<int>> nList,
                                    std::string path, int firstFrame,
                                    sol::optional<bool> isSlice,
                                    sol::optional<std::string> outputFileName) {
  chill::getIceType(yCloud, nList, path, firstFrame, isSlice.value_or(false),
                    outputFileName.value_or("chill.txt"));
  return iceStateNames(yCloud);
}

std::vector<std::string> getIceTypeNoPrint(Cloud &yCloud,
                                           std::vector<std::vector<int>> nList,
                                           sol::optional<bool> isSlice) {
  chill::getIceTypeNoPrint(yCloud, nList, isSlice.value_or(false));
  return iceStateNames(yCloud);
}

sol::table steinhardtQl(sol::this_state ts, const Cloud &yCloud,
                        std::vector<std::vector<int>> nList, int orderL) {
  return packSteinhardt(sol::state_view(ts),
                        chill::steinhardtQl(yCloud, nList, orderL));
}

sol::table steinhardtQlVoronoi(sol::this_state ts, const Cloud &yCloud,
                               double candidateCutoff, int orderL) {
  return packSteinhardt(
      sol::state_view(ts),
      chill::steinhardtQlVoronoi(yCloud, candidateCutoff, orderL));
}

sol::table voronoiFacetWeights(sol::this_state ts, const Cloud &yCloud,
                               double candidateCutoff) {
  sol::state_view lua(ts);
  const auto cells = chill::voronoiFacetWeights(yCloud, candidateCutoff);
  sol::table out = lua.create_table(static_cast<int>(cells.size()), 0);
  for (size_t i = 0; i < cells.size(); i++) {
    sol::table cell = lua.create_table(0, 3);
    cell["neighbours"] = sol::as_table(cells[i].neighbours);
    cell["weights"] = sol::as_table(cells[i].weights);
    cell["certified"] = cells[i].certified;
    out[i + 1] = cell;
  }
  return out;
}

sol::table classifyTemplates(sol::this_state ts, const Cloud &cloud,
                             std::vector<std::vector<int>> nList, int kNeigh) {
  sol::state_view lua(ts);
  const auto hits = chill::classifyTemplates(cloud, nList, kNeigh);
  sol::table out = lua.create_table(static_cast<int>(hits.size()), 0);
  for (size_t i = 0; i < hits.size(); i++) {
    sol::table row = lua.create_table(0, 2);
    row["name"] = hits[i].name;
    row["rmsd"] = hits[i].rmsd;
    out[i + 1] = row;
  }
  return out;
}

std::vector<double> soapSpectrum(const Cloud &yCloud, int iatom,
                                 std::vector<std::vector<int>> nList, int nMax,
                                 int lMax, double rcut) {
  return chill::soapSpectrum(yCloud, iatom, nList, nMax, lMax, rcut);
}

std::vector<std::vector<double>>
soapSpectrumAll(const Cloud &yCloud, std::vector<std::vector<int>> nList,
                int nMax, int lMax, double rcut) {
  return chill::soapSpectrumAll(yCloud, nList, nMax, lMax, rcut);
}

std::vector<std::vector<double>> voronoiFeatures(const Cloud &yCloud,
                                                 double candidateCutoff) {
  return chill::voronoiFeatures(yCloud, candidateCutoff);
}

void registerIO(sol::state_view lua, sol::table m) {
  lua.new_usertype<Cloud>(
      "PointCloud", sol::constructors<Cloud()>(), "nop",
      sol::readonly(&Cloud::nop), "currentFrame",
      sol::readonly(&Cloud::currentFrame), "box",
      [](const Cloud &c) { return sol::as_table(c.box); }, "boxLow",
      [](const Cloud &c) { return sol::as_table(c.boxLow); }, "iceTypes",
      [](const Cloud &c) { return sol::as_table(iceStateNames(c)); });
  m.set_function("readLammpsTrj", readLammpsTrj);
  m.set_function("readLammpsTrjO", readLammpsTrjO);
  m.set_function("readLammpsTrjreduced", readLammpsTrjreduced);
  m.set_function("readXYZ", readXYZ);
#ifdef SEAMS_HAS_CHEMFILES
  m.set_function("readChemfiles", readChemfiles);
#endif
#ifdef SEAMS_HAS_READCON
  m.set_function("readCon", readCon);
#endif
  m.set_function("readFrameOnlyOne", sinp::readLammpsTrjreduced);
  m.set_function("readFrameOnlyOneAllAtoms", sinp::readLammpsTrj);
  m.set_function("readFrame", sinp::readLammpsTrjO);
  m.set_function("writeDump", writeDump);
  m.set_function("writeHistogram", writeHistogram);
}

void registerNeighbours(sol::state_view lua, sol::table m) {
  m.set_function("neighListO",
                 [](double rcutoff, const Cloud &yCloud, int typeI) {
                   return sol::as_nested(neighListO(rcutoff, yCloud, typeI));
                 });
  m.set_function(
      "getNewNeighbourListByIndex", [](const Cloud &yCloud, double cutoff) {
        return sol::as_nested(getNewNeighbourListByIndex(yCloud, cutoff));
      });
  m.set_function("neighbourListByIndex",
                 [](const Cloud &yCloud, std::vector<std::vector<int>> nList) {
                   return sol::as_nested(neighbourListByIndex(yCloud, nList));
                 });
  m.set_function("kNearestNeighbourList", [](const Cloud &yCloud, int k,
                                             double candidateCutoff, int typeI,
                                             sol::optional<bool> mutual) {
    return sol::as_nested(
        kNearestNeighbourList(yCloud, k, candidateCutoff, typeI, mutual));
  });
  m.set_function("shellSeparation", shellSeparation);
  m.set_function(
      "mutualNearestUnlike",
      [](sol::this_state ts, const Cloud &yCloud, int typeI, int typeJ) {
        return packPairs(sol::state_view(ts),
                         nneigh::mutualNearestUnlike(yCloud, typeI, typeJ));
      });
  // Legacy spellings. A Lua table (what every as_nested binding returns)
  // and a container userdata both bind to a by-value vector; a reference
  // parameter accepts only the userdata and reads a table as garbage
  m.set_function("neighborList", nneigh::neighListO);
  m.set_function("bondNetworkByIndex",
                 [](const Cloud &yCloud, std::vector<std::vector<int>> nList) {
                   return sol::as_nested(neighbourListByIndex(yCloud, nList));
                 });
  m.set_function("getHbondNetwork", [](std::string filename, Cloud &yCloud,
                                       std::vector<std::vector<int>> nList,
                                       int targetFrame, int Htype,
                                       sol::optional<double> dist,
                                       sol::optional<double> angle) {
    return sol::as_nested(getHbondNetwork(filename, yCloud, nList, targetFrame,
                                          Htype, dist, angle));
  });
  m.set_function(
      "getHbondNetworkFromClouds",
      [](Cloud &yCloud, Cloud &hCloud, std::vector<std::vector<int>> nList,
         sol::optional<double> dist, sol::optional<double> angle) {
        return sol::as_nested(
            getHbondNetworkFromClouds(yCloud, hCloud, nList, dist, angle));
      });
  m.set_function("neighListPair", [](double rcutoff, const Cloud &yCloud,
                                     int typeI, int typeJ) {
    return sol::as_nested(neighListPair(rcutoff, yCloud, typeI, typeJ));
  });
  m.set_function(
      "getHbondNetworkFromDonors",
      [](Cloud &yCloud, Cloud &hCloud, std::vector<std::vector<int>> nList,
         std::vector<int> donorHs, sol::optional<double> dist,
         sol::optional<double> angle) {
        return sol::as_nested(getHbondNetworkFromDonors(yCloud, hCloud, nList,
                                                        donorHs, dist, angle));
      });
  m.set_function("donatedHydrogenBond", [](const Cloud &yCloud,
                                           const Cloud &hCloud, int acceptor,
                                           int donor, std::vector<int> donorHs,
                                           sol::optional<double> dist,
                                           sol::optional<double> angle) {
    return bond::donatedHydrogenBond(yCloud, hCloud, acceptor, donor, donorHs,
                                     dist.value_or(2.42), angle.value_or(30.0));
  });
}

void registerRings(sol::state_view lua, sol::table m) {
  m.set_function("ringNetwork",
                 [](std::vector<std::vector<int>> nList, int maxDepth) {
                   return sol::as_nested(ringNetwork(nList, maxDepth));
                 });
  lua.new_usertype<primitive::RingUpdater>(
      "RingUpdater", sol::constructors<primitive::RingUpdater(int)>(), "update",
      [](primitive::RingUpdater &self, std::vector<std::vector<int>> nList) {
        return sol::as_nested(self.update(nList));
      },
      "lastRecomputedSources", &primitive::RingUpdater::lastRecomputedSources,
      "lastBallsRefreshed", &primitive::RingUpdater::lastBallsRefreshed);
  // Legacy spelling; by-value so a table or a container userdata both bind
  m.set_function("getPrimitiveRings", ringNetwork);
  // Order-free per-ring cage classification and its exact incremental form
  m.set_function("cageAffiliation", cageAffiliation);
  m.set_function("findBySignature", findBySignature);
  lua.new_usertype<ring::AffiliationUpdater>(
      "AffiliationUpdater", sol::constructors<ring::AffiliationUpdater()>(),
      "update",
      [](sol::this_state ts, ring::AffiliationUpdater &self,
         std::vector<std::vector<int>> rings,
         std::vector<std::vector<int>> nList) {
        sol::state_view lua(ts);
        const auto &a = self.update(rings, nList);
        sol::table out = lua.create_table(0, 2);
        out["hc"] = sol::as_table(a.hc);
        out["ddc"] = sol::as_table(a.ddc);
        return out;
      },
      "lastReclassified", &ring::AffiliationUpdater::lastReclassified);
  // Seeded (hysteresis) affiliation: strict-graph seeds, permissive-graph
  // completion, component-gated acceptance
  m.set_function("seededCageAffiliation", seededCageAffiliation);
  m.set_function("topologyFingerprint", topologyFingerprint);
  m.set_function("localTopologyKey", localTopologyKey);
  m.set_function("ionEnvironment", ionEnvironment);
  m.set_function("topologyLibrary", topologyLibrary);
  m.set_function("classifyTopology", classifyTopology);
  m.set_function("guestOccupancy", guestOccupancy);
  m.set_function("periodicCentroid", periodicCentroid);
  m.set_function("shellRingCensus", shellRingCensus);
}

void registerOrder(sol::state_view lua, sol::table m) {
  m.set_function("classifyBonds", classifyBonds);
  m.set_function("registerBondClassifier", registerBondClassifier);
  m.set_function("bondClassifierNames",
                 []() { return sol::as_table(bondClassifierNames()); });
  m.set_function("getCorrelPlus", getCorrelPlus);
  m.set_function("getIceTypePlus", [](Cloud &yCloud,
                                      std::vector<std::vector<int>> nList,
                                      std::string path, int firstFrame,
                                      sol::optional<bool> isSlice,
                                      sol::optional<std::string> outName) {
    return sol::as_table(
        getIceTypePlus(yCloud, nList, path, firstFrame, isSlice, outName));
  });
  m.set_function("getIceTypePlusNoPrint",
                 [](Cloud &yCloud, std::vector<std::vector<int>> nList,
                    sol::optional<bool> isSlice) {
                   return sol::as_table(
                       getIceTypePlusNoPrint(yCloud, nList, isSlice));
                 });
  m.set_function("getIceTypeNoPrint", [](Cloud &yCloud,
                                         std::vector<std::vector<int>> nList,
                                         sol::optional<bool> isSlice) {
    return sol::as_table(getIceTypeNoPrint(yCloud, nList, isSlice));
  });
  m.set_function("getCorrel", getCorrel);
  m.set_function("getIceType", [](Cloud &yCloud,
                                  std::vector<std::vector<int>> nList,
                                  std::string path, int firstFrame,
                                  sol::optional<bool> isSlice,
                                  sol::optional<std::string> outName) {
    return sol::as_table(
        getIceType(yCloud, nList, path, firstFrame, isSlice, outName));
  });
  m.set_function("steinhardtQl", steinhardtQl);
  m.set_function("steinhardtQlVoronoi", steinhardtQlVoronoi);
  m.set_function("voronoiFacetWeights", voronoiFacetWeights);
  // Legacy spellings kept for the bulk example scripts
  m.set_function("chillPlus_cij",
                 [](Cloud &c, const std::vector<std::vector<int>> &n,
                    bool slice) -> Cloud & {
                   chill::getCorrelPlus(c, n, slice);
                   return c;
                 });
  m.set_function("chillPlus_iceType",
                 [](Cloud &c, const std::vector<std::vector<int>> &n,
                    std::string path, int first, bool slice,
                    std::string outName) -> Cloud & {
                   chill::getIceTypePlus(c, n, path, first, slice, outName);
                   return c;
                 });
  m.set_function("chill_cij",
                 [](Cloud &c, const std::vector<std::vector<int>> &n,
                    bool slice) -> Cloud & {
                   chill::getCorrel(c, n, slice);
                   return c;
                 });
  m.set_function("chill_iceType",
                 [](Cloud &c, const std::vector<std::vector<int>> &n,
                    std::string path, int first, bool slice,
                    std::string outName) -> Cloud & {
                   chill::getIceType(c, n, path, first, slice, outName);
                   return c;
                 });
  m.set_function("averageQ6", chill::getq6);
  m.set_function("modifyChill",
                 [](Cloud &c, std::vector<double> &q6) -> Cloud & {
                   chill::reclassifyWater(c, q6);
                   return c;
                 });
  m.set_function("percentage_Ice", chill::printIceType);
}

void registerDescriptors(sol::state_view lua, sol::table m) {
  m.set_function("classifyTemplates", classifyTemplates);
  m.set_function("soapSpectrum", [](const Cloud &yCloud, int iatom,
                                    std::vector<std::vector<int>> nList,
                                    int nMax, int lMax, double rcut) {
    return sol::as_table(soapSpectrum(yCloud, iatom, nList, nMax, lMax, rcut));
  });
  m.set_function("soapSpectrumAll", [](const Cloud &yCloud,
                                       std::vector<std::vector<int>> nList,
                                       int nMax, int lMax, double rcut) {
    return sol::as_nested(soapSpectrumAll(yCloud, nList, nMax, lMax, rcut));
  });
  m.set_function(
      "voronoiFeatures", [](const Cloud &yCloud, double candidateCutoff) {
        return sol::as_nested(voronoiFeatures(yCloud, candidateCutoff));
      });
}

int ringAnalysis(std::string path, std::vector<std::vector<int>> rings,
                 std::vector<std::vector<int>> nList, Cloud &yCloud,
                 int maxDepth, double sheetArea, int firstFrame) {
  return ring::polygonRingAnalysis(std::move(path), std::move(rings),
                                   std::move(nList), yCloud, maxDepth,
                                   sheetArea, firstFrame);
}

int calcRDF(std::string path, std::vector<double> &rdfValues,
            const Cloud &yCloud, double cutoff, double binwidth, int firstFrame,
            int finalFrame) {
  return rdf2::rdf2Danalysis_AA(std::move(path), rdfValues, yCloud, cutoff,
                                binwidth, firstFrame, finalFrame);
}

sol::table calcRDF3D(sol::this_state ts, const Cloud &yCloud, int typeI,
                     int typeJ, double rmax, int nbins) {
  sol::state_view lua(ts);
  const auto gr = rdf::partialRdf(yCloud, typeI, typeJ, rmax, nbins);
  sol::table t = lua.create_table(0, 2);
  t["r"] = sol::as_table(gr.r);
  t["g"] = sol::as_table(gr.g);
  return t;
}

sol::table calcRunningCN(sol::this_state ts, const Cloud &yCloud, int typeI,
                         int typeJ, double rmax, int nbins) {
  sol::state_view lua(ts);
  const auto h = rdf::partialRdf(yCloud, typeI, typeJ, rmax, nbins);
  const double rhoJ =
      (h.volume > 0.0) ? static_cast<double>(h.nJ) / h.volume : 0.0;
  const auto cn = rdf::runningCN(h, rhoJ);
  sol::table t = lua.create_table(0, 2);
  t["r"] = sol::as_table(h.r);
  t["cn"] = sol::as_table(cn);
  return t;
}

double calcCN(const Cloud &yCloud, int typeI, int typeJ, double rmax, int nbins,
              sol::optional<double> rCut) {
  const auto h = rdf::partialRdf(yCloud, typeI, typeJ, rmax, nbins);
  const double rhoJ =
      (h.volume > 0.0) ? static_cast<double>(h.nJ) / h.volume : 0.0;
  return rdf::coordinationNumber(h, rCut.value_or(rmax), rhoJ);
}

sol::table packDensity(sol::state_view lua, const site::DensityZ &profile,
                       int axis, sol::optional<site::Kind> kind) {
  sol::table out = lua.create_table(0, 5);
  out["centres"] = sol::as_table(profile.z);
  out["rho"] = sol::as_table(profile.rho);
  out["axis"] = axis == 0 ? "x" : (axis == 1 ? "y" : "z");
  if (kind) {
    out["site_kind"] = siteKindName(*kind);
  } else {
    out["atom_type"] = profile.type;
  }
  return out;
}

sol::table densityByType(sol::this_state ts, const Cloud &yCloud, int typeI,
                         int nbin, int axis) {
  return packDensity(sol::state_view(ts),
                     site::densityZ(yCloud, typeI, nbin, axis), axis,
                     sol::nullopt);
}

sol::table densityByKind(sol::this_state ts, const Cloud &yCloud,
                         const site::Table &table, site::Kind kind, int nbin,
                         int axis) {
  return packDensity(sol::state_view(ts),
                     site::densityZ(yCloud, table, kind, nbin, axis), axis,
                     kind);
}

sol::table contactPairs(sol::this_state ts, const Cloud &cloud,
                        const site::Table &table) {
  sol::state_view lua(ts);
  const auto ions = site::ionCloud(cloud, table);
  const auto pairs = nneigh::mutualNearestUnlike(ions, 1, 2);
  int nCation = 0;
  int nAnion = 0;
  for (const auto &point : ions.pts) {
    nCation += point.type == 1 ? 1 : 0;
    nAnion += point.type == 2 ? 1 : 0;
  }
  sol::table out = lua.create_table(0, 4);
  out["pairs"] = packPairs(lua, pairs);
  out["count"] = static_cast<int>(pairs.size());
  out["n_cation"] = nCation;
  out["n_anion"] = nAnion;
  return out;
}

sol::table domainStats(sol::this_state ts, const Cloud &cloud,
                       const site::Table &table, site::Kind kind,
                       double cutoff) {
  std::vector<bool> mask(static_cast<std::size_t>(cloud.nop), false);
  for (const int index : site::indicesOf(cloud, table, kind)) {
    if (index >= 0 && index < cloud.nop) {
      mask[static_cast<std::size_t>(index)] = true;
    }
  }
  const auto byIndex = nneigh::getNewNeighbourListByIndex(cloud, cutoff);
  std::vector<std::vector<int>> byId(byIndex.size());
  for (std::size_t i = 0; i < byIndex.size(); ++i) {
    byId[i].reserve(byIndex[i].size());
    for (const int index : byIndex[i]) {
      if (index >= 0 && index < cloud.nop) {
        byId[i].push_back(cloud.pts[static_cast<std::size_t>(index)].atomID);
      }
    }
  }
  const auto domain = clump::largestDomain(cloud, byId, mask);
  sol::table out = sol::state_view(ts).create_table(0, 4);
  out["site_kind"] = siteKindName(kind);
  out["n"] = domain.subset;
  out["largest"] = domain.largest;
  out["percolation"] = domain.percolation;
  return out;
}

int prismAnalysis(std::string path, std::vector<std::vector<int>> rings,
                  std::vector<std::vector<int>> nList, Cloud &cloud,
                  int maxDepth, int atomID, int firstFrame, int currentFrame,
                  bool doShapeMatching) {
  return ring::prismAnalysis(std::move(path), rings, nList, cloud, maxDepth,
                             atomID, firstFrame, currentFrame, doShapeMatching);
}

int clusterAnalysis(std::string path, Cloud &iceCloud, Cloud &yCloud,
                    const std::vector<std::vector<int>> &nList,
                    std::vector<std::vector<int>> &iceNeighbourList,
                    double cutoff, int firstFrame, std::string bopAnalysis) {
  return clump::clusterAnalysis(std::move(path), iceCloud, yCloud, nList,
                                iceNeighbourList, cutoff, firstFrame,
                                std::move(bopAnalysis));
}

int recenterCluster(Cloud &iceCloud,
                    const std::vector<std::vector<int>> &nList) {
  return clump::recenterClusterCloud(iceCloud, nList);
}

Cloud getPointCloudAtomsOfOneAtomType(Cloud &yCloud, Cloud &outCloud,
                                      int atomTypeI, bool isSlice,
                                      std::array<double, 3> coordLow,
                                      std::array<double, 3> coordHigh) {
  return gen::getPointCloudOneAtomType(yCloud, outCloud, atomTypeI, isSlice,
                                       coordLow, coordHigh);
}

void selectInSingleSlice(Cloud &yCloud, bool clearPreviousSliceSelection,
                         std::array<double, 3> coordLow,
                         std::array<double, 3> coordHigh) {
  gen::moleculesInSingleSlice(yCloud, clearPreviousSliceSelection, coordLow,
                              coordHigh);
}

void selectEdgeAtomsInRingsWithinSlice(
    const std::vector<std::vector<int>> &rings, Cloud &oCloud, Cloud &yCloud,
    std::array<double, 3> coordLow, std::array<double, 3> coordHigh,
    bool identicalCloud) {
  ring::getEdgeMoleculesInRings(rings, oCloud, yCloud, coordLow, coordHigh,
                                identicalCloud);
}

void selectAtomsInSliceWithRingEdgeAtoms(
    std::string path, const std::vector<std::vector<int>> &rings, Cloud &oCloud,
    Cloud &yCloud, std::array<double, 3> coordLow,
    std::array<double, 3> coordHigh, bool identicalCloud) {
  ring::printSliceGetEdgeMoleculesInRings(std::move(path), rings, oCloud,
                                          yCloud, coordLow, coordHigh,
                                          identicalCloud);
}

int bulkRingNumberAnalysis(std::string path,
                           std::vector<std::vector<int>> rings,
                           std::vector<std::vector<int>> nList,
                           Cloud &yCloud, int maxDepth, int firstFrame) {
  return ring::bulkPolygonRingAnalysis(std::move(path), rings, nList, yCloud,
                                       maxDepth, firstFrame);
}

int bulkTopologicalNetworkCriterion(std::string path,
                                    const std::vector<std::vector<int>> &rings,
                                    const std::vector<std::vector<int>> &nList,
                                    Cloud &yCloud, int firstFrame,
                                    bool onlyTetrahedral) {
  return ring::topoBulkAnalysis(std::move(path), rings, nList, yCloud,
                                firstFrame, onlyTetrahedral);
}

int bulkTopoUnitMatching(std::string path, std::vector<std::vector<int>> rings,
                         std::vector<std::vector<int>> nList, Cloud &yCloud,
                         int firstFrame, bool printClusters,
                         bool onlyTetrahedral,
                         sol::optional<std::string> templatePath) {
  return tum3::topoUnitMatchingBulk(
      std::move(path), std::move(rings), std::move(nList), yCloud, firstFrame,
      printClusters, onlyTetrahedral, templatePath.value_or("templates"));
}

void registerTopology(sol::state_view lua, sol::table m) {
  m.set_function("ringAnalysis", ringAnalysis);
  m.set_function("calcRDF", calcRDF);
  m.set_function("calcRDF3D", calcRDF3D);
  m.set_function("calcRunningCN", calcRunningCN);
  m.set_function("calcCN", calcCN);
  m.set_function("densityByType", densityByType);
  m.set_function("densityByKind", densityByKind);
  m.set_function("prismAnalysis", &luaApi::prismAnalysis);
  m.set_function("clusterAnalysis", clusterAnalysis);
  m.set_function("recenterCluster", recenterCluster);
  m.set_function("getPointCloudAtomsOfOneAtomType",
                 getPointCloudAtomsOfOneAtomType);
  m.set_function("selectInSingleSlice", selectInSingleSlice);
  m.set_function("selectEdgeAtomsInRingsWithinSlice",
                 selectEdgeAtomsInRingsWithinSlice);
  m.set_function("selectAtomsInSliceWithRingEdgeAtoms",
                 selectAtomsInSliceWithRingEdgeAtoms);
  m.set_function("bulkRingNumberAnalysis", &luaApi::bulkRingNumberAnalysis);
  m.set_function("bulkTopologicalNetworkCriterion",
                 bulkTopologicalNetworkCriterion);
  m.set_function("bulkTopoUnitMatching", bulkTopoUnitMatching);
}

void registerSite(sol::state_view lua, sol::table m) {
  lua.new_enum<site::Kind>("SiteKind",
                           {{"unspecified", site::Kind::unspecified},
                            {"cationHead", site::Kind::cationHead},
                            {"anion", site::Kind::anion},
                            {"tail", site::Kind::tail},
                            {"donorH", site::Kind::donorH},
                            {"acceptor", site::Kind::acceptor},
                            {"polar", site::Kind::polar},
                            {"apolar", site::Kind::apolar},
                            {"waterO", site::Kind::waterO},
                            {"waterH", site::Kind::waterH},
                            {"solvent", site::Kind::solvent}});
  lua.new_enum<site::Family>("SiteFamily",
                             {{"waterIce", site::Family::waterIce},
                              {"ionicLiquid", site::Family::ionicLiquid},
                              {"moltenSalt", site::Family::moltenSalt},
                              {"des", site::Family::des},
                              {"electrolyte", site::Family::electrolyte},
                              {"confinedIL", site::Family::confinedIL},
                              {"confinedWater", site::Family::confinedWater},
                              {"networkFormer", site::Family::networkFormer}});
  lua.new_usertype<site::Table>(
      "SiteTable", sol::constructors<site::Table()>(), "setType",
      [](site::Table &t, int typeId, site::Kind k) {
        t.typeToKind[typeId] = k;
      },
      "ofType", &site::Table::ofType);
  m["SiteKind"] = lua["SiteKind"];
  m["Kind"] = lua["SiteKind"];
  m["SiteFamily"] = lua["SiteFamily"];
  m["Family"] = lua["SiteFamily"];
  m["SiteTable"] = lua["SiteTable"];
  m.set_function("parseSiteSpec", [](const std::string &spec) {
    return site::parseSiteSpec(spec);
  });
  m.set_function("ionCloud", site::ionCloud);
  m.set_function("indicesOf", site::indicesOf);
  m.set_function("contactPairs", contactPairs);
  m.set_function("domainStats", domainStats);
}

void registerAll(sol::state_view lua, sol::table m) {
  registerIO(lua, m);
  registerNeighbours(lua, m);
  registerRings(lua, m);
  registerOrder(lua, m);
  registerDescriptors(lua, m);
  registerTopology(lua, m);
  registerSite(lua, m);
}

} // namespace luaApi
