=================
require("dseams")
=================


``require("dseams")`` loads ``lua/dseams.lua`` on ``dseams_core``.
``require("yoda")`` loads the same table.

Cage flags are hexagonal cage (HC) and double-diamond cage (DDC).
The chill bond-order classifier (CHILL) and CHILL+ return state
names. Pair correlation here means a radial distribution function (RDF).

Fennel call shape lives in `Fennel <../howto/fennel.rst>`_.
Compiled names on ``dseams.core`` live in
`Compiled registrations <lua-functions.rst>`_.

Public names
------------

.. code:: lua

    local dseams = require("dseams")
    local cloud = dseams.read("water.lammpstrj", {type = 2})
    local types = dseams.chill_plus(cloud, {cutoff = 3.5, type = 2})
    local nl = dseams.core.neighListO(3.5, cloud, 2)

``read``
    suffix-dispatching loader.

``neighbors``
    cutoff neighbour list by atom ID.

``neighbors_pair``
    I-J cutoff neighbour list.

``cn``
    site-site coordination number.

``rdf``
    partial three-dimensional RDF.

``running_cn``
    running integral of that pair correlation.

``knn``
    k-nearest graph by atom ID.

``chill_plus``
    CHILL+ state names, no output file.

``chill``
    CHILL labels, no dump on disk.

``cages``
    seeded HC/DDC per-atom flags.

``hbonds``
    hydrogen-bond adjacency table.

``density``
    Cartesian number-density profile.

``site_table``
    parse a type-to-site mapping.

``pairs``
    mutual nearest cation-anion pairs.

``domain``
    largest mapped-site domain statistics.

``core``
    ``require("dseams_core")``; compiled registrations.

These names form the public surface. Locals in ``lua/dseams.lua``
(``suffix``, ``opts``) stay private. Callers may drop the option
table. Defaults match ``lua/dseams.lua``.

The engine CLI injects compiled names as globals. Library scripts
call those names on ``dseams.core``.

Calling conventions
-------------------

PointCloud userdata
~~~~~~~~~~~~~~~~~~~

Readers return a ``PointCloud`` userdata. Loading ``dseams_core``
registers the usertype, so ``PointCloud.new()`` is a global after
``require``. Workflow scripts pass that scratch object into the
legacy readers.
PointCloud userdata fields:

``nop``
    particle count.

``currentFrame``
    frame index just read.

``box()``
    box lengths, 3-number table.

``boxLow()``
    box origin, 3-number table.

``iceTypes()``
    per-particle state name strings.

``iceTypes()`` yields one of ``cubic``, ``hexagonal``, ``water``,
``interfacial``, ``clathrate``, ``interClathrate``, ``reCubic``, ``reHex``,
``unclassified``.

Tables vs userdata
~~~~~~~~~~~~~~~~~~

Compiled names that take a neighbour list or a ring list take that
container **by value**. sol2 builds a C++ ``vector<vector<int>>`` from
either a nested Lua table or a container userdata already on the
stack.

``sol::as_nested`` and ``sol::as_table`` wrappers hand back a Lua
table. A raw bind of a C++ vector hands back container userdata.
Both shapes accept ``#`` and ``ipairs``.

A C++ reference parameter accepts userdata and treats a fresh
Lua table as garbage. The nested-table getters below take the
list by value, so both shapes bind.

``bondNetworkByIndex``
    neighbour list in; Lua table out.

``getPrimitiveRings``
    neighbour list in; container userdata out.

``prismAnalysis``
    rings plus nList in; integer out.

``bulkRingNumberAnalysis``
    ring list plus nList in; writes a histogram.

A Lua table from ``dseams.neighbors`` or ``getHbondNetwork`` feeds
those names directly. ``neighborList`` userdata binds the same way.

New-style names (``neighListO``, ``ringNetwork``,
``seededCageAffiliation``, ``getHbondNetwork*``, and the rest on
``dseams.core``) take a table or userdata and hand back a Lua table.
``readFrame*``, ``chillPlus_*``, and ``chill_*`` stay userdata in and
userdata out.

``neighborList`` is the spelling of ``neighListO`` that returns
container userdata. That spelling takes a cutoff, a ``PointCloud``,
and a type. Those compiled names skip C++ default arguments on a
raw bind. Wrapped names spell optionals in Lua (``sol::optional``).
Names without a Lua wrapper need every argument the bound C++
signature requires.

~read~(path[, opts])
--------------------

Suffix-dispatching loader. Returns a ``PointCloud``.

..
   # proseguard:off proselint.Uncomparables

.. table::

    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+
    | suffix                         | backend                                 | notes                                                                                                     |
    +================================+=========================================+===========================================================================================================+
    | ``.xyz``                       | ``readXYZ``                             | the file; ``opts.frame`` unused                                                                           |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+
    | ``.con``                       | ``readCon``                             | missing unless this tree compiled readcon                                                                 |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+
    | ``.pdb`` / ``.gro`` / ``.dcd`` | ``readChemfiles``                       | absent without chemfiles; ``opts.type`` defaults to ``-1`` (keep all)                                     |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+
    | other (LAMMPS dump)            | ``readLammpsTrj`` or ``readLammpsTrjO`` | ``opts.all = true`` keeps all atoms. Otherwise ``opts.type`` selects a type; nil tries type 2 then type 1 |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+

``opts.frame`` is the 1-based frame index (default 1). ``opts.all = true``
calls ``core.readLammpsTrj`` and keeps every atom. Otherwise
``opts.type`` is the LAMMPS type ID to keep. The helper has no
region filter.

A dump slice that shrinks ``nop`` is
``dseams.core.readLammpsTrjreduced(path, frame, type, true, lo, hi)``.
``core.readLammpsTrjO`` takes the same five arguments after the path
and sets ``inSlice``. ``lo == hi`` leaves that direction open.
``{0,0,0}`` / ``{50,0,0}`` is ``x`` in ``[0, 50]``, with ``y`` and ``z`` open.
The type argument is any LAMMPS type (the ``O`` is historical).

..
   # proseguard:on proselint.Uncomparables

~neighbors~(cloud[, opts])
--------------------------

Cutoff neighbour list by atom ID. Calls
``core.neighListO(opts.cutoff or 3.5, cloud, opts.type or 1)``.
Hands back a Lua table of rows (self ID first).

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
``floor(cutoff / 0.05)``). Hands back ``{r = r, g = g}``.

Running coordination number
---------------------------

``running_cn(cloud[, opts])`` integrates the partial ``g_IJ``.
Shared options and defaults match ``rdf``. The call is
``core.calcRunningCN``. Hands back ``{r = r, cn = cn}``, with
``rhoJ = nJ / volume``.

~knn~(cloud[, opts])
--------------------

..
   # proseguard:off proselint.Uncomparables

k-nearest graph by atom ID. Calls ``core.kNearestNeighbourList``.

- ``opts.k`` defaults to 4

- ``opts.cutoff`` (candidate cutoff) defaults to 5.0

- ``opts.type`` defaults to 1

- ``opts.mutual`` defaults to ``true``; the boolean ``false`` selects the union graph

CHILL+
------

``chill_plus(cloud[, opts])`` builds ``neighbors(cloud, opts)``, runs
``core.getCorrelPlus(cloud, nl, false)``, and returns
``core.getIceTypePlusNoPrint(cloud, nl, false)`` (1-based array of
state names). Writes no file. Mutates ``cloud``.

..
   # proseguard:on proselint.Uncomparables

~chill~(cloud[, opts])
----------------------

Same pipeline with ``core.getCorrel`` and ``core.getIceTypeNoPrint``.
Mutates ``cloud``. Writes no file.

~cages~(cloud[, opts])
----------------------

..
   # proseguard:off proselint.Uncomparables

Seeded HC/DDC membership. Builds mutual and union k-nearest graphs
(``opts.k`` 4, ``opts.cutoff`` 5.0, ``opts.type`` 1), converts each to an
index list, keeps six-membered rings, and calls
``core.seededCageAffiliation``. Hands back a table ``{hc = hc, ddc = ddc}``
of per-atom flags.

``opts.complete`` is the fifth argument of
``seededCageAffiliation``. The default is ``false``. ``true`` runs the
engine ring-adjacent walk: the last vertex of a six-ring whose
other vertices already carry an HC or DDC label.

.. code:: lua

    local aff = dseams.cages(cloud, {type = 2})
    local filled = dseams.cages(cloud, {type = 2, complete = true})

~seededCageAffiliation~(strictRings, strictNList, permRings, permNList[, flag])
-------------------------------------------------------------------------------

Compiled name on ``dseams.core``. Mutual-graph seeds, permissive-graph
fill. Hands back ``{hc = hc, ddc = ddc}``, each a 1-based array of
per-atom flags of length ``cloud.nop``.

Those per-atom flags take four ring and neighbour arguments by
value from the seeded call. A nested Lua table and a container
userdata both bind.

The fifth argument is ``ringAdjacentCompletion``, a boolean that
defaults to ``false`` when the call drops it. Pass ``true`` and the
accepted labels go through ``ring::ringAdjacentCompletion`` on the
permissive six-rings. HC flags and DDC flags run as two walks. The
walk is the all-but-one rule: a six-ring whose vertices all carry
that cage label but one fills the last vertex, and the walk
repeats until a fixed point.

.. code:: lua

    local aff = core.seededCageAffiliation(six_s, idx_s, six_u, idx_u)
    local filled = core.seededCageAffiliation(six_s, idx_s, six_u, idx_u, true)

``dseams.cages`` passes ``opts.complete or false`` as that fifth
argument. ``example_lua/library/topology.lua`` calls the compiled
name with ``true``.

..
   # proseguard:on proselint.Uncomparables

Worked example: the legacy ring chain
-------------------------------------

``legacy_chain.lua`` under ``example_lua/library/`` is the
worked example for the by-value getters. The script reads
``input/traj/exampleTraj.lammpstrj`` (oxygen type 2). A neighbour
table comes from ``dseams.neighbors``, then feeds
``getHbondNetwork``, ``bondNetworkByIndex``, ``getPrimitiveRings``, and
``prismAnalysis``. ``neighborList`` userdata binds those names too.
Both paths agree on the ring count. ``prismAnalysis`` writes
``topoINT/nPrisms.dat``.

..
   # proseguard:off proselint.Uncomparables

.. code:: lua

    local nList = dseams.neighbors(cloud, {cutoff = 3.5, type = 2})
    local hbn = core.getHbondNetwork(path, cloud, nList, 1, 1)
    local byIndex = core.bondNetworkByIndex(cloud, hbn)
    local rings = core.getPrimitiveRings(byIndex, 6)
    core.prismAnalysis(out .. "/", rings, byIndex, cloud, 6, 1, 1, 1, false)

..
   # proseguard:on proselint.Uncomparables

The meson test name is ``dseams_legacy_chain``. From a configured
build directory:

.. code:: bash

    meson test -C bbdir dseams_legacy_chain

~hbonds~(cloud[, opts])
-----------------------

Hydrogen-bond adjacency for the selected sites. The neighbour graph
uses ``opts.cutoff`` (default 3.5) and ``opts.type`` (default 1).

- With ``opts.path``, the helper reads hydrogens from that trajectory;
  ``opts.frame`` defaults to 1 and ``opts.h_type`` defaults to 1.

- With ``opts.h_cloud``, the helper uses the supplied hydrogen
  ``PointCloud`` instead.

- ``opts.dist`` and ``opts.angle`` use the engine defaults 2.42 and 30.0
  when dropped.

Hands back a nested Lua table. Supplying neither ``path`` nor
``h_cloud`` is an error.

~density~(cloud[, opts])
------------------------

..
   # proseguard:off rgoswami.ReviewRegister

Cartesian number density along ``opts.axis``: ``"x"``, ``"y"``, ``"z"``,
or the matching zero-based index. The default direction is ``"z"``.
``opts.bins`` defaults to that span divided into about 0.1
length-unit bins.

- Type mode uses ``opts.type`` (default 0) and returns
  ``{centres, rho, axis, atom_type}``.

- Site mode requires both ``opts.table`` and ``opts.kind`` and returns
  ``{centres, rho, axis, site_kind}``.

..
   # proseguard:on rgoswami.ReviewRegister

Site mapping table
------------------

``site_table(spec)`` parses a comma-separated mapping such as
``"1=cationHead,2=anion,3=tail"``. That call yields a ``SiteTable``
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
within ``opts.cutoff`` (default 3.5). Hands back
``{site_kind, n, largest, percolation}``, where ``n`` is the selected site
count and ``percolation = largest / n``.

``core``
--------

``require("dseams_core")`` returns this table. Every name from
``luaApi::registerAll`` (``src/lua_api.cpp``) lives here.

Usertypes (``PointCloud``, ``RingUpdater``, ``AffiliationUpdater``) live
on the Lua state, off this table. ``RingUpdater.new(6)`` works after
``require``.

Signatures for every compiled name live in
`Compiled registrations <lua-functions.rst>`_.
