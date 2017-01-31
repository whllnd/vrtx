#pragma once

#include <armadillo>
#include <vector>

namespace vrtx {

// Data type for storing the location of a detected vortex
struct Vrtx {
	int pId;
	int timeIdx;
	int len;
};

// Templated base function; eventually to be preferred
template<typename DetectionAlgorithm>
std::vector<Vrtx> detect(DetectionAlgorithm alg) {
	return alg.detect(trajectories);
}

// Base class for detection algorithms
//class DetectionAlgorithm {
//public:
//	virtual std::vector<Vrtx> detect(arma::cube trajectories) = 0;
//};

} // namespace Vrtx
