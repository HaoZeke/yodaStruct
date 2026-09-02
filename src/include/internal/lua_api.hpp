//-----------------------------------------------------------------------------------
// d-SEAMS - Deferred Structural Elucidation Analysis for Molecular Simulations
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------------

#ifndef SEAMS_LUA_API_H_
#define SEAMS_LUA_API_H_

// The build writes lua.hpp (absolute pkg-config 5.3/5.4 headers)
// so sol's #include <lua.hpp> does not pick a distro 5.5 header.
#include <mol_sys.hpp>
#include <site.hpp>
#include <sol/sol.hpp>

#include <array>
#include <string>
#include <tuple>
#include <vector>

/** @file lua_api.hpp
 *  @brief Lua names on `dseams.core`, documented for Doxygen.
 *
 *  Each function here is registered by `luaApi::registerAll`. Names
 *  that take a neighbour list or a ring list take that container by
 *  value, so a Lua table of tables and a container userdata both
 *  bind. `sol::as_nested` / `sol::as_table` wrappers return Lua
 *  tables. A raw bind of a C++ vector returns container userdata.
 */

namespace luaApi {

using Cloud = molSys::PointCloud<molSys::Point<double>, double>;

/** @defgroup dseams_core dseams.core
 *  Names on `require("dseams").core`. The public helpers
 *  (`read`, `neighbors`, `knn`, `chill_plus`, `cages`) live in
 *  `lua/dseams.lua` and call these.
 *  @{
 */

/** @name I/O
 *  @{
 */

/** One complete LAMMPS dump frame with every atom type. */
Cloud readLammpsTrj(std::string filename, int targetFrame);

/** One LAMMPS dump frame, one atom type. isSlice sets inSlice; atoms stay.
 *  The type argument is any LAMMPS type (the O is historical). */
Cloud readLammpsTrjO(std::string filename, int targetFrame, int typeO,
                     sol::optional<bool> isSlice,
                     sol::optional<std::array<double, 3>> low,
                     sol::optional<std::array<double, 3>> high);

/** One LAMMPS dump frame, dropping atoms outside the slice when asked.
 *  nop is the kept count. An axis with lo == hi is unconstrained. */
Cloud readLammpsTrjreduced(std::string filename, int targetFrame, int typeI,
                           sol::optional<bool> isSlice,
                           sol::optional<std::array<double, 3>> low,
                           sol::optional<std::array<double, 3>> high);

/** XYZ coordinates, whole file. */
Cloud readXYZ(const std::string &filename);

#ifdef SEAMS_HAS_CHEMFILES
/** One chemfiles frame. `typeFilter` -1 keeps every type. */
Cloud readChemfiles(std::string filename, int targetFrame,
                    sol::optional<int> typeFilter);
#endif

#ifdef SEAMS_HAS_READCON
/** One readcon `.con` frame. */
Cloud readCon(std::string filename, int targetFrame);
#endif

/** Write a LAMMPS dump. */
int writeDump(const Cloud &yCloud, std::string path, std::string outFile);

/** Write `cij` / `q6` / `q3` columns. */
int writeHistogram(const Cloud &yCloud,
                   const std::vector<std::vector<int>> &nList,
                   const std::vector<double> &avgQ6);

/** @} */

/** @name Neighbours
 *  @{
 */

/** Distance-cutoff neighbour list, one type, by atom ID. */
std::vector<std::vector<int>> neighListO(double rcutoff, const Cloud &yCloud,
                                         int typeI);

/** Index-ordered neighbour list built from the cloud. */
std::vector<std::vector<int>> getNewNeighbourListByIndex(const Cloud &yCloud,
                                                         double cutoff);

/** Convert an ID-ordered list to cloud indices. */
std::vector<std::vector<int>>
neighbourListByIndex(const Cloud &yCloud, std::vector<std::vector<int>> nList);

/** Periodic k-nearest graph. `mutual` defaults to true (intersection).
 *  `false` is the union graph. */
std::vector<std::vector<int>> kNearestNeighbourList(const Cloud &yCloud, int k,
                                                    double candidateCutoff,
                                                    int typeI,
                                                    sol::optional<bool> mutual);

/** `{max d_k, min d_{k+1}}`. The cutoff graph equals the k-nearest
 *  graph when `max d_k <= rcut <= min d_{k+1}`. */
std::tuple<double, double> shellSeparation(const Cloud &yCloud, int k,
                                           int typeI);

/** Hydrogen-bond network. Defaults 2.42 A and 30 deg. */
std::vector<std::vector<int>>
getHbondNetwork(std::string filename, Cloud &yCloud,
                std::vector<std::vector<int>> nList, int targetFrame, int Htype,
                sol::optional<double> dist, sol::optional<double> angle);

/** Hydrogen-bond network from an oxygen cloud and a hydrogen cloud. */
std::vector<std::vector<int>> getHbondNetworkFromClouds(
    Cloud &yCloud, Cloud &hCloud, std::vector<std::vector<int>> nList,
    sol::optional<double> dist, sol::optional<double> angle);

/** I-J neighbour list. Like-type reuses neighListO. */
std::vector<std::vector<int>> neighListPair(double rcutoff, const Cloud &yCloud,
                                            int typeI, int typeJ);

/** Hydrogen-bond network from a flat list of hCloud indices. */
std::vector<std::vector<int>>
getHbondNetworkFromDonors(Cloud &yCloud, Cloud &hCloud,
                          std::vector<std::vector<int>> nList,
                          std::vector<int> donorHs, sol::optional<double> dist,
                          sol::optional<double> angle);

/** @} */

/** @name Rings and cages
 *  @{
 */

/** Primitive rings, hop-bounded. `nList` binds by value.
 *  `getPrimitiveRings` is the same function. */
std::vector<std::vector<int>> ringNetwork(std::vector<std::vector<int>> nList,
                                          int maxDepth);

/** Claim-free HC/DDC flags on six-rings. Returns `{hc = hc, ddc = ddc}`. */
sol::table cageAffiliation(sol::this_state ts,
                           std::vector<std::vector<int>> rings,
                           std::vector<std::vector<int>> nList);

/** Mutual-graph seeds, permissive-graph completion. Returns `{hc = hc, ddc = ddc}`.
 *
 *  The four ring and neighbour arguments are by-value vectors: a Lua
 *  table of tables and a container userdata both bind.
 *
 *  The fifth argument `ringAdjacentCompletion` is optional and
 *  defaults to false. When true, the accepted labels pass through
 *  `ring::ringAdjacentCompletion` on the permissive six-rings. HC
 *  and DDC complete on separate flag vectors. `dseams.cages` passes
 *  `opts.complete`.
 */
sol::table seededCageAffiliation(sol::this_state ts,
                                 std::vector<std::vector<int>> strictRings,
                                 std::vector<std::vector<int>> strictNList,
                                 std::vector<std::vector<int>> permRings,
                                 std::vector<std::vector<int>> permNList,
                                 sol::optional<bool> ringAdjacentCompletion);

/** @} */

/** @name Order parameters
 *  @{
 */

/** Classify bonds with a named rule or a `{staggeredMax = staggeredMax,
 *  eclipsedMin = eclipsedMin, eclipsedMax = eclipsedMax,
 *  coordinationNumber = coordinationNumber}` table. */
void classifyBonds(Cloud &yCloud, std::vector<std::vector<int>> nList,
                   const sol::object &ruleSpec, sol::optional<bool> isSlice);

/** Register a named bond-classification rule. */
void registerBondClassifier(std::string name, const sol::table &t);

/** Names of registered bond-classification rules. */
std::vector<std::string> bondClassifierNames();

/** CHILL+ `c_ij`. */
void getCorrelPlus(Cloud &yCloud, std::vector<std::vector<int>> nList,
                   sol::optional<bool> isSlice,
                   sol::optional<int> coordinationNumber);

/** CHILL+ ice types, write a file, return state names. */
std::vector<std::string>
getIceTypePlus(Cloud &yCloud, std::vector<std::vector<int>> nList,
               std::string path, int firstFrame, sol::optional<bool> isSlice,
               sol::optional<std::string> outputFileName);

/** CHILL+ ice types, no file. */
std::vector<std::string>
getIceTypePlusNoPrint(Cloud &yCloud, std::vector<std::vector<int>> nList,
                      sol::optional<bool> isSlice);

/** CHILL `c_ij`. */
void getCorrel(Cloud &yCloud, std::vector<std::vector<int>> nList,
               sol::optional<bool> isSlice,
               sol::optional<int> coordinationNumber);

/** CHILL ice types, write a file, return state names. */
std::vector<std::string> getIceType(Cloud &yCloud,
                                    std::vector<std::vector<int>> nList,
                                    std::string path, int firstFrame,
                                    sol::optional<bool> isSlice,
                                    sol::optional<std::string> outputFileName);

/** CHILL ice types, no file. */
std::vector<std::string> getIceTypeNoPrint(Cloud &yCloud,
                                           std::vector<std::vector<int>> nList,
                                           sol::optional<bool> isSlice);

/** `{ql = ql, qlBar = qlBar}` from a neighbour list. */
sol::table steinhardtQl(sol::this_state ts, const Cloud &yCloud,
                        std::vector<std::vector<int>> nList, int orderL);

/** `{ql = ql, qlBar = qlBar}` from a Voronoi neighbour list. */
sol::table steinhardtQlVoronoi(sol::this_state ts, const Cloud &yCloud,
                               double candidateCutoff, int orderL);

/** Per-atom `{neighbours = neighbours, weights = weights, certified = certified}`. */
sol::table voronoiFacetWeights(sol::this_state ts, const Cloud &yCloud,
                               double candidateCutoff);

/** @} */

/** @name Structure descriptors
 *  @{
 */

/** Overlay each neighbour shell onto FCC, HCP, BCC, SC. */
sol::table classifyTemplates(sol::this_state ts, const Cloud &cloud,
                             std::vector<std::vector<int>> nList, int kNeigh);

/** Bartok SOAP of one particle. */
std::vector<double> soapSpectrum(const Cloud &yCloud, int iatom,
                                 std::vector<std::vector<int>> nList, int nMax,
                                 int lMax, double rcut);

/** SOAP of every particle. */
std::vector<std::vector<double>>
soapSpectrumAll(const Cloud &yCloud, std::vector<std::vector<int>> nList,
                int nMax, int lMax, double rcut);

/** Per-atom `{q4, q6, q8}` from one Voronoi pass. */
std::vector<std::vector<double>> voronoiFeatures(const Cloud &yCloud,
                                                 double candidateCutoff);

/** @} */

/** @name Topology, clusters, and selection
 *  @{
 */

/** Quasi-2D polygon-ring analysis. Writes under `path`. */
int ringAnalysis(std::string path, std::vector<std::vector<int>> rings,
                 std::vector<std::vector<int>> nList, Cloud &yCloud,
                 int maxDepth, double sheetArea, int firstFrame);

/** Quasi-2D RDF, same species. Accumulates into `rdfValues`. */
int calcRDF(std::string path, std::vector<double> &rdfValues,
            const Cloud &yCloud, double cutoff, double binwidth, int firstFrame,
            int finalFrame);

/** Partial 3D RDF g_IJ(r). Returns `{r = r, g = g}` bin centres and values. */
sol::table calcRDF3D(sol::this_state ts, const Cloud &yCloud, int typeI,
                     int typeJ, double rmax, int nbins);

/** Running site-site CN from the same histogram. `rhoJ = nJ / volume`.
 *  Returns `{r = r, cn = cn}` bin centres and the integral at each bin outer edge. */
sol::table calcRunningCN(sol::this_state ts, const Cloud &yCloud, int typeI,
                         int typeJ, double rmax, int nbins);

/** Site-site CN to `rCut` (default `rmax`). `rhoJ = nJ / volume`. */
double calcCN(const Cloud &yCloud, int typeI, int typeJ, double rmax, int nbins,
              sol::optional<double> rCut);

/** Type-resolved Cartesian number-density profile. */
sol::table densityByType(sol::this_state ts, const Cloud &yCloud, int typeI,
                         int nbin, int axis);

/** Site-kind-resolved Cartesian number-density profile. */
sol::table densityByKind(sol::this_state ts, const Cloud &yCloud,
                         const site::Table &table, site::Kind kind, int nbin,
                         int axis);

/** Mutual nearest cation-anion pairs and ion counts. */
sol::table contactPairs(sol::this_state ts, const Cloud &cloud,
                        const site::Table &table);

/** Largest cutoff-connected mapped site domain. */
sol::table domainStats(sol::this_state ts, const Cloud &cloud,
                       const site::Table &table, site::Kind kind,
                       double cutoff);

/** Quasi-1D prism analysis. `atomID` is the first-frame ID.
 *  `rings` and `nList` bind by value (Lua table or userdata). */
int prismAnalysis(std::string path, std::vector<std::vector<int>> rings,
                  std::vector<std::vector<int>> nList, Cloud &cloud,
                  int maxDepth, int atomID, int firstFrame, int currentFrame,
                  bool doShapeMatching);

/** Largest ice cluster into `iceCloud`. `bopAnalysis` is `"q6"` or `"chill"`.
 */
int clusterAnalysis(std::string path, Cloud &iceCloud, Cloud &yCloud,
                    const std::vector<std::vector<int>> &nList,
                    std::vector<std::vector<int>> &iceNeighbourList,
                    double cutoff, int firstFrame, std::string bopAnalysis);

/** Recenter the largest-cluster cloud. */
int recenterCluster(Cloud &iceCloud,
                    const std::vector<std::vector<int>> &nList);

/** Copy one atom type into `outCloud`. */
Cloud getPointCloudAtomsOfOneAtomType(Cloud &yCloud, Cloud &outCloud,
                                      int atomTypeI, bool isSlice,
                                      std::array<double, 3> coordLow,
                                      std::array<double, 3> coordHigh);

/** Mark molecules whose atoms fall in a single AABB slice. */
void selectInSingleSlice(Cloud &yCloud, bool clearPreviousSliceSelection,
                         std::array<double, 3> coordLow,
                         std::array<double, 3> coordHigh);

/** Rings that touch the slice keep every member. */
void selectEdgeAtomsInRingsWithinSlice(
    const std::vector<std::vector<int>> &rings, Cloud &oCloud, Cloud &yCloud,
    std::array<double, 3> coordLow, std::array<double, 3> coordHigh,
    bool identicalCloud);

/** Same selection, then write molecule IDs and a LAMMPS data file. */
void selectAtomsInSliceWithRingEdgeAtoms(
    std::string path, const std::vector<std::vector<int>> &rings, Cloud &oCloud,
    Cloud &yCloud, std::array<double, 3> coordLow,
    std::array<double, 3> coordHigh, bool identicalCloud);

/** Bulk ring-number histogram, every size up to `maxDepth`.
 *  `rings` and `nList` bind by value (Lua table or userdata). */
int bulkRingNumberAnalysis(std::string path,
                           std::vector<std::vector<int>> rings,
                           std::vector<std::vector<int>> nList,
                           Cloud &yCloud, int maxDepth, int firstFrame);

/** Bulk DDC/HC topological network criterion. */
int bulkTopologicalNetworkCriterion(std::string path,
                                    const std::vector<std::vector<int>> &rings,
                                    const std::vector<std::vector<int>> &nList,
                                    Cloud &yCloud, int firstFrame,
                                    bool onlyTetrahedral);

/** Topological unit matching. `templatePath` defaults to `"templates"`. */
int bulkTopoUnitMatching(std::string path, std::vector<std::vector<int>> rings,
                         std::vector<std::vector<int>> nList, Cloud &yCloud,
                         int firstFrame, bool printClusters,
                         bool onlyTetrahedral,
                         sol::optional<std::string> templatePath);

/** @} */

/** @name Usertypes
 *  @{
 *
 *  `PointCloud`: `nop`, `currentFrame`, `box()`, `boxLow()`, `iceTypes()`.
 *  `RingUpdater(maxDepth)`: `update(nList)`, `lastRecomputedSources`,
 *  `lastBallsRefreshed`.
 *  `AffiliationUpdater`: `update(rings, nList)` returns `{hc = hc, ddc = ddc}`,
 *  `lastReclassified`.
 *  @}
 */

/** @name Legacy workflow names
 *  @{
 *
 *  Spellings used by the workflow scripts under `example_lua/`.
 *  Nested-table arguments bind by value: `bondNetworkByIndex`,
 *  `getPrimitiveRings`, `prismAnalysis`, and
 *  `bulkRingNumberAnalysis` accept a Lua table or a container
 *  userdata. The worked example is
 *  `example_lua/library/legacy_chain.lua`.
 *
 *  Readers: `readFrameOnlyOne`, `readFrameOnlyOneAllAtoms`, `readFrame`.
 *  Neighbours: `neighborList`, `bondNetworkByIndex`, `getPrimitiveRings`.
 *  CHILL: `chillPlus_cij`, `chillPlus_iceType`, `chill_cij`,
 *  `chill_iceType`, `averageQ6`, `modifyChill`, `percentage_Ice`.
 *  @}
 */

/** @} */ // end dseams_core

//! PointCloud usertype, trajectory readers and the file writers
void registerIO(sol::state_view lua, sol::table m);

//! Neighbour list construction, by atom ID and by cloud index
void registerNeighbours(sol::state_view lua, sol::table m);

//! Primitive ring enumeration and the RingUpdater usertype
void registerRings(sol::state_view lua, sol::table m);

//! CHILL, CHILL+, Steinhardt and Voronoi-weighted order parameters
void registerOrder(sol::state_view lua, sol::table m);

//! Template overlay, SOAP and Voronoi structure descriptors
void registerDescriptors(sol::state_view lua, sol::table m);

//! Topological network criteria, clustering and selection analyses
void registerTopology(sol::state_view lua, sol::table m);

//! Site tables, ion COM clouds, contact pairs and connected domains
void registerSite(sol::state_view lua, sol::table m);

//! Every registration group above, into table m
void registerAll(sol::state_view lua, sol::table m);

} // namespace luaApi

#endif // SEAMS_LUA_API_H_
