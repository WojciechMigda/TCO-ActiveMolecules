#!/bin/sh

cat header.hpp matrix.hpp algebra.hpp cache.hpp CP.hpp molecule_input_placeholder.hpp similarities_input_placeholder.hpp ActiveMolecules.hpp | grep -v "#include \"" > submission.cpp
g++ -std=c++11 -c submission.cpp
gvim submission.cpp &
