from ._tumorutil import tumor2d_simulate
import numpy as np


def simulate(**kwargs):
    res = tumor2d_simulate(**kwargs)
    return dict(growth_curve=np.array(res.growth_curve),
                extra_cellular_matrix=np.array(res.extra_cellular_matrix),
                proliferation=np.array(res.proliferation))