======
dseams
======

.. raw:: html

   <p class="dseams-hero">
     <img class="dseams-hero-logo dseams-hero-logo--light"
          src="_static/logo/dseams-logo-light.png"
          alt="dseams"
          width="320"
          height="320"
          loading="eager" />
     <img class="dseams-hero-logo dseams-hero-logo--dark"
          src="_static/logo/dseams-logo-dark.png"
          alt="dseams"
          width="320"
          height="320"
          loading="eager" />
   </p>

Lua and Fennel library for the d-SEAMS C++ engine.

.. grid:: 1 2 3 3
   :gutter: 2
   :padding: 1 1 0 0
   :class-container: sd-text-center

   .. grid-item-card:: Read
      :link: quickstart
      :link-type: doc
      :class-card: sd-shadow-sm

      Load a dump with ``require("dseams")``.

   .. grid-item-card:: Classify
      :link: tutorials/read-and-classify
      :link-type: doc
      :class-card: sd-shadow-sm

      CHILL+ labels and HC / DDC cage flags.

   .. grid-item-card:: Embed
      :link: howto/embed-lua
      :link-type: doc
      :class-card: sd-shadow-sm

      Host ``dseams_core.so`` from Lua, Fennel, or C.

About
=====

This repository is a *library*. There is no ``yodaStruct`` executable.
Call ``require("dseams")`` from Lua, or ``(require :dseams)`` from Fennel.
The engine CLI is ``seams`` in
`seams-core <https://github.com/d-SEAMS/seams-core>`_.
Python is `pydseams <https://github.com/d-SEAMS/PydSEAMSlib>`_.

.. code-block:: lua

   local dseams = require("dseams")
   local cloud = dseams.read("water.lammpstrj")
   print(dseams.chill_plus(cloud, {cutoff = 3.5}))

``dseams.core`` is the compiled registrations (``dseams_core.so``).
Helpers stay in Lua. ``require("yoda")`` still resolves to ``dseams``.

Suite stack
===========

How the Lua library, the compiled module, and the rest of d-SEAMS
relate:

.. mermaid::

   flowchart TB
     subgraph hosts["Hosts"]
       LUA[lua]
       FNL[Fennel]
       EMB["C / C++ embed"]
     end
     subgraph lib["this repository"]
       HELPERS["lua/dseams.lua"]
       CORE["dseams_core.so"]
     end
     subgraph engine["seams-core"]
       YDS[libyodaLib]
       SEAMS[seams CLI]
     end
     subgraph py["PydSEAMSlib"]
       PY[pydseams]
     end
     LUA --> HELPERS
     FNL --> HELPERS
     EMB --> CORE
     HELPERS --> CORE
     LC[linkcell]
     LC --> YDS
     CORE --> YDS
     SEAMS --> YDS
     PY --> YDS

.. tip::

   ``require("dseams")`` loads the helpers. Those helpers call
   ``require("dseams_core")``. Set ``LUA_PATH`` to the Lua files and
   ``LUA_CPATH`` to the directory that holds ``dseams_core.so``.

Documentation structure
=======================

This documentation follows the `Diataxis <https://diataxis.fr/>`_ framework.

.. toctree::
   :maxdepth: 2
   :caption: Getting Started

   quickstart

.. toctree::
   :maxdepth: 2
   :caption: Tutorials

   tutorials/index
   tutorials/read-and-classify

.. toctree::
   :maxdepth: 2
   :caption: How-to

   howto/index
   howto/install
   howto/fennel
   howto/embed-lua
   howto/faq
   howto/troubleshooting

.. toctree::
   :maxdepth: 2
   :caption: Explanation

   explanation/index
   explanation/library-not-cli
   explanation/frontends
   explanation/citation

.. toctree::
   :maxdepth: 2
   :caption: Reference

   reference/index
   reference/lua
   reference/lua-functions

The compiled-registration appendix is :doc:`reference/lua-functions`,
generated from Doxygen of ``lua_api.hpp``.

Related projects
================

- `seams-core <https://github.com/d-SEAMS/seams-core>`_ :: C++ engine and ``seams`` CLI
- `pydseams <https://github.com/d-SEAMS/PydSEAMSlib>`_ :: Python Frame API on the same engine
- `linkcell <https://github.com/d-SEAMS/linkcell>`_ :: periodic linked-cell k-nearest neighbours
- `dseams.info <https://dseams.info>`_ :: Project site

License
=======

MIT. Cite the `2020 JCIM paper <https://doi.org/10.1021/acs.jcim.0c00031>`_
and the ``CITATION.cff`` in this tree. See :doc:`explanation/citation`.

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
