from tumor2d.simulate import nr_valid
import numpy as np


def test_valid_1():
    arr = np.array([1, 2, 3, 0, 0])
    assert nr_valid(arr) == 3


def test_valid_1_1():
    arr = np.array([1, 0, 3, 0, 0])
    assert nr_valid(arr) == 3


def test_valid_1_2():
    arr = np.array([0, 0, 3, 0, 0])
    assert nr_valid(arr) == 3


def test_valid_2():
    arr = np.array([1, 2, 3, 4, 4])
    assert nr_valid(arr) == 5


def test_valid_2_1():
    arr = np.array([0, 2, 3, 4, 4])
    assert nr_valid(arr) == 5


def test_valid_2_2():
    arr = np.array([0, 2, 3, 0, 4])
    assert nr_valid(arr) == 5


def test_valid_3():
    arr = np.array([0, 0, 0])
    assert nr_valid(arr) == 0


def test_valid_4():
    arr = np.array([0])
    assert nr_valid(arr) == 0


def test_valid_5():
    arr = np.array([1])
    assert nr_valid(arr) == 1


def test_valid_6():
    arr = np.array([])
    assert nr_valid(arr) == 0
