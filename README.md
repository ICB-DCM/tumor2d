# README #

Tumor2d simulation **not** according to the paper. 

### Installation ###

It is required,

* that SWIG is installed,
* GSL (gnu scientific library) is installed
* BLAS is installed.


Then,

* clone the repository
* and run ``python setup.py build_ext --inplace`` from within the repository
* if the repo is in you ``PYTHONPATH``, you should be able to use it now



### Distribution ###


Currently not possible to make a package via ``python setup.py bdist_wheel``
reliably.
The reason is, that ``build_ext`` has to be run before the other installation steps
so that the SWIG generated ``.py`` is properly included.
However, it works to run ``python setup.py build_ext --inplace``, possibly followed
by ``python setup.py bdist_wheel``, as the SWIG generated ``.py`` file is
then already there.



### Running the tests ###

Run ``pytest test``.