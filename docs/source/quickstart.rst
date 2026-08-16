==========
Quickstart
==========

Call ``require("dseams")`` from Lua, or ``(require :dseams)`` from
Fennel. This tree builds no ``yodaStruct`` executable. Call ``seams``
for the engine CLI, in
`seams-core <https://github.com/d-SEAMS/seams-core>`_.

``require("yoda")`` still resolves to ``dseams``.

Prerequisites
=============

- A Lua 5.4 interpreter on ``PATH``
- meson >= 1.3, ninja, a C++20 compiler, pkg-config
- Work from the repository root so the shipped dump path resolves

How meson emits ``dseams_core.so``, and where an install puts it, is
in :doc:`howto/install`.

Meson
=====

From the repository root. ``LUA_PATH`` finds ``lua/dseams.lua``.
``LUA_CPATH`` finds ``dseams_core.so`` in the build directory.

.. code-block:: bash

   meson setup bbdir --wrap-mode=nofallback
   meson compile -C bbdir
   LUA_PATH="$PWD/lua/?.lua;;" LUA_CPATH="$PWD/bbdir/?.so;;" \
     lua example_lua/library/read.lua

That script is:

.. code-block:: lua

   local dseams = require("dseams")
   local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
   assert(cloud.nop > 0, "empty cloud")
   print(string.format("dseams_lib nop=%d", cloud.nop))

You should see:

.. code-block:: text

   dseams_lib nop=250

``dseams.read`` keeps one atom type. This dump stores oxygen as
LAMMPS type 2, so the option table is ``{type = 2}``.

Nix
===

``nix develop`` sets ``LUA_PATH`` and ``LUA_CPATH`` to the installed
library.

.. code-block:: bash

   nix build
   nix develop
   lua example_lua/library/read.lua

Fennel
======

Fennel loads the same Lua module:

.. code-block:: fennel

   (local dseams (require :dseams))
   (local cloud (dseams.read "input/traj/exampleTraj.lammpstrj" {:type 2}))
   (print (dseams.chill_plus cloud {:cutoff 3.5 :type 2}))

How to invoke the vendored compiler and the kebab-case wrappers in
``lua/dseams.fnl`` is in :doc:`howto/fennel`.

Next steps
==========

- :doc:`tutorials/read-and-classify` :: CHILL+ labels and cage flags on the same cloud
- :doc:`howto/install` :: How the ``.so`` is built and found
- :doc:`howto/embed-lua` :: Load ``dseams_core`` from a process that already owns ``main``
