==================================
Tutorial: Read a dump and classify
==================================

    :Author: `Rohit Goswami <https://rgoswami.me>`_


Prerequisites
-------------

- Lua 5.4 on ``PATH``

- meson >= 1.3, ninja, a C++20 compiler, pkg-config

- This repository, with work from the repository root so the dump
  path resolves

- The `Quickstart <../quickstart.rst>`_ build, or the meson / Nix
  steps in Step 1

Learning Objectives
-------------------

By the end of this tutorial you will be able to:

1. Call ``require("dseams")`` from a stock ``lua`` process

2. Read one LAMMPS dump frame into a ``PointCloud``

3. Label each oxygen with ``dseams.chill_plus``

4. Flag hexagonal-cage and double-diamond-cage membership with
   ``dseams.cages``

5. See why those two tables answer different questions

This repository does not pin the CHILL+ histogram. Print the counts
you get. Do not treat a ``water``-heavy split as a failed run.

Step 1: Build the library
-------------------------

Meson. ``LUA_PATH`` finds ``lua/dseams.lua``. ``LUA_CPATH`` finds
``dseams_core.so``.

.. code:: bash

    meson setup bbdir --wrap-mode=nofallback
    meson compile -C bbdir
    export LUA_PATH="$PWD/lua/?.lua;;"
    export LUA_CPATH="$PWD/bbdir/?.so;;"

Nix sets the same two variables to the installed library:

.. code:: bash

    nix build
    nix develop

If you already ran the `Quickstart <../quickstart.rst>`_, the
exports (or the Nix shell) are enough. Skip the compile.

Step 2: Require and read
------------------------

``input/traj/exampleTraj.lammpstrj`` is one frame of 750 atoms: 500
of LAMMPS type 1 (hydrogen) and 250 of type 2 (oxygen). Ice
helpers classify oxygen.

The shipped reader is ``example_lua/library/read.lua``:

.. code:: lua

    local dseams = require("dseams")
    local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
    assert(cloud.nop > 0, "empty cloud")
    print(string.format("dseams_lib nop=%d", cloud.nop))

.. code:: bash

    lua example_lua/library/read.lua

You should see:

::

    dseams_lib nop=250

``dseams.read`` keeps one atom type. Pass ``{type = 2}`` on this dump.
``chill_plus`` and ``cages`` build neighbour graphs with
``opts.type or 1``, so they need ``type = 2`` as well. A cloud of
oxygens plus a type-1 neighbour filter is an empty graph.

Step 3: Label with CHILL+
-------------------------

Save this as ``/tmp/read-and-classify.lua``. It is the rest of the
tutorial.

.. code:: lua

    local dseams = require("dseams")

    local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})
    assert(cloud.nop > 0, "empty cloud")

    local box = cloud:box()
    print(string.format("nop=%d frame=%d box={%.2f, %.2f, %.2f}",
      cloud.nop, cloud.currentFrame, box[1], box[2], box[3]))

    local types = dseams.chill_plus(cloud, {cutoff = 3.5, type = 2})
    assert(#types == cloud.nop)

    local counts = {}
    for i = 1, #types do
      counts[types[i]] = (counts[types[i]] or 0) + 1
    end
    local parts = {}
    for name, n in pairs(counts) do
      parts[#parts + 1] = string.format("%s=%d", name, n)
    end
    table.sort(parts)
    print("chill_plus " .. table.concat(parts, " "))

    local aff = dseams.cages(cloud, {type = 2})
    assert(#aff.hc == cloud.nop and #aff.ddc == cloud.nop)

    local function nflag(flags)
      local n = 0
      for i = 1, #flags do
        if flags[i] and flags[i] ~= 0 then
          n = n + 1
        end
      end
      return n
    end
    print(string.format("cages hc=%d ddc=%d nop=%d",
      nflag(aff.hc), nflag(aff.ddc), cloud.nop))

.. code:: bash

    lua /tmp/read-and-classify.lua

The first line is the 250 oxygens, frame 1, and the box
(40 x 40 x 180 Angstrom, a long z). The ice-nanotube examples in
``example_lua/iceNanotube/`` use this dump.

``dseams.chill_plus`` builds a 3.5 Angstrom cutoff neighbour list,
runs ``getCorrelPlus``, and returns ``getIceTypePlusNoPrint``. No file
is written. The cloud is mutated. The return value is a 1-based
array of state names, length ``cloud.nop``.

Names you can see: ``cubic``, ``hexagonal``, ``water``, ``interfacial``,
``clathrate``, ``interClathrate``, ``unclassified``.

CHILL+ looks at the four-neighbour shell. A bond with correlation
<= -0.8 is staggered; a bond in [-0.35, 0.25] is eclipsed.

- 4 staggered: cubic

- 3 staggered and 1 eclipsed: hexagonal

- 4 eclipsed: clathrate

- 3 eclipsed: interClathrate

- mixed ice-like bonds: interfacial

- anything else, or not 4-coordinated: water

Print the histogram. Add the counts. They sum to 250. This tree
does not assert a particular split. A confined water dump can
land mostly in ``water`` and ``interfacial``. That is a
classification, not a failed run.

Step 4: Flag cages
------------------

``dseams.cages`` builds two 4-nearest graphs (mutual = strict,
union = permissive; candidate cutoff 5.0), keeps six-membered
rings, and returns ``seededCageAffiliation``: a table
``{hc = ..., ddc = ...}`` of per-atom flags, each length
``cloud.nop``.

- ``hc[i]`` is true when oxygen ``i`` belongs to an accepted
  hexagonal cage.

- ``ddc[i]`` is true when it belongs to an accepted double-diamond
  cage.

HC and DDC are bulk ice motifs (two basal six-rings plus
prisms; one equatorial six-ring plus six peripherals). Seeded
affiliation accepts a permissive-graph atom only when its
affiliated component contains a mutual-graph seed. An empty
strict pass accepts nothing.

Print ``hc`` and ``ddc`` counts. Zero is a valid answer: this dump
is confined water, not a bulk ice lattice.

Step 5: Read both tables
------------------------

``chill_plus`` is a local bond-order label. ``cages`` is six-ring
membership. A cubic oxygen need not sit in a DDC. An HC oxygen
need not be labeled hexagonal. Read both tables; do not expect
them to match.

The helpers live in ``lua/dseams.lua``. The compiled names they
call are on ``dseams.core``; see
`Lua surface <../reference/lua.rst>`_.

Complete Example Script
-----------------------

The block in Step 3 is the complete script. Save it as
``/tmp/read-and-classify.lua`` and run it from the repository root
with ``LUA_PATH`` and ``LUA_CPATH`` set.

Fennel
------

``(require :dseams)`` is the same Lua table. Names stay
snake\ :sub:`case`\. The vendored compiler is
``src/include/external/fennel/fennel.lua``.

.. code:: fennel

    (local dseams (require :dseams))
    (local cloud (dseams.read "input/traj/exampleTraj.lammpstrj" {:type 2}))
    (local types (dseams.chill_plus cloud {:cutoff 3.5 :type 2}))
    (local aff (dseams.cages cloud {:type 2}))
    (print (string.format "nop=%d ntypes=%d nhc=%d nddc=%d"
                          cloud.nop (length types) (length aff.hc)
                          (length aff.ddc)))

Save the snippet as ``/tmp/read-and-classify.fnl``, or run the
shipped file ``example_lua/library/chill.fnl`` (that one stops
after ``chill_plus``):

.. code:: bash

    lua src/include/external/fennel/fennel.lua /tmp/read-and-classify.fnl
    lua src/include/external/fennel/fennel.lua example_lua/library/chill.fnl

How Fennel finds ``dseams``, and how ``lua/dseams.fnl`` adds
``chill-plus``, is in `Fennel <../howto/fennel.rst>`_.

Troubleshooting
---------------

module 'dseams' not found
~~~~~~~~~~~~~~~~~~~~~~~~~

``LUA_PATH`` does not contain ``lua/?.lua``. From the repository
root:

.. code:: bash

    export LUA_PATH="$PWD/lua/?.lua;;"

module 'dseams\ :sub:`core`\' not found
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``LUA_CPATH`` does not contain the build directory that holds
``dseams_core.so``:

.. code:: bash

    export LUA_CPATH="$PWD/bbdir/?.so;;"

empty cloud, or ``nop=0``
~~~~~~~~~~~~~~~~~~~~~~~~~

You omitted ``{type = 2}`` on a dump whose oxygens are type 2, or
you ran the script from a directory where
``input/traj/exampleTraj.lammpstrj`` does not exist. Work from the
repository root.

cages hc=0 ddc=0
~~~~~~~~~~~~~~~~

Zero is a valid answer on this dump. Confined water need not
contain an accepted hexagonal cage or double-diamond cage.

Import-style globals (``readFrameOnlyOne`` is nil)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Scripts that mention ``readFrameOnlyOne``, ``functionScript``, or
``trajectory`` as a global expect the 2020 driver. Rewrite them to
``require("dseams")``. See
`The library, not a second CLI <../explanation/library-not-cli.rst>`_.

Next Steps
----------

- `Call dseams from Fennel <../howto/fennel.rst>`_ - Lisp syntax on the same table

- `Embed in a Lua host <../howto/embed-lua.rst>`_ - Load the ``.so`` from your own process

- `Lua surface <../reference/lua.rst>`_ - Helpers and ``dseams.core`` names

- `Install the library <../howto/install.rst>`_ - How the ``.so`` is built and found

Summary
-------

You learned to:

1. ✓ Call ``require("dseams")`` from stock ``lua``

2. ✓ Read one dump frame into a ``PointCloud``

3. ✓ Label oxygens with ``dseams.chill_plus``

4. ✓ Flag cage membership with ``dseams.cages``

5. ✓ Treat the two tables as different questions

The helpers stay in ``lua/dseams.lua``. The compiled registrations
stay on ``dseams.core``.
