//-----------------------------------------------------------------------------------
// d-SEAMS - Deferred Structural Elucidation Analysis for Molecular Simulations
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------------

#include <lua_api.hpp>
#include <sol/sol.hpp>

#if __has_include(<seams_config.hpp>)
#include <seams_config.hpp>
#endif

extern "C" int luaopen_dseams_core(lua_State *L) {
#if __has_include(<seams_config.hpp>)
  seams::cfg::load();
#endif
  sol::state_view lua(L);
  sol::table m = lua.create_table();
  luaApi::registerAll(lua, m);
  sol::stack::push(lua, m);
  return 1;
}
