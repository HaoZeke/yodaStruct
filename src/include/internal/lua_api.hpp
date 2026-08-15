//-----------------------------------------------------------------------------------
// d-SEAMS - Deferred Structural Elucidation Analysis for Molecular Simulations
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------------

#ifndef SEAMS_LUA_API_H_
#define SEAMS_LUA_API_H_

#include <sol/sol.hpp>

/** @file lua_api.hpp
 *  @brief Lua registration of the dseams library.
 *   Each group registers one cohesive slice of the API; registerAll wires
 *   every group into the given table. New-style functions take and return
 *   plain Lua tables; the legacy names keep container-userdata semantics
 *   so older scripts keep running.
 */

namespace luaApi {

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

//! Every registration group above, into table m
void registerAll(sol::state_view lua, sol::table m);

} // namespace luaApi

#endif // SEAMS_LUA_API_H_
