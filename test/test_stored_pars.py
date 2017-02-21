import pytest
import os
from .pars import pars
import numpy as np
from tumor2d import simulate
import copy


@pytest.fixture(params=list(range(1)))
def par_and_gt(request):
    base = os.path.join(os.path.dirname(__file__), "test_data")
    n = request.param
    par = pars[n]
    data = np.load(os.path.join(base, "{}.npz".format(n)))
    return (par, data)


def test_reproduce_stored(par_and_gt):
    par, data = par_and_gt
    res = simulate(**par)
    for key in data:
        assert (res[key] == data[key]).all()


def test_parameter_variation(par_and_gt):
    par, data = par_and_gt
    for key in par:
        par_cp = copy.copy(par)
        par_cp[key] *= 2
        res = simulate(**par_cp)
        for key_data in data:
            assert (res[key_data] != data[key_data]).any(),\
                "Parameter {} has no influence".format(key)
