#include <iostream>

#include "algorithms/detectionAlgorithm.h"
#include "algorithms/haar.h"
#include "mongoDB/mongoDBDriver.h"
#include "statistics/statistics.h"

int main() {

	// Connect to database and cache some trajectories for performance reasons
	vrtx::db::nvfou512n3 db;
	db.info();
	auto trajectories{db.trajectories(0, 100, vrtx::db::nvfou512n3::latAcc)};

	// Detect vortices
	std::vector<vrtx::Vrtx> vortices;
	vrtx::detection::HaarTransform haar(/*db,*/ 3., 2);
	for (std::size_t i(0); i < trajectories.size(); i++) {
		auto detected{haar.detect(trajectories[i])};
		std::move(detected.begin(), detected.end(), std::back_inserter(vortices));
	}

	// Gather statistics
	vrtx::statistics::all(db, vortices);
	vrtx::statistics::evalDetectionAlgorithm(db, haar);
	exit(0);
}

