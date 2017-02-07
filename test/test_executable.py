from tumor2d import simulate


def test_result_non_empty():
    sim = simulate()
    assert len(sim['extra_cellular_matrix_profile']) > 0
    assert len(sim['growth_curve']) > 0
    assert len(sim['proliferation_profile']) > 0


def test_same_seed_same_result():
    sim1 = simulate(randseed=1)
    sim2 = simulate(randseed=1)
    assert (sim1['extra_cellular_matrix_profile'] == sim2['extra_cellular_matrix_profile']).all()
    assert (sim1['growth_curve'] == sim2['growth_curve']).all()
    assert (sim1['proliferation_profile'] == sim2['proliferation_profile']).all()


def test_different_seed_different_result():
    sim1 = simulate(randseed=1)
    sim2 = simulate(randseed=2)

    assert ((sim1['extra_cellular_matrix_profile'] != sim2['extra_cellular_matrix_profile']).any()
            or (sim1['growth_curve'] != sim2['growth_curve']).any()
            or (sim1['proliferation_profile'] != sim2['proliferation_profile']).any()
            )
