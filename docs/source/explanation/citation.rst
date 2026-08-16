===========
How to cite
===========

If you use this library, cite the d-SEAMS paper. The software
record is the ``CITATION.cff`` in this tree.

Preferred citation
==================

Goswami, Rohit; Goswami, Amrita; Singh, Jayant Kumar.
d-SEAMS: Deferred Structural Elucidation Analysis for Molecular
Simulations. *Journal of Chemical Information and Modeling*
2020, doi:`10.1021/acs.jcim.0c00031 <https://doi.org/10.1021/acs.jcim.0c00031>`_.

.. code-block:: bibtex

   @article{goswami2020dseams,
     author  = {Goswami, Rohit and Goswami, Amrita and Singh, Jayant Kumar},
     title   = {d-{SEAMS}: Deferred Structural Elucidation Analysis
                for Molecular Simulations},
     journal = {Journal of Chemical Information and Modeling},
     year    = {2020},
     doi     = {10.1021/acs.jcim.0c00031},
   }

Software citation
=================

``CITATION.cff`` names the software
``yodaStruct: Lua and Fennel front end for d-SEAMS`` and points
the preferred citation at the paper above. Cite the paper for
the methods. Cite the CFF when you need a software artifact
(repository `d-SEAMS/yodaStruct <https://github.com/d-SEAMS/yodaStruct>`_,
license MIT).

Call the library ``dseams`` in prose (``require("dseams")``). The
repository name remains ``yodaStruct``.

Acknowledgments
===============

.. code-block:: text

   Structural classification used dseams
   (https://github.com/d-SEAMS/yodaStruct), the Lua and Fennel
   library for the d-SEAMS engine [Goswami, Goswami, and Singh,
   J. Chem. Inf. Model. 2020, 10.1021/acs.jcim.0c00031].

Related software
================

- `seams-core <https://github.com/d-SEAMS/seams-core>`_ :: C++ engine and ``seams`` CLI
- `pydseams <https://github.com/d-SEAMS/PydSEAMSlib>`_ :: Python bindings on the same engine

See also
========

- :doc:`library-not-cli`
- :doc:`../reference/lua`
- :doc:`../tutorials/index`
