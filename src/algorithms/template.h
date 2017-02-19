#pragma once

#include <cmath>
#include <cassert>

#include "detectionAlgorithm.h"
#include "mongoDB/mongoDBDriver.h"
#include "tools/visualize.h"

namespace vrtx {
namespace detection {

class Template : public DetectionAlgorithm { // TO REPLACE: Template
public:

	Template(db::DBInstance& db, int exampleVar /* Further variables the algorithm depends on ... */)
	: DetectionAlgorithm(db)
	, mExampleVar(exampleVar)
	{
		// Constructor definition goes here, if more than just variable
		// initialization is needed
	}

	// Need to implement this, since it's demanded from base class DetectionAlgorithm
	std::vector<Vrtx> detect();

private:

	// Example function declaration
	std::vector<Vrtx> extractVorticesFromTrajectory(arma::mat const& trajectory);

	// Example member variable
	int mExampleVar;
};

} // namespace detection
} // namespace haar
