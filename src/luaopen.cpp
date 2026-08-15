//-----------------------------------------------------------------------------------
// d-SEAMS - Deferred Structural Elucidation Analysis for Molecular Simulations
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------------

#include <lua_api.hpp>
#include <sol/sol.hpp>

extern "C" int luaopen_dseams_core(lua_State *L) {
  sol::state_view lua(L);
  sol::table m = lua.create_table();
  luaApi::registerAll(lua, m);
  sol::stack::push(lua, m);
  return 1;
}
