from setuptools import setup, Extension
from glob import glob

setup(
    ext_modules=[Extension(name='_nixTumor2d',
                           sources=(['src/tumor_if.i']
                                    + glob("src/*.cpp")),
                           libraries=["gsl", "cblas"],
                           swig_opts=['-c++', '-modern', '-I../include'],
                           extra_compile_args=["-DDIMENSIONS=2"])
                ]
    )