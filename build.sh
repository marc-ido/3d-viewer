#!/usr/bin/env bash
# Build script for 3D FEM Viewer  (v13)
set -e
emcc src/source.cpp -o app.js \
     -s USE_WEBGL2=1 \
     -s FULL_ES3=1 \
     -s ALLOW_MEMORY_GROWTH=1 \
     -s EXPORTED_FUNCTIONS='["_main","_setSubdivision","_solvePDE","_clearPDE"]' \
     -s EXPORTED_RUNTIME_METHODS='["ccall"]' \
     -O2
echo "Build complete."
