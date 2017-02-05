#!/usr/bin/env bash
# this is an old file
# package should be build with setuptools, not with this script

cd src
swig -python -c++ tumor_if.i
c++ -c tumor_if.cpp tumor_if_wrap.cxx AsciiIO.cpp Montecarlo.cpp Agent.cpp Angiogenesis.cpp CellProcesses.cpp DiffusionReactionEquation.cpp finiteDifferences.cpp Interpolation.cpp Mathematix.cpp Molecule.cpp SparseMatrix.cpp Substrate.cpp EpsIO.cpp  Vessel_Graph.cpp Vessel_Unit.cpp VesselNetwork.cpp VoronoiDiagramExtended.cpp statistics.cpp hash.cpp -I/usr/include/python3.6m/ -DDIMENSIONS=2 -fPIC -g
g++ -shared tumor_if.o tumor_if_wrap.o AsciiIO.o Montecarlo.o Agent.o Angiogenesis.o CellProcesses.o DiffusionReactionEquation.o finiteDifferences.o Interpolation.o Mathematix.o Molecule.o SparseMatrix.o Substrate.o EpsIO.o Vessel_Graph.o Vessel_Unit.o VesselNetwork.o VoronoiDiagramExtended.o statistics.o hash.o -lgsl -lgslcblas -g -o _nixTumor2d.so

cd ..
rm -f src/*.o
rm -f src/*.lo
rm -f src/*.la
rm -f src/*.o
mv src/_nixTumor2d.so .


