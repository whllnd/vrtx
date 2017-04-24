#pragma once

#include <armadillo>
#include <vector>

#include "mongoDB/mongoDBDriver.h"

namespace vrtx {

// Data type for storing the location of a detected vortex
struct Vrtx {
	int id;
	int timestamp;
	int len;
	int rev;
	void print(std::string&& m) {
		std::cout << m << std::endl;
		std::cout << "\tid:          " << id << std::endl;
		std::cout << "\ttimestamp:   " << timestamp << std::endl;
		std::cout << "\tlength:      " << len << std::endl;
		std::cout << "\trevolutions: " << rev << std::endl;
	}
};

// Templated base function; eventually to be preferred
//template<typename DetectionAlgorithm>
//std::vector<Vrtx> detect(DetectionAlgorithm alg) {
//	return alg.detect();
//}

// Base class for detection algorithms
class DetectionAlgorithm {
public:
	DetectionAlgorithm(db::nvfou512n3& db) : mDb(db) {}
	virtual std::vector<Vrtx> detect(int minID=0, int maxID=-1) = 0;
	db::nvfou512n3 const& db() {
		return mDb;
	}

protected:
	db::nvfou512n3& mDb;
};

} // namespace Vrtx
