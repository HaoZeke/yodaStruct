=========================
Call dseams from Fennel
=========================

Problem
=======

You want lisp syntax on the same library a Lua script loads with
``require("dseams")``. No ``yodaStruct`` binary. No YAML config.

Paths
=====

Meson, from the repository root:

.. code-block:: bash

   meson setup bbdir --wrap-mode=nofallback
   meson compile -C bbdir
   export LUA_PATH="$PWD/lua/?.lua;;"
   export LUA_CPATH="$PWD/bbdir/?.so;;"

Nix sets the same two variables to the installed library:

.. code-block:: bash

   nix develop

The vendored compiler lives at
``src/include/external/fennel/fennel.lua``. A meson install puts a
copy at ``$prefix/share/luadseams/fennel.lua``. Invoke it as a Lua
script. It already puts ``fennel`` on ``package.loaded``.

``(require :dseams)``
=====================

The library example lives at ``example_lua/library/chill.fnl``:

.. code-block:: fennel

   (local dseams (require :dseams))
   (local cloud (dseams.read "input/traj/exampleTraj.lammpstrj" {:type 2}))
   (assert (> cloud.nop 0))
   (local types (dseams.chill_plus cloud {:cutoff 3.5 :type 2}))
   (print (string.format "dseams_fnl nop=%d ntypes=%d" cloud.nop (length types)))

Run it from the repository root:

.. code-block:: bash

   lua src/include/external/fennel/fennel.lua example_lua/library/chill.fnl

Fennel ``require`` falls through to Lua ``package.path``, so
``(require :dseams)`` returns the same table as ``require("dseams")``:
``read``, ``neighbors``, ``knn``, ``chill_plus``, ``chill``, ``cages``, and
``.core``. Names stay snake_case.

Scripts that mention ``readFrameOnlyOne``, ``functionScript``, or
``trajectory`` as a global expect the 2020 driver. Rewrite them to
``(require :dseams)``. ``example_lua/fennel/script.fnl`` is that older
shape.

``lua/dseams.fnl``
==================

``lua/dseams.fnl`` wraps that table with kebab-case names
(``chill-plus``) and re-exports ``.core``. Load it with ``fennel.dofile``
under a name that is not ``dseams``, so the inner ``(require :dseams)``
still hits the Lua module:

.. code-block:: fennel

   (local fennel (require :fennel))
   (local dseams (fennel.dofile "lua/dseams.fnl"))
   (local cloud (dseams.read "input/traj/exampleTraj.lammpstrj" {:type 2}))
   (print (dseams.chill-plus cloud {:cutoff 3.5 :type 2}))

Putting ``lua/?.fnl`` on ``fennel.path`` and then ``(require :dseams)``
would compile ``dseams.fnl`` as ``dseams`` and load that file as
itself. Do not do that.

meson installs ``dseams.fnl`` next to ``dseams.lua`` under
``share/luadseams/lua/``. Nix does not symlink it into the versioned
``share/lua/`` tree. After ``meson install``, pass the installed path
to ``fennel.dofile``.

``(require :yoda)`` aliases the Lua module. Prefer ``(require :dseams)``.

See also
========

- :doc:`../tutorials/read-and-classify`
- :doc:`embed-lua`
- :doc:`install`
