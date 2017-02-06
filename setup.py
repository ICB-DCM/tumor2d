import setuptools
import setuptools.command.install
from setuptools import setup, Extension, find_packages
from glob import glob
from subprocess import run, PIPE

class Build_ext_first(setuptools.command.install.install):
    def run(self):
        self.run_command("build_ext")
        return setuptools.command.install.install.run(self)

gsl_cflags = run(["gsl-config", "--cflags"], stdout=PIPE).stdout.decode()


tumor2d_ext = Extension(
    name='_nixTumor2d',
    sources=(['tumor2d/src/tumor_if.i']
             + [f for f in glob("tumor2d/src/*.cpp")
                if f != "tumor2d/src/tumor_if_wrap.cpp"]),
    libraries=["gsl", "gslcblas", "m"],
    swig_opts=['-c++', '-modern'],
    extra_compile_args=["-DDIMENSIONS=2"] + gsl_cflags.split(" "))

setup(
    ext_modules=[tumor2d_ext],
    name="tumor2d",
    version="0.1.0",
    packages=find_packages(),
    cmdclass={'install': Build_ext_first}
)
