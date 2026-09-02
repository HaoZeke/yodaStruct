=================
require("dseams")
=================


This page names the public ``require("dseams")`` table
(``lua/dseams.lua`` on ``dseams_core``). ``require("yoda")`` resolves to
the same table.

Fennel call shape lives in `Fennel <../howto/fennel.rst>`_.
Compiled names on ``dseams.core`` live in
`Compiled registrations <lua-functions.rst>`_.

The ice-type helpers wrap the Moore ice classifier (CHILL) and the
Nguyen plus star (``chill_plus``). Cage flags are hexagonal cage
(HC) and double-diamond cage (DDC). ``rdf`` and ``running_cn`` wrap
the partial radial distribution function (RDF).

Public names
------------

The required table exposes these helpers.

.. code:: lua

    local dseams = require("dseams")
    local cloud = dseams.read("water.lammpstrj", {type = 2})

``read`` loads a file by suffix. ``neighbors`` and ``neighbors_pair``
build cutoff neighbour lists. ``cn``, ``rdf``, and ``running_cn`` wrap
site-site coordination and the partial RDF. ``knn`` builds a
k-nearest graph. ``chill_plus`` returns Nguyen plus state names.
``chill`` returns Moore classifier labels. Neither writes a file.
``cages`` returns seeded HC and DDC flags. ``hbonds`` returns a
hydrogen-bond adjacency table. ``density``, ``site_table``, ``pairs``,
and ``domain`` cover number density and mapped-site statistics.
``core`` is ``require("dseams_core")``.

These names form the public surface. Locals in ``lua/dseams.lua``
(``suffix``, ``opts``) stay private. Option tables may be omitted.
Defaults match ``lua/dseams.lua``.

The engine CLI injects compiled names as globals. Library scripts
call those names on ``dseams.core``.

Calling conventions
-------------------

PointCloud userdata
~~~~~~~~~~~~~~~~~~~

Readers return a ``PointCloud`` userdata. The usertype is registered
on the Lua state when ``dseams_core`` loads, so ``PointCloud.new()`` is
a global after ``require``. Workflow scripts pass that scratch object
into the legacy readers.

That userdata exposes ``nop`` (particle count), ``currentFrame``
(frame index just read), ``box()`` and ``boxLow()`` (3-number
tables), and ``iceTypes()`` (per-particle state name strings).
iceTypes strings: ``cubic``, ``hexagonal``, ``water``, ``interfacial``,
``clathrate``, ``interClathrate``, ``reCubic``, ``reHex``,
``unclassified``.

Tables vs userdata
~~~~~~~~~~~~~~~~~~

New-style compiled names take neighbour lists and ring lists **by value**. sol2 builds a C++ container from a plain Lua table of
tables. Those names return a Lua table through ``sol::as_nested`` or
``sol::as_table``.

Nested-table getters (``bondNetworkByIndex``, ``getPrimitiveRings``,
``prismAnalysis``, ``bulkRingNumberAnalysis``) take the same by-value
containers. A Lua table from ``neighbors``, ``neighListO``, or
``getHbondNetwork`` binds. A container userdata from ``neighborList``
binds too. The getter copies the rows; it does not borrow the Lua
value.

``bondNetworkByIndex`` returns a Lua table (``sol::as_nested``).
``getPrimitiveRings`` returns the C++ ring list as container
userdata. ``prismAnalysis`` and ``bulkRingNumberAnalysis`` write files
and return an integer status.

Readers and ice-type writers (``readFrame*``, ``chillPlus_*``, ``chill_*``)
take C++ container **references**. They bind container userdata (the
object another C++ bind already put on the stack). A freshly
written Lua table fails to bind there.

``neighborList`` constructs neighbour-list userdata. That name
returns a C++ container.

New-style names (``neighListO``, ``ringNetwork``, ``cageAffiliation``,
``calcCN``) take a Lua table and return a Lua table. Nested-table
getters (``bondNetworkByIndex``, ``getPrimitiveRings``,
``prismAnalysis``, ``bulkRingNumberAnalysis``) take a Lua table or
userdata and return a table, userdata, or int. Legacy names
(``neighborList``, ``readFrame*``, ``chillPlus_*``, ``chill_*``) take
userdata and return userdata.

``sol2`` does not apply C++ default arguments on a raw bind. Wrapped
names spell optionals in Lua (``sol::optional``). Readers and ice-type
writers need every argument the bound C++ signature requires.

``example_lua/library/legacy_chain.lua`` is the worked example of
this convention. The last section of this page names the calls.

~read~(path[, opts])
--------------------

Suffix-dispatching loader. Returns a ``PointCloud``.

.. table::

    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------+
    | suffix                         | backend                                 | notes                                                                                               |
    +================================+=========================================+=====================================================================================================+
    | ``.xyz``                       | ``readXYZ``                             | reads the file as one frame; ``opts.frame`` unused                                                  |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------+
    | ``.con``                       | ``readCon``                             | fails without readcon in this build                                                                 |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------+
    | ``.pdb`` / ``.gro`` / ``.dcd`` | ``readChemfiles``                       | needs chemfiles; ``opts.type`` defaults to ``-1`` (keep every atom)                                 |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------+
    | other (LAMMPS dump)            | ``readLammpsTrj`` or ``readLammpsTrjO`` | ``opts.all`` keeps every atom. Otherwise ``opts.type`` selects a type; nil tries type 2 then type 1 |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------+

``opts.frame`` is the 1-based frame index (default 1). ``opts.all``
calls ``core.readLammpsTrj`` and keeps every atom. Otherwise
``opts.type`` is the LAMMPS type ID to keep. This helper has no
region filter.

A dump slice that shrinks ``nop`` is this call:

..
   #skip_lint_start

.. code:: lua

    dseams.core.readLammpsTrjreduced(path, frame, type, true, lo, hi)

..
   #skip_lint_end

``core.readLammpsTrjO`` takes the same five arguments after the path
and sets ``inSlice``. A coordinate with ``lo == hi`` is unconstrained,
so ``{0,0,0}`` / ``{50,0,0}`` is ``x`` in ``[0, 50]``, ``y`` and ``z`` open.
The ``O`` in ``readLammpsTrjO`` is a spelling leftover; the type
argument is any LAMMPS type.

~neighbors~(cloud[, opts])
--------------------------

Cutoff neighbour list by atom ID. Calls
``core.neighListO(opts.cutoff or 3.5, cloud, opts.type or 1)``.
Returns a Lua table of rows (self ID first).

Pair neighbours
---------------

``neighbors_pair(cloud[, opts])`` returns an I-J cutoff neighbour list. Calls
``core.neighListPair(opts.cutoff or 3.5, cloud, opts.type_i or 1, opts.type_j or 2)``. Like-type pairs reuse ``neighListO``.

~cn~(cloud[, opts])
-------------------

Site-site coordination number. Calls ``core.calcCN`` with
``opts.type_i`` (default 1), ``opts.type_j`` (default 2),
``opts.cutoff`` (default 4.5), and ``opts.bins`` (default
``floor(cutoff / 0.1)``). ``rhoJ`` is ``nJ / volume`` from the
partial RDF.

~rdf~(cloud[, opts])
--------------------

Partial three-dimensional radial distribution function. Calls
``core.calcRDF3D`` with ``opts.type_i`` (default 1), ``opts.type_j``
(default 2), ``opts.cutoff`` (default 12.0), and ``opts.bins`` (default
``floor(cutoff / 0.05)``). Returns a table with keys ``r`` and ``g``.

Running coordination number
---------------------------

``running_cn(cloud[, opts])`` is the running integral of the partial
``g_IJ``. Uses the same options and
defaults as ``rdf`` and calls ``core.calcRunningCN``. Returns a table
with keys ``r`` and ``cn``, with ``rhoJ = nJ / volume``.

~knn~(cloud[, opts])
--------------------

k-nearest graph by atom ID. Calls ``core.kNearestNeighbourList``.
``opts.k`` defaults to 4. ``opts.cutoff`` defaults to 5.0.
``opts.type`` defaults to 1. ``opts.mutual`` selects the mutual graph.
A Lua off-value selects the union graph.

``chill_plus``
--------------

``chill_plus(cloud[, opts])`` builds ``neighbors(cloud, opts)``.
The Nguyen plus pair is:

..
   #skip_lint_start

.. code:: lua

    core.getCorrelPlus(cloud, nl, false)
    return core.getIceTypePlusNoPrint(cloud, nl, false)

..
   #skip_lint_end

The return value is a 1-based array of state names. Writes no
file. Mutates ``cloud``.

~chill~(cloud[, opts])
----------------------

Same pipeline with ``core.getCorrel`` and ``core.getIceTypeNoPrint``.
Mutates ``cloud``. Writes no file.

~cages~(cloud[, opts])
----------------------

Seeded HC and DDC membership. Builds mutual and union k-nearest
graphs (``opts.k`` 4, ``opts.cutoff`` 5.0, ``opts.type`` 1), converts
each to an index list, keeps six-membered rings, and returns
per-atom HC and DDC flags.

The compiled name is:

.. code:: lua

    core.seededCageAffiliation(strictRings, strictNList,
                               permRings, permNList[, flag])

The first four arguments are six-membered rings and
index-neighbour lists on the mutual graph, then the same pair on
the union graph. They are by-value tables. The return value is a
table with keys ``hc`` and ``ddc``.

The fifth argument is optional. That flag is the engine's
ring-adjacent fill (``ringAdjacentCompletion``). Omit it to keep
the seeded labels. Pass the on-value and the accepted HC and DDC
flags run through the all-but-one walk: a six-ring whose vertices
all carry that cage label but one gains the label on the vacant
vertex, and the walk repeats to a fixed point. The helper reads
the option as:

..
   #skip_lint_start

.. code:: lua

    core.seededCageAffiliation(six_s, idx_s, six_u, idx_u,
                               o.complete or false)

..
   #skip_lint_end

~hbonds~(cloud[, opts])
-----------------------

Hydrogen-bond adjacency for the selected sites. The neighbour graph
uses ``opts.cutoff`` (default 3.5) and ``opts.type`` (default 1).

- With ``opts.path``, the helper reads hydrogen atoms from that
  trajectory; ``opts.frame`` defaults to 1 and ``opts.h_type``
  defaults to 1.

- With ``opts.h_cloud``, the helper uses the supplied hydrogen
  ``PointCloud`` instead.

- ``opts.dist`` and ``opts.angle`` use the engine defaults 2.42 and 30.0
  when omitted.

Returns a nested Lua table. Supplying neither ``path`` nor ``h_cloud``
raises an error.

~density~(cloud[, opts])
------------------------

Cartesian number density along one box coordinate. The option
table selects the coordinate:

..
   #skip_lint_start

.. code:: lua

    dseams.density(cloud, {axis = "z", bins = n})

..
   #skip_lint_end

``"x"``, ``"y"``, ``"z"``, or the matching zero-based index. The default
is ``"z"``. ``opts.bins`` defaults to that box length divided into
about 0.1 length-unit bins.

Type mode uses ``opts.type`` (default 0). Site mode requires both
``opts.table`` and ``opts.kind``. Each mode returns centres, density,
the chosen coordinate, and the type or site kind.

Site mapping table
------------------

``site_table(spec)`` parses a comma-separated mapping such as
``"1=cationHead,2=anion,3=tail"``. That call returns a ``SiteTable``
userdata accepted by ``density``, ``pairs``, and ``domain``. Site kinds
live on ``dseams.core.Kind`` (alias ``SiteKind``), including ``polar``
and ``apolar``.

~pairs~(cloud, opts)
--------------------

Requires ``opts.table``. Maps the cloud to ionic sites and returns
mutual nearest unlike pairs as
``{pairs, count, n_cation, n_anion}``. Each pair contains the original
atom IDs.

~domain~(cloud, opts)
---------------------

Requires ``opts.table`` and ``opts.kind``. The graph joins mapped sites
within ``opts.cutoff`` (default 3.5). Returns
``{site_kind, n, largest, percolation}``, where ``n`` is the selected site
count and ``percolation = largest / n``.

``core``
--------

The table returned by ``require("dseams_core")``. Every name from
``luaApi::registerAll`` (``src/lua_api.cpp``) lives here.

Usertypes (``PointCloud``, ``RingUpdater``, ``AffiliationUpdater``) are
registered on the Lua state, not on this table.
``RingUpdater.new(6)`` works after ``require``.

Signatures for every compiled name live in
`Compiled registrations <lua-functions.rst>`_.

Worked example: the legacy ring chain
-------------------------------------

``example_lua/library/legacy_chain.lua`` is the worked example of the
by-value getters. The script reads the water dump in
``input/traj/exampleTraj.lammpstrj``. The script constructs a
neighbour table with ``dseams.neighbors`` and feeds that table through
``getHbondNetwork``, ``bondNetworkByIndex``, ``getPrimitiveRings``, and
``prismAnalysis``. Those names accept the Lua table the helpers
return. They accept the container userdata from ``neighborList``
too. The table path and the userdata path must agree on the ring
count. The script then writes a prism histogram under a temporary
directory.

From the repository root, with ``LUA_PATH`` and ``LUA_CPATH`` set as in
the `Quickstart <../quickstart.rst>`_:

.. code:: bash

    lua example_lua/library/legacy_chain.lua

The script prints ``legacy_chain rings=N`` on success. The chain
itself is:

.. code:: lua

    local nList = dseams.neighbors(cloud, {cutoff = 3.5, type = 2})
    local hbn = core.getHbondNetwork(path, cloud, nList, 1, 1)
    local byIndex = core.bondNetworkByIndex(cloud, hbn)
    local rings = core.getPrimitiveRings(byIndex, 6)

    local userdata_nlist = core.neighborList(3.5, cloud, 2)
    local rings_ud = core.getPrimitiveRings(
      core.bondNetworkByIndex(cloud, userdata_nlist), 6)
