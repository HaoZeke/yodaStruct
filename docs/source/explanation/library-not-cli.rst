================================
The library, not a second CLI
================================

Call ``require("dseams")``. The 2020 Deferred Structural Elucidation
Analysis for Molecular Simulations (d-SEAMS) release ran as an
executable named ``yodaStruct``. This tree builds no such binary.
Call ``seams`` for the engine CLI, in
`seams-core <https://github.com/d-SEAMS/seams-core>`_.

The problem
===========

Two binaries for one engine split the driver story. A process you
cannot ``require`` does not compose: you cannot embed it in a
Fennel REPL, and you cannot call it from a program that already
owns ``main``.

.. mermaid::

   flowchart LR
     subgraph old["2020 release"]
       YS[yodaStruct executable]
       YAML[config.yml]
       GLOB[Lua globals]
       YAML --> YS
       YS --> GLOB
     end
     subgraph now["this tree"]
       REQ["require(\"dseams\")"]
       SO[dseams_core.so]
       REQ --> SO
     end
     subgraph engine["seams-core"]
       CLI[seams CLI]
       YDS[libyodaLib]
       CLI --> YDS
       SO --> YDS
     end

The 2020 executable
===================

The Journal of Chemical Information and Modeling paper (Goswami,
Goswami, and Singh, 2020,
`doi:10.1021/acs.jcim.0c00031 <https://doi.org/10.1021/acs.jcim.0c00031>`_)
released the same code as ``yodaStruct``. A run looked like:

.. code-block:: bash

   yodaStruct -c lua_inputs/config.yml

The binary parsed YAML, created a Lua state, registered C++
functions as globals (``readFrameOnlyOne``, ``neighborList``, and
others), and evaluated ``vars.lua`` plus ``functions.lua``. The
script never called ``require``. It lived inside the driver. The
process owned the program.

A later rebuild of that driver in this tree kept the same YAML
schema and the same globals, plus vendored Fennel for ``.fnl``
scripts. seams-core already owns the engine CLI (``seams``).

The library
===========

meson builds the shared module ``dseams_core``. ``lua/dseams.lua``
does ``require("dseams_core")`` and exports ``read``, ``neighbors``,
``knn``, ``chill_plus``, ``chill``, and ``cages``. A normal ``lua`` or
Fennel process loads it:

.. code-block:: lua

   local dseams = require("dseams")

``require("yoda")`` returns the same table. Python already works
this way: `pydseams <https://github.com/d-SEAMS/PydSEAMSlib>`_
loads as a module. Helpers stay in the scripting language. The
compiled module exports the registration surface (``dseams_core``,
``pydseams._core``).

Why the driver is not here
==========================

A library composes. A binary does not.

One CLI belongs with the engine. Call ``seams`` in seams-core.
This repository does not grow a second one.

The 2020 YAML workflow (config plus globals) belongs to ``seams``.
Run an ordinary file with ``lua`` after setting ``LUA_PATH`` and
``LUA_CPATH``, or after ``nix develop``. Hosts that already own
``main`` load ``luaopen_dseams_core``; see :doc:`../howto/embed-lua`.

Trade-offs
==========

- Library scripts lose the YAML driver. They gain a normal
  ``require``, a Fennel REPL, and an embeddable ``.so``.
- Compiled names live on ``dseams.core``, not as globals. Scripts
  that mention ``readFrameOnlyOne`` as a global need a rewrite.
- Optional I/O (``readCon``, ``readChemfiles``) follows the
  seams-core build flags. The helper surface stays the same.

What to call
============

=======================  ==========================================
want                     call
=======================  ==========================================
Lua / Fennel library     ``require("dseams")`` in this repository
old Lua name             ``require("yoda")`` (alias)
engine CLI               ``seams`` in seams-core
Python library           ``pydseams``
=======================  ==========================================
