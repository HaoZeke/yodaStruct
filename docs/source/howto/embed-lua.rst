===================
Embed in a Lua host
===================



Problem
-------

Your process already owns ``main``. You want the d-SEAMS
classifiers inside that process. A second CLI does not compose.
Load the shared module.

What to load
------------

Two layers:

1. ``dseams_core.so`` \:\: compiled registrations. C entry
   ``luaopen_dseams_core``. This is ``dseams.core``.

2. ``lua/dseams.lua`` \:\: helpers (``read``, ``neighbors``, ``knn``,
   ``chill_plus``, ``chill``, ``cages``). This file calls
   ``require("dseams_core")``.

A host that loads only ``luaopen_dseams_core`` gets the compiled
names. It does not get ``dseams.read``. Keep ``dseams.lua`` on
``package.path`` unless you call ``dseams.core`` yourself.

Usertypes (``PointCloud``, ``RingUpdater``, ``AffiliationUpdater``)
register on the Lua state when ``dseams_core`` loads.

Stock lua
---------

This is the same path as the `Quickstart <../quickstart.rst>`_:

.. code:: bash

    export LUA_PATH="$PREFIX/share/luadseams/lua/?.lua;;"
    export LUA_CPATH="$PREFIX/lib/?.so;;"
    lua your_script.lua

.. code:: lua

    local dseams = require("dseams")
    local cloud = dseams.read("water.lammpstrj", {type = 2})

In-tree, ``$PREFIX/share/luadseams/lua`` is ``lua/`` and
``$PREFIX/lib`` is the meson build directory (``bbdir/``).

C host via package.cpath
------------------------

Open a state, open the standard libraries, prepend the two
search paths, then ``require("dseams")``. Lua's ``package.cpath``
loader finds ``luaopen_dseams_core`` inside ``dseams_core.so``.

.. code:: c

    #include <stdio.h>
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>

    int main(void) {
      lua_State *L = luaL_newstate();
      luaL_openlibs(L);
      /* Set package.path and package.cpath to the install layout.
         See howto/install. */
      if (luaL_dostring(L,
            "local dseams = require('dseams')\n"
            "assert(dseams.read ~= nil)\n"
            "assert(dseams.core ~= nil)\n") != LUA_OK) {
        fprintf(stderr, "%s\n", lua_tostring(L, -1));
        lua_close(L);
        return 1;
      }
      lua_close(L);
      return 0;
    }

Set the paths from C with ``lua_getglobal(L, "package")`` and
``lua_setfield``, or inherit ``LUA_PATH`` / ``LUA_CPATH`` from the
environment (``luaL_openlibs`` honours both).

C host via luaL\ :sub:`requiref`\
---------------------------------

When you link ``dseams_core`` and do not want a filesystem search
for the ``.so``:

.. code:: c

    #include <lua.h>
    #include <lauxlib.h>

    extern int luaopen_dseams_core(lua_State *L);

    /* After luaL_openlibs(L): */
    luaL_requiref(L, "dseams_core", luaopen_dseams_core, 0);
    lua_pop(L, 1);
    /* package.loaded["dseams_core"] is now set.
       require("dseams") still needs lua/dseams.lua on package.path. */

sol2 exposes the same opener as
``lua.require("dseams_core", luaopen_dseams_core, false)``. That
``require`` takes a C function, not a module-name string. After
the opener is in ``package.loaded``, load the helpers with
``require_file`` on ``dseams.lua``, or set ``package.path`` and run
``require("dseams")`` through the Lua VM.

Fennel host
-----------

Load the vendored compiler, then ``(require :dseams)``. The
compiler is ``src/include/external/fennel/fennel.lua`` in the
tree, or ``$prefix/share/luadseams/fennel.lua`` after install.

.. code:: lua

    local fennel = require("fennel")  -- after fennel.lua is on package.path
    -- or: fennel = dofile("src/include/external/fennel/fennel.lua")
    print(fennel.eval([[
      (local dseams (require :dseams))
      (assert dseams.read)
    ]]))

Kebab-case wrappers: `Fennel <fennel.rst>`_.

What not to do
--------------

- Do not look for a ``yodaStruct`` binary. meson does not emit one.

- Do not inject ``readFrameOnlyOne`` as a global unless you copy
  the seams-core CLI. Library scripts call names on the table
  that ``require("dseams")`` returns, or on ``dseams.core``.

- Do not ``require("dseams_core")`` and expect ``read`` / ``chill_plus``
  / ``cages``. Those helpers stay in Lua.

See also
--------

- `Install the library <install.rst>`_ - Where the ``.so`` lives

- `The library, not a second CLI <../explanation/library-not-cli.rst>`_

- `Lua surface <../reference/lua.rst>`_
