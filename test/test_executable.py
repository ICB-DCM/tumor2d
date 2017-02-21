from tumor2d import simulate


def test_result_non_empty():
    sim = simulate()
    assert len(sim['extra_cellular_matrix_profile']) > 0
    assert len(sim['growth_curve']) > 0
    assert len(sim['proliferation_profile']) > 0
    assert sim['growth_curve'][0] < sim['growth_curve'][-1]

