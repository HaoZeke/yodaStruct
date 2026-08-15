Compiled registrations
======================

Appendix: every name from ``luaApi::registerAll`` (``src/lua_api.cpp``).
This is the compiled-registration list that ``docs/luaFunctions.md``
also carries. The public ``require("dseams")`` helpers are in
:doc:`lua`.

Names below are on ``dseams.core``, or globals under the engine CLI.
Optional backends are listed only when the matching ``#ifdef`` is on.
Usertypes are registered on the Lua state, not on ``dseams.core``.

Fennel call shape is in :doc:`../howto/fennel`.

Name index
----------

58 functions (56 always, plus ``readChemfiles`` and ``readCon`` when
linked) and 3 usertypes.

I/O
~~~

``readLammpsTrjO``, ``readLammpsTrjreduced``, ``readXYZ``, ``readChemfiles``,
``readCon``, ``readFrame``, ``readFrameOnlyOne``, ``readFrameOnlyOneAllAtoms``,
``writeDump``, ``writeHistogram``, ``PointCloud``.

Neighbours
~~~~~~~~~~

``neighListO``, ``kNearestNeighbourList``, ``shellSeparation``,
``neighbourListByIndex``, ``getNewNeighbourListByIndex``,
``getHbondNetwork``, ``getHbondNetworkFromClouds``, ``neighborList``,
``bondNetworkByIndex``.

CHILL
~~~~~

``classifyBonds``, ``registerBondClassifier``, ``bondClassifierNames``,
``getCorrelPlus``, ``getIceTypePlus``, ``getIceTypePlusNoPrint``,
``getCorrel``, ``getIceType``, ``getIceTypeNoPrint``, ``steinhardtQl``,
``steinhardtQlVoronoi``, ``voronoiFacetWeights``, ``chillPlus_cij``,
``chillPlus_iceType``, ``chill_cij``, ``chill_iceType``, ``averageQ6``,
``modifyChill``, ``percentage_Ice``.

Rings and cages
~~~~~~~~~~~~~~~

``ringNetwork``, ``RingUpdater``, ``getPrimitiveRings``, ``cageAffiliation``,
``AffiliationUpdater``, ``seededCageAffiliation``.

Descriptors
~~~~~~~~~~~

``classifyTemplates``, ``soapSpectrum``, ``soapSpectrumAll``,
``voronoiFeatures``.

Prism and topology
~~~~~~~~~~~~~~~~~~

``prismAnalysis``, ``ringAnalysis``, ``calcRDF``, ``clusterAnalysis``,
``recenterCluster``, ``getPointCloudAtomsOfOneAtomType``,
``selectInSingleSlice``, ``selectEdgeAtomsInRingsWithinSlice``,
``selectAtomsInSliceWithRingEdgeAtoms``, ``bulkRingNumberAnalysis``,
``bulkTopologicalNetworkCriterion``, ``bulkTopoUnitMatching``.

I/O
---

New-style readers allocate a scratch cloud and return it. They do
not take an inout ``resCloud``.

``readLammpsTrjO`` (filename, targetFrame, typeO[, isSlice, low, high])
   One LAMMPS dump frame, one atom type. ``isSlice`` defaults to
   false; ``low`` / ``high`` default to ``{0,0,0}``.

``readLammpsTrjreduced`` (filename, targetFrame, typeI[, isSlice, low, high])
   Same, dropping atoms outside the slice when ``isSlice`` is true.

``readXYZ`` (filename)
   XYZ coordinates. Whole file.

``readChemfiles`` (filename, targetFrame[, typeFilter])
   Chemfiles formats. Registered only with ``SEAMS_HAS_CHEMFILES``.
   ``typeFilter`` defaults to ``-1``.

``readCon`` (filename, targetFrame)
   eOn ``.con``. Registered only with ``SEAMS_HAS_READCON``.

``writeDump`` (yCloud, path, outFile)
   Write a LAMMPS dump. Returns an int status.

``writeHistogram`` (yCloud, nList, avgQ6)
   Write ``cij`` / ``q6`` / ``q3`` columns. Returns an int status.

Legacy readers take an inout ``PointCloud`` userdata and return it.
``sol2`` does not fill C++ defaults here; pass the slice arguments.

``readFrame`` (filename, targetFrame, resCloud, typeO, isSlice, low, high)
   ``sinp::readLammpsTrjO``.

``readFrameOnlyOne`` (filename, targetFrame, resCloud, typeI, isSlice, low, high)
   ``sinp::readLammpsTrjreduced``.

``readFrameOnlyOneAllAtoms`` (filename, targetFrame, resCloud, isSlice, low, high)
   ``sinp::readLammpsTrj`` (every type).

PointCloud
~~~~~~~~~~

Usertype. Fields ``nop``, ``currentFrame``. Methods ``box()``, ``boxLow()``,
``iceTypes()``. ``PointCloud.new()`` builds an empty cloud.

Neighbours
----------

New-style lists return nested Lua tables. Rows are by atom ID unless
the name says "ByIndex". ID rows lead with the self ID.

``neighListO`` (rcutoff, yCloud, typeI)
   Distance cutoff, one type, by atom ID.

``kNearestNeighbourList`` (yCloud, k, candidateCutoff, typeI[, mutual])
   k-nearest graph. ``mutual`` defaults to true (each nominates the
   other). ``false`` is the union graph. ``candidateCutoff`` must exceed
   the k-th neighbour distance.

``shellSeparation`` (yCloud, k, typeI)
   Two numbers: ``max d_k``, ``min d_{k+1}``. The cutoff and k-nearest
   graphs coincide when ``max d_k <= rcut <= min d_{k+1}``.

``neighbourListByIndex`` (yCloud, nList)
   ID list to index list. ``nList`` may be a Lua table.

``getNewNeighbourListByIndex`` (yCloud, cutoff)
   Index list built directly from the cloud.

``getHbondNetwork`` (filename, yCloud, nList, targetFrame, Htype[, dist, angle])
   Hydrogen-bond graph from a trajectory. ``dist`` defaults to 2.42,
   ``angle`` to 30.0 (water). Accepts a Lua-table ``nList``; returns
   userdata.

``getHbondNetworkFromClouds`` (yCloud, hCloud, nList[, dist, angle])
   Same criterion, H atoms from ``hCloud``. Same return convention.

Legacy (userdata in and out):

``neighborList`` (rcutoff, yCloud, typeI)
   ``nneigh::neighListO``.

``bondNetworkByIndex`` (yCloud, nList)
   ``nneigh::neighbourListByIndex``.

CHILL
-----

New-style classifiers mutate the cloud. Name-returning calls also
return a 1-based array of ice-state strings.

``classifyBonds`` (yCloud, nList, rule[, isSlice])
   ``rule`` is ``"CHILL"``, ``"CHILL+"``, or a table
   ``{staggeredMax, eclipsedMin, eclipsedMax, coordinationNumber}``.
   ``isSlice`` defaults to false.

``registerBondClassifier`` (name, ruleTable)
   Store or replace a named rule.

``bondClassifierNames`` ()
   Array of registered names.

``getCorrelPlus`` (yCloud, nList[, isSlice, coordinationNumber])
   CHILL+ ``c_ij``. ``isSlice`` defaults to false;
   ``coordinationNumber`` defaults to 4 (non-positive keeps each row).

``getIceTypePlus`` (yCloud, nList, path, firstFrame[, isSlice, outputFileName])
   Classify and write ``path/bop/<outputFileName>``. File name
   defaults to ``chillPlus.txt``. Returns state names.

``getIceTypePlusNoPrint`` (yCloud, nList[, isSlice])
   Classify only. Returns state names.

``getCorrel`` (yCloud, nList[, isSlice, coordinationNumber])
   CHILL ``c_ij``. Same defaults as ``getCorrelPlus``.

``getIceType`` (yCloud, nList, path, firstFrame[, isSlice, outputFileName])
   Classify and write. File name defaults to ``chill.txt``.

``getIceTypeNoPrint`` (yCloud, nList[, isSlice])
   Classify only.

``steinhardtQl`` (yCloud, nList, orderL)
   ``{ql = ..., qlBar = ...}`` arrays. ``orderL`` is 3, 4, or 6.

``steinhardtQlVoronoi`` (yCloud, candidateCutoff, orderL)
   Same table, Voronoi facet-area weights. ``orderL`` is 3, 4, 6, or 8.

``voronoiFacetWeights`` (yCloud, candidateCutoff)
   1-based array of ``{neighbours, weights, certified}`` per atom.
   ``candidateCutoff`` must exceed the largest facet-neighbour distance.

Legacy (userdata ``nList``, return the cloud or a C++ vector):

``chillPlus_cij`` (yCloud, nList, isSlice)
   ``getCorrelPlus``; returns ``yCloud``.

``chillPlus_iceType`` (yCloud, nList, path, firstFrame, isSlice, outName)
   ``getIceTypePlus``; returns ``yCloud``.

``chill_cij`` (yCloud, nList, isSlice)
   ``getCorrel``; returns ``yCloud``.

``chill_iceType`` (yCloud, nList, path, firstFrame, isSlice, outName)
   ``getIceType``; returns ``yCloud``.

``averageQ6`` (yCloud, nList, isSlice)
   Per-atom averaged ``q6`` vector.

``modifyChill`` (yCloud, q6)
   Reclassify water from ``q6``; returns ``yCloud``.

``percentage_Ice`` (yCloud, path, firstFrame, isSlice, outputFileName)
   Write ice-type percentages.

Rings
-----

Index neighbour lists (leading self entry). New-style ``nList`` may be
a Lua table.

``ringNetwork`` (nList, maxDepth)
   Primitive rings of every size up to ``maxDepth``, each a table of
   atom indices. Returns a Lua table of rings.

``RingUpdater``
   Usertype. ``RingUpdater.new(maxDepth)``; ``updater:update(nList)``
   returns the same ring table as ``ringNetwork``. After a repeat call
   on an unchanged graph, ``updater:lastRecomputedSources()`` is 0.
   ``updater:lastBallsRefreshed()`` is the ball-refresh count.

``getPrimitiveRings`` (nList, maxDepth)
   Legacy spelling of ``primitive::ringNetwork`` (userdata).

Cages
-----

Six-membered rings, neighbour lists by index.

``cageAffiliation`` (rings, nList)
   Per-ring flags ``{hc = ..., ddc = ...}``, same length as ``rings``.

``AffiliationUpdater``
   Usertype. ``AffiliationUpdater.new()``;
   ``updater:update(rings, nList)`` returns the same table as
   ``cageAffiliation``. ``updater:lastReclassified()`` is the dirty
   closure (every ring on the first call).

``seededCageAffiliation`` (strictRings, strictNList, permRings, permNList)
   Per-atom flags ``{hc = ..., ddc = ...}``. Mutual-graph seeds,
   union-graph completion, component-gated acceptance.

Descriptors
-----------

``classifyTemplates`` (yCloud, nList, kNeigh)
   Overlay each k-neighbour shell onto FCC, HCP, BCC, SC. Returns a
   1-based array of ``{name, rmsd}``.

``soapSpectrum`` (yCloud, iatom, nList, nMax, lMax, rcut)
   Bartok SOAP of one particle. Flat array of length
   ``nMax * nMax * (lMax + 1)``. ``iatom`` is 0-based.

``soapSpectrumAll`` (yCloud, nList, nMax, lMax, rcut)
   SOAP of every particle. Nested table, ``nop`` rows.

``voronoiFeatures`` (yCloud, candidateCutoff)
   Per-atom ``{q4, q6, q8}`` from one Voronoi pass per order.

Prism
-----

``prismAnalysis`` (path, rings, nList, yCloud, maxDepth, atomID, firstFrame, currentFrame, doShapeMatching)
   Quasi-one-dimensional prism assignment. ``rings`` and ``nList`` are
   index lists (userdata on this bind). ``atomID`` is the lowest atom ID
   used as a reference; it is passed by value from Lua. Returns an int
   status.

Topology (ring sheets, RDF, clusters, selection, bulk)
------------------------------------------------------

These binds keep C++ reference semantics (userdata clouds and lists).

``ringAnalysis`` (path, rings, nList, yCloud, maxDepth, sheetArea, firstFrame)
   Quasi-two-dimensional polygon-ring analysis.

``calcRDF`` (path, rdfValues, yCloud, cutoff, binwidth, firstFrame, finalFrame)
   2-D RDF. ``rdfValues`` is an inout vector userdata.

``clusterAnalysis`` (path, iceCloud, yCloud, nList, iceNeighbourList, cutoff, firstFrame[, bopAnalysis])
   Largest ice cluster into ``iceCloud``. ``bopAnalysis`` is ``"q6"`` or
   ``"chill"`` (C++ default ``"q6"`` is not applied unless passed).

``recenterCluster`` (iceCloud, nList)
   Recenter the cluster cloud on the box.

``getPointCloudAtomsOfOneAtomType`` (yCloud, outCloud, atomTypeI, isSlice, low, high)
   Copy one type into ``outCloud``.

``selectInSingleSlice`` (yCloud, clearPrevious, low, high)
   Mark molecules that touch the slice.

``selectEdgeAtomsInRingsWithinSlice`` (rings, oCloud, yCloud, low, high, identicalCloud)
   Expand the slice to ring-edge molecules.

``selectAtomsInSliceWithRingEdgeAtoms`` (path, rings, oCloud, yCloud, low, high, identicalCloud)
   Same, and write IDs plus a LAMMPS data file.

``bulkRingNumberAnalysis`` (path, rings, nList, yCloud, maxDepth, firstFrame)
   Bulk primitive-ring counts.

``bulkTopologicalNetworkCriterion`` (path, rings, nList, yCloud, firstFrame, onlyTetrahedral)
   DDC / HC search on six-rings.

``bulkTopoUnitMatching`` (path, rings, nList, yCloud, firstFrame, printClusters, onlyTetrahedral[, templatePath])
   Topological unit matching. ``templatePath`` defaults to
   ``"templates"``. ``rings`` / ``nList`` may be Lua tables on this wrap.
