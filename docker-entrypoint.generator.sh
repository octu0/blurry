#!/bin/sh

set -eux
exec 2>&1

cd /out

# build main
clang++ -g \
  -I/halide/Halide-Runtime/include -I/halide/Halide-Runtime/share/Halide/tools \
  -L/halide/Halide-Runtime/lib -lHalide `libpng-config --cflags --ldflags` -ljpeg -lpthread -ldl -std=c++11 \
  /app/blurry.cpp \
  /app/blurry_gen.cpp \
  -o blurry.out

# generate
LD_LIBRARY_PATH=/halide/Halide-Runtime/lib ./blurry.out

# build runtime
clang++ -g \
  -I/halide/Halide-Runtime/include -I/halide/Halide-Runtime/share/Halide/tools \
  -L/halide/Halide-Runtime/lib -lHalide `libpng-config --cflags --ldflags` -ljpeg -lpthread -ldl -std=c++11 \
  /app/generator.cpp /halide/Halide-Runtime/share/Halide/tools/GenGen.cpp \
  -o generator.out

# generate
LD_LIBRARY_PATH=/halide/Halide-Runtime/lib ./generator.out -r halide_runtime_x86 -e object -o . target=host-x86-64

for h in lib*_linux.a; do
  echo "ar ${h} halide_runtime";
  ar q ${h} halide_runtime_x86.o 
done

rm blurry.out
rm generator.out
