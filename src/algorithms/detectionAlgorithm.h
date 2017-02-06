#pragma once

#include <armadillo>
#include <vector>

#include "mongoDB/mongoDBDriver.h"

namespace vrtx {

// Data type for storing the location of a detected vortex
struct Vrtx {
	int pId;
	int timeIdx;
	int len;
	int rev;
};

// Templated base function; eventually to be preferred
//template<typename DetectionAlgorithm>
//std::vector<Vrtx> detect(DetectionAlgorithm alg) {
//	return alg.detect();
//}

// Base class for detection algorithms
class DetectionAlgorithm {
public:
	DetectionAlgorithm(db::DBInstance& db) : mDb(db) {}
	virtual std::vector<Vrtx> detect() = 0;

protected:
	db::DBInstance& mDb;
};

} // namespace Vrtx
