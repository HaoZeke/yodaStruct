==============
Four front ends
==============

Four entry points look like four products. They share one engine.

========================================== ======================================== ============================
Want                                       Call                                     Lives in
========================================== ======================================== ============================
Lua library                                ``require("dseams")``                    this repository
Fennel, same table                         ``(require :dseams)``                    this repository
Fennel kebab aliases                       ``fennel.dofile("lua/dseams.fnl")``      this repository
Old Lua name                               ``require("yoda")``                      this repository (alias)
Compiled registrations only                ``require("dseams_core")``               ``dseams_core.so``
Python library                             ``import pydseams``                      PydSEAMSlib
Engine CLI                                 ``seams``                                seams-core
========================================== ======================================== ============================

Helpers stay in the scripting language (``lua/dseams.lua`` here,
Python helpers in pydseams). The compiled module is the registration
surface (``dseams_core``, ``pydseams.yoda``).

YAML plus globals (``readFrameOnlyOne``, ``trajectory``,
``functionScript``) is the 2020 / ``seams`` driver contract. A library
script never sees those globals unless the host injects them. This
tree does not grow a second CLI.

See :doc:`library-not-cli`.
