===============
Troubleshooting
===============

Common problems and their solutions.

Installation Issues
===================

Problem: module 'dseams' not found
----------------------------------

**Cause:** ``package.path`` does not contain ``dseams.lua``.

**Solution:**

.. code-block:: bash

   # In-tree
   export LUA_PATH="$PWD/lua/?.lua;;"

   # After meson install
   export LUA_PATH="$PREFIX/share/luadseams/lua/?.lua;;"

   # Nix
   nix develop

Confirm the file exists at that path, then:

.. code-block:: bash

   lua -e 'print(package.path)'

Problem: module 'dseams_core' not found
---------------------------------------

**Cause:** ``package.cpath`` does not contain ``dseams_core.so``.

**Solution:**

.. code-block:: bash

   # In-tree meson build directory
   export LUA_CPATH="$PWD/bbdir/?.so;;"

   # After meson install (note: lib/?.so, not lib/lua/5.4/?.so)
   export LUA_CPATH="$PREFIX/lib/?.so;;"

The file name is ``dseams_core.so``, not ``libdseams_core.so``. A
``cpath`` pattern of ``$PREFIX/lib/lua/5.4/?.so`` misses
``$PREFIX/lib/dseams_core.so`` unless you add the Nix-style
symlink.

Problem: yodaStruct: command not found
--------------------------------------

**Cause:** This tree builds no executable.

**Solution:** Call ``require("dseams")`` from ``lua``, or call ``seams``
in seams-core. See :doc:`../explanation/library-not-cli`.

Read Issues
===========

Problem: empty cloud / nop=0
----------------------------

**Cause:** Type filter does not match the dump, or the path does
not resolve from the current working directory.

**Solution:**

1. Work from the repository root for shipped examples.
2. Pass the oxygen type:

   .. code-block:: lua

      local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})

3. On a LAMMPS dump, if ``opts.type`` is nil, ``dseams.read`` tries
   type 2 then type 1. An explicit wrong type still yields an
   empty cloud.

Problem: chill_plus returns nothing useful / empty neighbour graph
------------------------------------------------------------------

**Cause:** ``chill_plus`` and ``cages`` default ``opts.type`` to 1. A
cloud of type-2 oxygens plus a type-1 neighbour filter is an
empty graph.

**Solution:**

.. code-block:: lua

   dseams.chill_plus(cloud, {cutoff = 3.5, type = 2})
   dseams.cages(cloud, {type = 2})

Problem: readCon is not in this build (readcon-core missing)
------------------------------------------------------------

**Cause:** seams-core was compiled without ``SEAMS_HAS_READCON``.

**Solution:** Use a LAMMPS dump or ``.xyz``, or rebuild seams-core
with readcon. ``dseams.read`` on ``.con`` raises this error on
purpose.

Problem: readChemfiles is not in this build (chemfiles missing)
---------------------------------------------------------------

**Cause:** seams-core was compiled without ``SEAMS_HAS_CHEMFILES``.

**Solution:** Use a LAMMPS dump or ``.xyz``, or rebuild with
chemfiles. Suffixes ``.pdb``, ``.gro``, and ``.dcd`` take this path.

Classification Issues
=====================

Problem: almost every oxygen is water or unclassified
-----------------------------------------------------

**Cause:** CHILL+ labels the four-neighbour shell. Confined water
and poorly coordinated oxygens land in ``water`` or
``unclassified``.

**Solution:** Print the histogram and add the counts. They sum to
``cloud.nop``. This tree does not pin a particular split. A
``water``-heavy result is a classification, not a crash.

Problem: cages hc=0 ddc=0
-------------------------

**Cause:** Seeded affiliation found no accepted hexagonal cage or
double-diamond cage. An empty strict (mutual) pass accepts
nothing.

**Solution:** Zero is valid. The shipped ``exampleTraj.lammpstrj``
is confined water, not a bulk ice lattice. Do not expect HC / DDC
counts to match the CHILL+ histogram.

Fennel Issues
=============

Problem: (require :dseams) loops or loads the wrapper as itself
---------------------------------------------------------------

**Cause:** ``lua/?.fnl`` is on ``fennel.path``, so ``(require :dseams)``
compiles ``dseams.fnl`` instead of loading ``dseams.lua``.

**Solution:** Leave Fennel ``require`` falling through to Lua
``package.path``. Load kebab-case wrappers with ``fennel.dofile``
under a name that is not ``dseams``. See :doc:`fennel`.

Problem: readFrameOnlyOne is nil
--------------------------------

**Cause:** The script expects the 2020 driver globals.

**Solution:** Rewrite to ``(require :dseams)`` and call
``dseams.read`` / ``dseams.core.``...

Common Error Messages
=====================

=============================================  =======================  =========================================================
Error                                          Cause                    Solution
=============================================  =======================  =========================================================
``module 'dseams' not found``                  ``LUA_PATH``             Point at ``lua/?.lua`` or ``share/luadseams/lua/?.lua``
``module 'dseams_core' not found``             ``LUA_CPATH``            Point at the dir that holds ``dseams_core.so``
``yodaStruct: command not found``              no executable            ``require("dseams")`` or ``seams``
``empty cloud``                                type or cwd              ``{type = 2}``, repository root
``readCon is not in this build``               no readcon               LAMMPS / XYZ, or rebuild
``readChemfiles is not in this build``         no chemfiles             LAMMPS / XYZ, or rebuild
=============================================  =======================  =========================================================

Getting Help
============

If these solutions do not help:

1. Check :doc:`faq`
2. Read :doc:`../reference/lua`
3. Open an issue on `yodaStruct <https://github.com/d-SEAMS/yodaStruct>`_

Include:

- meson / Nix revision
- Lua version (``lua -v``)
- The exact ``LUA_PATH`` and ``LUA_CPATH``
- The script and the full error

See Also
========

- :doc:`faq`
- :doc:`install`
- :doc:`../tutorials/read-and-classify`
