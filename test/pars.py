from tumor2d import simulate
import os
import numpy as np

pars = [
    dict(division_rate=4.17e-2,
         initial_spheroid_radius=1.2e1,
         initial_quiescent_cell_fraction=7.5e-1,
         division_depth=100,
         ecm_production_rate=5e-3,
         ecm_degradation_rate=8e-4,
         ecm_division_threshold=1e-2,
         randseed=42),
]


def store_gt():
    base = os.path.join(os.path.dirname(__file__), "test_data")
    try:
        os.mkdir(base)
    except FileExistsError:
        pass

    for nr, p in enumerate(pars):
        f = os.path.join(base, str(nr))
        if not os.path.exists(f):
            results = simulate(**p)
            res_dict = dict(
                proliferation_profile=results['proliferation_profile'],
                extra_cellular_matrix_profile=
                results['extra_cellular_matrix_profile'],
                growth_curve=results['growth_curve'])
            f = os.path.join(base, str(nr))

            np.savez(f, **res_dict)
        else:
            print("FILE EXISTS NOTHING DONE")
