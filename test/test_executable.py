from tumor2d import tumor2d_simulate


def test_can_be_run():
    sim = tumor2d_simulate()
    assert len(sim.extra_cellular_matrix) > 0
    assert len(sim.growth_curve) > 0
    assert len(sim.proliferation) > 0
