#include "algorithms/detectionAlgorithm.h"
#include "algorithms/haar.h"
#include "mongoDB/mongoDBDriver.h"
#include "statistics/stats.h"

int main() {

	// Connect to database; TODO: probably throws an exception, when no database is running
	vrtx::db::DBInstance db;

	// Detect vortices
	vrtx::detection::HaarTransform haar(db, 3, 2);
	auto vortices = vrtx::detect(haar, trajectories);

	// Gather statistics
	vrtx::statistics::all(vortices);
}

