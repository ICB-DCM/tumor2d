import os
import numpy as np
from .simulate import simulate, nr_valid
from .distance import Tumor2DDistance 
from .log_transform import log_transform

__all__ = ["simulate", "nr_valid", "log_model"]


stored_data_db = os.path.join(os.path.dirname(__file__), "db1.db")


def load_default():
    path = os.path.join(os.path.dirname(__file__), "default_samples.npz")
    raw_data = np.load(path)
    data_var = {key: val.var(axis=0) for key, val in raw_data.items()}
    data_mean = {key: val.mean(axis=0) for key, val in raw_data.items()}    

    for key in ["extra_cellular_matrix_profile", 'proliferation_profile']:
        n_valid_points = nr_valid(data_mean[key])
        nonzero = np.count_nonzero(data_mean[key])
        assert n_valid_points >= nonzero
        data_mean[key] = data_mean[key][:n_valid_points]
        data_var[key] = data_var[key][:n_valid_points]
        
    return raw_data, data_mean, data_var


def log_model(x):
    return log_transform(simulate)(**x)

    
    
distance = Tumor2DDistance(load_default()[2])

