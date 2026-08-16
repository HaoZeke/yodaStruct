======================
Compiled registrations
======================


Every name on ``dseams.core`` is documented on the C++ registration
header ``src/include/internal/lua_api.hpp``. Doxygen reads that file;
Sphinx includes the XML through Breathe. This page is not a hand list.

The public ``require("dseams")`` helpers are in `require("dseams") <lua.rst>`_.
Fennel call shape is in `Fennel <../howto/fennel.rst>`_.

.. doxygengroup:: dseams_core
   :content-only:
   :members:
