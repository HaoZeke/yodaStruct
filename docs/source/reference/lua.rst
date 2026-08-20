=================
require("dseams")
=================


Public table from ``require("dseams")`` (``lua/dseams.lua`` on
``dseams_core``). ``require("yoda")`` is the same table.

Fennel call shape is in `Fennel <../howto/fennel.rst>`_.
Compiled names on ``dseams.core`` are in
`Compiled registrations <lua-functions.rst>`_.

Public names
------------

.. code:: lua

    local dseams = require("dseams")
    local cloud = dseams.read("water.lammpstrj", {type = 2})
    local types = dseams.chill_plus(cloud, {cutoff = 3.5, type = 2})
    local nl = dseams.core.neighListO(3.5, cloud, 2)

.. table::

    +--------------------+----------+----------------------------------------------------+
    | name               | kind     | role                                               |
    +====================+==========+====================================================+
    | ``read``           | function | suffix-dispatching loader                          |
    +--------------------+----------+----------------------------------------------------+
    | ``neighbors``      | function | cutoff neighbour list by atom ID                   |
    +--------------------+----------+----------------------------------------------------+
    | ``neighbors_pair`` | function | I-J cutoff neighbour list                          |
    +--------------------+----------+----------------------------------------------------+
    | ``cn``             | function | site-site coordination number                      |
    +--------------------+----------+----------------------------------------------------+
    | ``rdf``            | function | partial three-dimensional RDF                      |
    +--------------------+----------+----------------------------------------------------+
    | ``running_cn``     | function | running site-site coordination number              |
    +--------------------+----------+----------------------------------------------------+
    | ``knn``            | function | k-nearest graph by atom ID                         |
    +--------------------+----------+----------------------------------------------------+
    | ``chill_plus``     | function | CHILL+ state names; no file                        |
    +--------------------+----------+----------------------------------------------------+
    | ``chill``          | function | CHILL state names; no file                         |
    +--------------------+----------+----------------------------------------------------+
    | ``cages``          | function | seeded HC/DDC per-atom flags                       |
    +--------------------+----------+----------------------------------------------------+
    | ``hbonds``         | function | hydrogen-bond adjacency table                      |
    +--------------------+----------+----------------------------------------------------+
    | ``density``        | function | Cartesian number-density profile                   |
    +--------------------+----------+----------------------------------------------------+
    | ``site_table``     | function | parse a type-to-site mapping                       |
    +--------------------+----------+----------------------------------------------------+
    | ``pairs``          | function | mutual nearest cation-anion pairs                  |
    +--------------------+----------+----------------------------------------------------+
    | ``domain``         | function | largest mapped-site domain statistics              |
    +--------------------+----------+----------------------------------------------------+
    | ``core``           | table    | ``require("dseams_core")``; compiled registrations |
    +--------------------+----------+----------------------------------------------------+

These names are the public surface. Locals in ``lua/dseams.lua``
(``suffix``, ``opts``) are not exported. Option tables may be omitted.
Defaults are those of ``lua/dseams.lua``.

The engine CLI injects compiled names as globals. Library scripts
call them on ``dseams.core``.

Calling conventions
-------------------

PointCloud userdata
~~~~~~~~~~~~~~~~~~~

Readers return a ``PointCloud`` userdata, not a table. The usertype is
registered on the Lua state when ``dseams_core`` loads, so
``PointCloud.new()`` is a global after ``require``. Workflow scripts pass
that scratch object into the legacy readers.

.. table::

    +------------------+--------+---------------------------------+
    | member           | kind   | meaning                         |
    +==================+========+=================================+
    | ``nop``          | field  | particle count                  |
    +------------------+--------+---------------------------------+
    | ``currentFrame`` | field  | frame index just read           |
    +------------------+--------+---------------------------------+
    | ``box()``        | method | box lengths, 3-number table     |
    +------------------+--------+---------------------------------+
    | ``boxLow()``     | method | box origin, 3-number table      |
    +------------------+--------+---------------------------------+
    | ``iceTypes()``   | method | per-particle state name strings |
    +------------------+--------+---------------------------------+

Ice-state strings: ``cubic``, ``hexagonal``, ``water``, ``interfacial``,
``clathrate``, ``interClathrate``, ``reCubic``, ``reHex``, ``unclassified``.

Tables vs userdata
~~~~~~~~~~~~~~~~~~

New-style compiled names take neighbour lists and ring lists **by value**. sol2 builds a C++ container from a plain Lua table of
tables. Those names return ``sol::as_nested`` or ``sol::as_table``, so
the result is a Lua table.

Legacy names take C++ container **references**. They bind only
container userdata (the object another C++ bind already put on the
stack). A freshly written Lua table is not accepted.

.. table::

    +--------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------------------+----------------------------------+
    | style  | examples                                                                                                                                                                                                                                                                                     | nList / rings in | result                           |
    +========+==============================================================================================================================================================================================================================================================================================+==================+==================================+
    | new    | ``neighListO``, ``neighListPair``, ``neighbourListByIndex``, ``kNearestNeighbourList``, ``ringNetwork``, ``cageAffiliation``, ``getCorrelPlus``, ``calcCN``, ``calcRDF3D``, ``calcRunningCN``, ``getHbondNetwork*``, ``densityByType``, ``densityByKind``, ``contactPairs``, ``domainStats`` | Lua table        | Lua table (or void / name table) |
    +--------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------------------+----------------------------------+
    | legacy | ``neighborList``, ``bondNetworkByIndex``, ``getPrimitiveRings``, ``readFrame*``, ``chillPlus_*``, ``chill_*``                                                                                                                                                                                | userdata         | userdata                         |
    +--------+----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------------------+----------------------------------+

``sol2`` does not apply C++ default arguments on a raw bind. Wrapped
names spell optionals in Lua (``sol::optional``). Legacy names need
every argument the bound C++ signature requires.

~read~(path[, opts])
--------------------

Suffix-dispatching loader. Returns a ``PointCloud``.

.. table::

    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+
    | suffix                         | backend                                 | notes                                                                                                     |
    +================================+=========================================+===========================================================================================================+
    | ``.xyz``                       | ``readXYZ``                             | whole file; ``opts.frame`` unused                                                                         |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+
    | ``.con``                       | ``readCon``                             | errors if this build has no readcon                                                                       |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+
    | ``.pdb`` / ``.gro`` / ``.dcd`` | ``readChemfiles``                       | errors if this build has no chemfiles; ``opts.type`` defaults to ``-1`` (keep all)                        |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+
    | other (LAMMPS dump)            | ``readLammpsTrj`` or ``readLammpsTrjO`` | ``opts.all = true`` keeps all atoms. Otherwise ``opts.type`` selects a type; nil tries type 2 then type 1 |
    +--------------------------------+-----------------------------------------+-----------------------------------------------------------------------------------------------------------+

``opts.frame`` is the 1-based frame index (default 1). ``opts.all = true``
calls ``core.readLammpsTrj`` and keeps every atom. Otherwise
``opts.type`` is the LAMMPS type ID to keep. There is no region in the
high-level helper.

A dump slice that shrinks ``nop`` is
``dseams.core.readLammpsTrjreduced(path, frame, type, true, lo, hi)``.
``core.readLammpsTrjO`` takes the same five arguments after the path
and only sets ``inSlice``. An axis with ``lo == hi`` is unconstrained,
so ``{0,0,0}`` / ``{50,0,0}`` is ``x`` in ``[0, 50]``, ``y`` and ``z`` open.
The ``O`` in ``readLammpsTrjO`` is historical; the type argument is any
LAMMPS type.

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
``floor(cutoff / 0.05)``). Returns ``{r = {...}, g = {...}}``.

Running coordination number
---------------------------

``running_cn(cloud[, opts])`` is the running integral of the partial
``g_IJ``. Uses the same options and
defaults as ``rdf`` and calls ``core.calcRunningCN``. Returns
``{r = {...}, cn = {...}}``, with ``rhoJ = nJ / volume``.

~knn~(cloud[, opts])
--------------------

k-nearest graph by atom ID. Calls ``core.kNearestNeighbourList``.

- ``opts.k`` defaults to 4

- ``opts.cutoff`` (candidate cutoff) defaults to 5.0

- ``opts.type`` defaults to 1

- ``opts.mutual`` defaults to true; only the boolean ``false`` selects the union graph

CHILL+
------

``chill_plus(cloud[, opts])`` builds ``neighbors(cloud, opts)``, runs
``core.getCorrelPlus(cloud, nl, false)``, returns
``core.getIceTypePlusNoPrint(cloud, nl, false)`` (1-based array of
state names). Does not write a file. Mutates ``cloud``.

~chill~(cloud[, opts])
----------------------

Same pipeline with ``core.getCorrel`` and ``core.getIceTypeNoPrint``.
Mutates ``cloud``. Does not write a file.

~cages~(cloud[, opts])
----------------------

Seeded HC/DDC membership. Builds mutual and union k-nearest graphs
(``opts.k`` 4, ``opts.cutoff`` 5.0, ``opts.type`` 1), converts each to an
index list, keeps six-membered rings, and returns
``core.seededCageAffiliation(...)``: a table ``{hc = ..., ddc = ...}``
of per-atom flags.

~hbonds~(cloud[, opts])
-----------------------

Hydrogen-bond adjacency for the selected sites. The neighbour graph
uses ``opts.cutoff`` (default 3.5) and ``opts.type`` (default 1).

- With ``opts.path``, the helper reads hydrogens from that trajectory;
  ``opts.frame`` defaults to 1 and ``opts.h_type`` defaults to 1.

- With ``opts.h_cloud``, the helper uses the supplied hydrogen
  ``PointCloud`` instead.

- ``opts.dist`` and ``opts.angle`` use the engine defaults 2.42 and 30.0
  when omitted.

Returns a nested Lua table. Supplying neither ``path`` nor ``h_cloud`` is
an error.

~density~(cloud[, opts])
------------------------

Cartesian number density along ``opts.axis``: ``"x"``, ``"y"``, ``"z"``,
or the corresponding zero-based index. The default axis is ``"z"``.
``opts.bins`` defaults to the axis span divided into approximately 0.1
length-unit bins.

- Type mode uses ``opts.type`` (default 0) and returns
  ``{centres, rho, axis, atom_type}``.

- Site mode requires both ``opts.table`` and ``opts.kind`` and returns
  ``{centres, rho, axis, site_kind}``.

Site mapping table
------------------

``site_table(spec)`` parses a comma-separated mapping such as
``"1=cationHead,2=anion,3=tail"``. The result is a ``SiteTable`` userdata
accepted by ``density``, ``pairs``, and ``domain``. Site kinds are exposed
on ``dseams.core.Kind`` (alias ``SiteKind``), including ``polar`` and
``apolar``.

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

Signatures for every compiled name are in
`Compiled registrations <lua-functions.rst>`_.
