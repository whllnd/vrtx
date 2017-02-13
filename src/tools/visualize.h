#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <armadillo>

namespace vrtx {
namespace visualize {

// Pass copy, since we need to copy anyway
auto energies(arma::mat energyMat) -> void;

} // namespace visualize
} // namespace vrtx
