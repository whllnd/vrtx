#include <iostream>

#include "algorithms/detectionAlgorithm.h"
#include "algorithms/haar.h"
#include "mongoDB/mongoDBDriver.h"
#include "statistics/statistics.h"

int main() {

	// Connect to database
	vrtx::db::nvfou512n3 db;
	db.info();

	// Detect vortices
	vrtx::detection::HaarTransform haar(db, 3., 2);
	auto vortices = haar.detect();

	// Gather statistics
	vrtx::statistics::all(vortices);
}

