from tumor2d._tumorutil import tumor2d_simulate
import os
import numpy as np

pars = [

{'randseed': 788277789, 'profiledepth': 1000, 'profiletime': 17.0, 'outputrate': 24, 'endtime': 500, 'ecm_degradationrate': 0.021083607723695452, 'emc_productionrate': 0.0047593917721942651, 'ecm_threshold_quiescence': 0.0022659712759345958, 'division_depth': 13.489330712670885, 'max_celldivision_rate': 0.0024017406985124526, 'initial_quiescent_fraction': 0.69734743082613859, 'initial_radius': 11.94344040456731}
,
{'randseed': 258691170, 'profiledepth': 1000, 'profiletime': 17.0, 'outputrate': 24, 'endtime': 500, 'ecm_degradationrate': 4.5747520910011917e-05, 'emc_productionrate': 0.0027332623778742087, 'ecm_threshold_quiescence': 0.00016133781630495205, 'division_depth': 900.33300588813938, 'max_celldivision_rate': 0.0016308267826376335, 'initial_quiescent_fraction': 0.63210785488943866, 'initial_radius': 11.863449865440597}
,
{'randseed': 1195507408, 'profiledepth': 1000, 'profiletime': 17.0, 'outputrate': 24, 'endtime': 500, 'ecm_degradationrate': 9.565675636516155e-05, 'emc_productionrate': 3.6994906289935878e-05, 'ecm_threshold_quiescence': 0.013601164547683411, 'division_depth': 199.63498086764918, 'max_celldivision_rate': 0.001848082888634075, 'initial_quiescent_fraction': 0.00057780475088896213, 'initial_radius': 8.9682831223129202}
,
{'randseed': 99349953, 'profiledepth': 1000, 'profiletime': 17.0, 'outputrate': 24, 'endtime': 500, 'ecm_degradationrate': 0.0010835064520194715, 'emc_productionrate': 0.00071629065463763291, 'ecm_threshold_quiescence': 0.0078299480935141233, 'division_depth': 157.84934806528648, 'max_celldivision_rate': 0.016115036171368773, 'initial_quiescent_fraction': 2.5040536866007329e-05, 'initial_radius': 9.5247365872394596}

]


def store_gt():
    base = os.path.join(os.path.dirname(__file__), "test_data")
    try:
        os.mkdir(base)
    except FileExistsError:
        pass

    for nr, p in enumerate(pars):
        if not os.path.exists(f):
            results = tumor2d_simulate(**p)
            res_dict = dict(proliferation=results.proliferation,
                            ecm=results.extra_cellular_matrix,
                            gc=results.growth_curve)
            f = os.path.join(base, str(nr))

            np.savez(f, **res_dict)
        else:
            print("FILE EXISTS NOTHING DONE")
