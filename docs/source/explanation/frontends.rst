==============
Three products
==============


Three products share one engine. Fennel, ``require("yoda")``, and
``dseams_core`` are spellings of the Lua library, not extra products.

.. table::

    +----------------------+-----------------------+---------------------------------------------------------+
    | want                 | call                  | lives in                                                |
    +======================+=======================+=========================================================+
    | Lua / Fennel library | ``require("dseams")`` | this repository                                         |
    +----------------------+-----------------------+---------------------------------------------------------+
    | Python library       | ``import pydseams``   | `PydSEAMSlib <https://github.com/d-SEAMS/PydSEAMSlib>`_ |
    +----------------------+-----------------------+---------------------------------------------------------+
    | Engine CLI           | ``seams``             | `seams-core <https://github.com/d-SEAMS/seams-core>`_   |
    +----------------------+-----------------------+---------------------------------------------------------+

Aliases of the Lua product: ``(require :dseams)``,
``fennel.dofile("lua/dseams.fnl")``, ``require("yoda")``.
``require("dseams_core")`` is the compiled registration surface,
not a product.

Helpers stay in the scripting language (``lua/dseams.lua`` here,
Python helpers in pydseams). The compiled module is the registration
surface (``dseams_core``, ``pydseams.yoda``).

YAML plus globals (``readFrameOnlyOne``, ``trajectory``,
``functionScript``) is the 2020 driver. It is not live. A library
script never sees those globals unless the host injects them. This
tree does not grow a second CLI. ``seams`` is flags and twelve-factor
knobs, not ``conf.yaml``.

See `The library, not a second CLI <library-not-cli.rst>`_.
