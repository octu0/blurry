// +build ignore.

#include "Halide.h"

using namespace Halide;

class EmptyGenerator : public Halide::Generator<EmptyGenerator> {
public:
    void generate() {
    }
};

