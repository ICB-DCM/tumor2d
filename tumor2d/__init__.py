import os
import numpy as np
from functools import wraps

from .simulate import simulate, nr_valid


__all__ = ["simulate", "nr_valid", "log_model"]


def load_default():
    path = os.path.join(os.path.dirname(__file__), "default_samples.npz")
    defaults = np.load(path)
    return defaults


def log_transform(f):
    """
    Log transform a function. For example, given
    
    f(par1=par2_val, par2=par2_val),
    then call log_transform(f)(log_par1=log_par1_val, log_par2=log_par2_val).
    The orinal function gets then called with
    f(par1=10**log_par1_val, par2=10**log_par2_val).
    
    
    Parameters
    ----------
    
    f:  Function whose parameters are to be log transformed
    
    
    Returns
    -------
    
    log transformed function
    """
    @wraps(f)
    def log_transformed(**log_kwargs):
        kwargs = dict([(key[4:], 10**value) if key.startswith("log")
                       else (key, value)
                       for key, value in log_kwargs.items()])
        return f(**kwargs)
        
    return log_transformed

def log_model(x):
    log_transform(simulate)(**x)
