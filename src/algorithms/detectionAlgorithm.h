#pragma once

#include <armadillo>
#include <vector>

#include "mongoDB/mongoDBDriver.h"

namespace vrtx {

// Data type for storing the location of a detected vortex
struct Vrtx {
	int id;    // Particle id
	int time;  // Timestamp of trajectory
	int len;   // Length of vortex
	int rev;   // Number of revolutions
	void print(std::string&& m) {
		std::cout << m << std::endl;
		std::cout << "\tid:          " << id << std::endl;
		std::cout << "\ttimestamp:   " << time << std::endl;
		std::cout << "\tlength:      " << len << std::endl;
		std::cout << "\trevolutions: " << rev << std::endl;
	}
};

// Base class for detection algorithms
class DetectionAlgorithm {
public:
	//DetectionAlgorithm(/*db::nvfou512n3& db) : mDb(db*/) {}
	//virtual std::vector<Vrtx> detect(int minID=0, int maxID=-1) = 0;
	virtual std::vector<Vrtx> detect(db::traj const& traj) = 0;
	//db::nvfou512n3 const& db() {
	//	return mDb;
	//}

protected:
	//db::nvfou512n3& mDb;
};

} // namespace Vrtx
