#pragma once

#include <armadillo>
#include <vector>

namespace vrtx {

// Data type for storing the location of a detected vortex
struct vrtx {
	vrtx(int idx=0, int time=0, int length=0)
	: particleIdx(idx)
	, timestamp(time)
	, len(length)
	 {}
	int particleIdx;
	int timestamp;
	int len;
};

// Base class for detection algorithms
class detectionAlgorithm {
public:
	virtual std::vector<vrtx> detect(arma::cube trajectories) = 0;
};

} // namespace vrtx
