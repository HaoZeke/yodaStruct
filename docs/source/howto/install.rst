====================
Install the library
====================

Problem
=======

You need ``require("dseams")`` to resolve in a ``lua`` process. That
takes two files: the helpers (``lua/dseams.lua``) and the compiled
module (``dseams_core.so``). This page is how meson builds the
``.so``, where an install puts both files, and how Lua finds them.

This tree builds no ``yodaStruct`` executable. The engine CLI is
``seams`` in `seams-core <https://github.com/d-SEAMS/seams-core>`_.

What meson emits
================

The meson project name is ``luadseams``. The only compile target is
the shared module ``dseams_core`` (``src/lua_api.cpp`` and
``src/luaopen.cpp``), with ``name_prefix`` empty, so the file on disk
is ``dseams_core.so``. The C entry is ``luaopen_dseams_core``.

seams-core is a meson subproject, linked static, with
``with_python=false``, ``with_tests=false``, ``with_cli=false``, and
``with_lua=disabled``. This library does not grow a second engine
CLI.

Lua is resolved as pkg-config ``lua``, then ``lua-5.4``, then a wrap
fallback. ``--wrap-mode=nofallback`` keeps the fallback off when a
system Lua is present.

Installed data (meson ``install: true`` on the module, plus
``install_data``):

=============================================  ============================================
artifact                                       default destination
=============================================  ============================================
``dseams_core.so``                             ``$prefix/lib/dseams_core.so``
``dseams.lua``, ``yoda.lua``, ``dseams.fnl``   ``$prefix/share/luadseams/lua/``
vendored ``fennel.lua``                        ``$prefix/share/luadseams/``
=============================================  ============================================

meson does not rewrite Lua's default ``package.path``. You set
``LUA_PATH`` and ``LUA_CPATH``, or you symlink into a versioned Lua
tree.

In-tree build
=============

From the repository root:

.. code-block:: bash

   meson setup bbdir --wrap-mode=nofallback
   meson compile -C bbdir
   export LUA_PATH="$PWD/lua/?.lua;;"
   export LUA_CPATH="$PWD/bbdir/?.so;;"
   lua example_lua/library/read.lua

The meson test ``dseams_library_read`` sets those two variables the
same way and runs ``example_lua/library/read.lua`` with
``workdir`` at the source root.

meson install
=============

.. code-block:: bash

   meson setup bbdir --wrap-mode=nofallback --prefix="$PWD/prefix"
   meson compile -C bbdir
   meson install -C bbdir
   export LUA_PATH="$PWD/prefix/share/luadseams/lua/?.lua;;"
   export LUA_CPATH="$PWD/prefix/lib/?.so;;"
   lua example_lua/library/read.lua

``require("dseams")`` loads ``dseams.lua``, which calls
``require("dseams_core")``. ``package.cpath`` must match
``dseams_core.so`` (no ``lib`` prefix). A ``cpath`` of
``$prefix/lib/lua/5.4/?.so`` does not find
``$prefix/lib/dseams_core.so`` unless you add a symlink.

Nix
===

The flake package installs the meson layout, then:

- symlinks ``dseams.lua`` and ``yoda.lua`` into
  ``$out/share/lua/${luaversion}/``
- symlinks ``dseams_core.so`` into ``$out/lib/lua/${luaversion}/``
  when the file exists
- writes a setup-hook that prepends
  ``$out/share/luadseams/lua/?.lua`` to ``LUA_PATH`` and
  ``$out/lib/?.so`` to ``LUA_CPATH``

``nix develop`` sets those two variables to the installed library:

.. code-block:: bash

   nix build
   nix develop
   lua example_lua/library/read.lua

``dseams.fnl`` is installed under ``share/luadseams/lua/``. It is not
on the versioned ``share/lua/`` path. Load kebab-case wrappers with
``fennel.dofile`` on that file; see :doc:`fennel`.

How require finds the module
============================

.. code-block:: lua

   local dseams = require("dseams")  -- lua/dseams.lua via LUA_PATH
   -- that file does:
   --   local core = require("dseams_core")  -- dseams_core.so via LUA_CPATH

``require("yoda")`` is ``lua/yoda.lua``, which returns
``require("dseams")``.

Optional readers are compile-gated in seams-core:

- ``core.readCon`` exists only with ``SEAMS_HAS_READCON``
- ``core.readChemfiles`` exists only with ``SEAMS_HAS_CHEMFILES``

``dseams.read`` errors with a clear message when you pass ``.con`` /
``.pdb`` / ``.gro`` / ``.dcd`` and the matching bind is missing. LAMMPS
dumps and ``.xyz`` do not need those backends.

Verification
============

.. code-block:: bash

   lua -e 'print(require("dseams").read ~= nil)'
   lua example_lua/library/read.lua

The second command prints ``dseams_lib nop=250`` when the shipped
dump path resolves.

Next steps
==========

- :doc:`../quickstart` :: First ``require``
- :doc:`../tutorials/read-and-classify` :: CHILL+ and cages
- :doc:`embed-lua` :: Load the ``.so`` from C
- :doc:`troubleshooting` :: ``module not found`` and empty clouds
