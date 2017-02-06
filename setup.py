import setuptools
import setuptools.command.install
from setuptools import setup, Extension, find_packages
from glob import glob
from subprocess import run, PIPE

ENABLE_OPENMP = False

# I think this build ext first
class Build_ext_first(setuptools.command.install.install):
    def run(self):
        self.run_command("build_ext")
        return setuptools.command.install.install.run(self)

gsl_cflags = (run(["gsl-config", "--cflags"], stdout=PIPE)
              .stdout
              .decode()
              .strip("\n")
              .split(" "))

gsl_include = [flag[2:] for flag in gsl_cflags if flag.startswith("-I")]


if ENABLE_OPENMP:
    omp_liraries = ["gomp"]
    omp_compile_args = ["-fopenmp"]
else:
    omp_liraries = []
    omp_compile_args = []


tumor2d_ext = Extension(
    name='_nixTumor2d',
    sources=(['tumor2d/src/tumor_if.i']
             + [f for f in glob("tumor2d/src/*.cpp")
                if f != "tumor2d/src/tumor_if_wrap.cpp"]),
    libraries=["gsl", "gslcblas", "m"] + omp_liraries,
    swig_opts=['-c++', '-modern'],
    include_dirs=gsl_include,
    extra_compile_args=["-DDIMENSIONS=2"] + omp_compile_args)

setup(
    ext_modules=[tumor2d_ext],
    name="tumor2d",
    version="0.1.0",
    packages=find_packages(),
    cmdclass={'install': Build_ext_first}
)
