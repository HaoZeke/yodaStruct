==========================
Frequently Asked Questions
==========================


.. contents::


Frequently Asked Questions
--------------------------

Installation
~~~~~~~~~~~~

Where is the yodaStruct binary?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This tree does not build one. Call ``require("dseams")``. The
engine CLI is ``seams`` in
`seams-core <https://github.com/d-SEAMS/seams-core>`_.

See: `The library, not a second CLI <../explanation/library-not-cli.rst>`_

How do I install the library?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Build ``dseams_core.so`` with meson, then set ``LUA_PATH`` and
``LUA_CPATH``. Nix: ``nix develop``.

.. code:: bash

    meson setup bbdir --wrap-mode=nofallback
    meson compile -C bbdir
    export LUA_PATH="$PWD/lua/?.lua;;"
    export LUA_CPATH="$PWD/bbdir/?.so;;"

See: `Install the library <install.rst>`_

I get "module 'dseams' not found"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``LUA_PATH`` does not include the directory that holds
``dseams.lua``.

.. code:: bash

    export LUA_PATH="$PWD/lua/?.lua;;"

I get "module 'dseams\ :sub:`core`\' not found"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``LUA_CPATH`` does not include the directory that holds
``dseams_core.so``.

.. code:: bash

    export LUA_CPATH="$PWD/bbdir/?.so;;"

Usage
~~~~~

What do I require?
^^^^^^^^^^^^^^^^^^

.. code:: lua

    local dseams = require("dseams")

Fennel: ``(require :dseams)``. ``require("yoda")`` is the same table.

What is dseams.core?
^^^^^^^^^^^^^^^^^^^^

The compiled registrations from ``dseams_core.so``
(``luaApi::registerAll``). Helpers (``read``, ``chill_plus``, ``cages``)
stay in ``lua/dseams.lua`` and call names on ``dseams.core``.

How do I read a LAMMPS dump?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: lua

    local cloud = dseams.read("input/traj/exampleTraj.lammpstrj", {type = 2})

Pass the oxygen type. On the shipped dump that type is 2.

See: `Read a dump and classify <../tutorials/read-and-classify.rst>`_

Why do chill\ :sub:`plus`\ and cages disagree?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

They answer different questions. ``chill_plus`` is a four-neighbour
bond-order label. ``cages`` is six-ring HC / DDC membership. A
cubic oxygen need not sit in a DDC.

Does chill\ :sub:`plus`\ write a file?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

No. The helper calls ``getIceTypePlusNoPrint``. The cloud is
mutated. The return value is an array of state names.

Which Lua version?
^^^^^^^^^^^^^^^^^^

The Nix package and the meson Lua probe target Lua 5.4. The
meson wrap fallback is ``lua-5.4``.

Errors
~~~~~~

My cloud has nop=0
^^^^^^^^^^^^^^^^^^

The type filter does not match the dump, or the path does not
resolve. Work from the repository root. Pass ``{type = 2}`` on the
shipped example.

readCon / readChemfiles is not in this build
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Those binds exist only when seams-core was compiled with
``SEAMS_HAS_READCON`` or ``SEAMS_HAS_CHEMFILES``. LAMMPS dumps and
``.xyz`` do not need them.

Fennel loads dseams.fnl as itself
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Do not put ``lua/?.fnl`` on ``fennel.path`` and then
``(require :dseams)``. Load kebab-case wrappers with
``fennel.dofile`` under another name. See `Fennel <fennel.rst>`_.

Compatibility
~~~~~~~~~~~~~

Is require("yoda") going away?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

``lua/yoda.lua`` returns ``require("dseams")``. Prefer
``require("dseams")``.

Where is the Python API?
^^^^^^^^^^^^^^^^^^^^^^^^

`pydseams <https://github.com/d-SEAMS/PydSEAMSlib>`_. Helpers stay
in Python. The compiled surface is ``pydseams.yoda`` (``_core`` is an
alias).

Where is the engine CLI?
^^^^^^^^^^^^^^^^^^^^^^^^

``seams`` in `seams-core <https://github.com/d-SEAMS/seams-core>`_.
The 2020 YAML workflow (config plus globals) is gone. Runtime knobs
are twelve-factor (``SEAMS_CONFIG`` / ``./seams.env``, then the
environment, then CLI flags).

See Also
~~~~~~~~

- `Install the library <install.rst>`_

- `Troubleshooting <troubleshooting.rst>`_

- `Tutorials <../tutorials/index.rst>`_

- `Lua surface <../reference/lua.rst>`_
