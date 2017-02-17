import pytest
import os
from .pars import pars
import numpy as np
from tumor2d._tumorutil import tumor2d_simulate


@pytest.fixture(params=list(range(4)))
def par_and_gt(request):
    base = os.path.join(os.path.dirname(__file__), "test_data")
    n = request.param
    par = pars[n]
    data = np.load(os.path.join(base, "{}.npz".format(n)))
    return (par, data)


def test_stored(par_and_gt):
    par, data = par_and_gt
    res = tumor2d_simulate(**par)
    assert (res.growth_curve == data["gc"]).all()
    assert (res.extra_cellular_matrix == data["ecm"]).all()
    assert (res.proliferation == data["proliferation"]).all()
