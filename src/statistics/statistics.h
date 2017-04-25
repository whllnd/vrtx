#pragma once

#include <vector>

#include "algorithms/detectionAlgorithm.h"
#include "mongoDB/mongoDBDriver.h"

namespace vrtx {
namespace statistics {

void    all(db::nvfou512n3 const& db, std::vector<Vrtx> const& vortices);
void volume(db::nvfou512n3 const& db, std::vector<Vrtx> const& vortices);
void length(std::vector<Vrtx> const& vortices);

template<typename T>
void evalDetectionAlgorithm(T const& alg);

template<typename T>
void evalDetectionAlgorithm(T&& alg) {

	// Get database instance
	auto db = alg.db();

	// Get ground truth data
	auto gt = std::vector<Vrtx>(); //db.queryField<std::vector<Vrtx>>("ground_truth"); TODO

	// Get id's of ground truth data
	std::vector<int> idVec(gt.size());
	for (auto const& vortex : gt) {
		idVec.push_back(vortex.id);
	}

	// Make detection algorithm work
	auto vortices = alg.detect(idVec);

	// TODO: Compare vortices detect by algorithm with ground truth data
	// ...
}

} // namespace statistics
} // namespace vrtx
