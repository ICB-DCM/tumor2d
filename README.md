# README #

Tumor2d simulation is not precisely according to but heavily based on
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

It is required that

* SWIG is installed,
  * either use your system package manager, or ``conda install swig``
    if running anaconda
* BLAS is installed
  * use your system package manager, if not installed. It should be already installed if you're running anaconda (e.g. the MKL or OpenBLAS)


Python requirements are:

* numpy.


Then,

* clone the repository ``git clone https://github.com/ICB-DCM/tumor2d``
* change into the repository directory ``cd tumor2d``
* and run ``python setup.py build_ext --inplace`` from within the repository
* if the repo is in your ``PYTHONPATH``, you should be able to use it now
* if you want to install the package, run ``pip install .``


### Usage ###

The function ``tumor2d.simulate`` is the main entry point.
To run it with the default parameters, do

```
from tumor2d import simulate
simulate()
```

It takes 8 parameters as input:

* ``division_rate``
* ``initial_spheroid_radius``
* ``initial_quiescent_cell_fraction``
* ``division_depth``
* ``ecm_production_rate``
* ``ecm_degradation_rate``
* ``ecm_division_threshold``
* ``randseed``


The function returns a dictionary containing the
* ``growth_curve``
* ``extra_cellular_matrix_profile``
* ``proliferation_profile``

which can be used as summary statistics for ABC-SMC inference
with [pyABC](http://pyabc.readthedocs.io/en/latest/). In the documentation of pyABC, there is an [example](http://pyabc.readthedocs.io/en/latest/examples/multiscale_agent_based.html) included, showing how to perform analysis for this model.

### Distribution ###


Currently, it is not possible to make a package via ``python setup.py bdist_wheel`` reliably. The reason i, that ``build_ext`` has to be run before the other installation steps, so that the SWIG generated ``.py`` is properly included.
There are indications online on how to achieve running SWIG first. However, these did not work so far. Meanwhile, it works to run ``python setup.py build_ext --inplace``, possibly followed by ``python setup.py bdist_wheel``, as the SWIG generated ``.py`` file is then already there.



### Running the tests ###

Run ``pytest test``.
