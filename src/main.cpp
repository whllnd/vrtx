#include <iostream>

#include "algorithms/detectionAlgorithm.h"
#include "algorithms/haar.h"
#include "mongoDB/mongoDBDriver.h"
#include "statistics/statistics.h"

int main() {

	// Connect to database
	vrtx::db::DBInstance nvfou512n3;

	// Detect vortices
	vrtx::detection::HaarTransform haar(nvfou512n3, 3, 2);
	auto vortices = haar.detect();

	// Gather statistics
	vrtx::statistics::all(vortices);
}

