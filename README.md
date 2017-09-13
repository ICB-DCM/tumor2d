# README #

Tumor2d simulation **not** precisely according to the paper but **heavily** based on
the original source code found [here](https://github.com/ICB-DCM/pABC-SMC) from the paper


```
@article{JagiellaRic2017,
	Author = {N. Jagiella and D. Rickert and F. J. Theis and J. Hasenauer},
	Doi = {10.1016/j.cels.2016.12.002},
	Journal = {Cell Systems},
	Keywords = {parameter estimation, ABC, multi-scale modeling, agent-based models},
	Month = {Feb.},
	Number = {2},
	Pages = {194--206},
	Title = {Parallelization and high-performance computing enables automated statistical inference of multiscale models},
	Volume = {4},
	Year = {2017}}
```


### Installation ###

It is required,

* that SWIG is installed,
  * either use your system package manager, or ``conda install swig``
    if running anaconda
* BLAS is installed
  * use your system package manager, if not installed. It should be already installed
    if you're running anaconda (e.g. the MKL or OpenBLAS)


Then,

* clone the repository
* and run ``python setup.py build_ext --inplace`` from within the repository
* if the repo is in your ``PYTHONPATH``, you should be able to use it now



### Distribution ###


Currently not possible to make a package via ``python setup.py bdist_wheel``
reliably.
The reason is, that ``build_ext`` has to be run before the other installation steps
so that the SWIG generated ``.py`` is properly included.
There are indications online on how to achieve running SWIG first.
However, these did not work so far.
However, it works to run ``python setup.py build_ext --inplace``, possibly followed
by ``python setup.py bdist_wheel``, as the SWIG generated ``.py`` file is
then already there.



### Running the tests ###

Run ``pytest test``.
