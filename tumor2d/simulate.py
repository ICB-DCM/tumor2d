from ._tumorutil import tumor2d_simulate
import numpy as np


def simulate(division_rate=4.7e-2,
             initial_spheroid_radius=1.2e1,
             initial_quiescent_cell_fraction=7.5e-1,
             ecm_production_rate=5-3,
             ecm_degradation_rate=8e-4,
             ecm_division_threshold=1e-2,
             randseed=None):
    """

    Parameters
    ----------


    .. notes::

        Jagiella, Nick, Dennis Rickert, Fabian J. Theis, and Jan Hasenauer.
        “Parallelization and High-Performance Computing Enables Automated
        Statistical Inference of Multi-Scale Models.”
        Cell Systems 0, no. 0 (January 11, 2017). doi:10.1016/j.cels.2016.12.002.

        First table on page e4

    """
    res = tumor2d_simulate(max_celldivision_rate=division_rate,
                           initial_radius=initial_spheroid_radius,
                           initial_quiescent_fraction=initial_quiescent_cell_fraction,
                           emc_productionrate=ecm_production_rate,
                           ecm_degradationrate=ecm_degradation_rate,
                           ecm_threshold_quiescence=ecm_division_threshold,
                           randseed=randseed)
    return dict(growth_curve=np.array(res.growth_curve),
                extra_cellular_matrix=np.array(res.extra_cellular_matrix),
                proliferation=np.array(res.proliferation))
