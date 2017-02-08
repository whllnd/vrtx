#pragma once

namespace vrtx {
namespace statistics {

void all(std::vector<Vrtx> const& vortices);
void length(std::vector<Vrtx> const& vortices);
void volume(std::vector<Vrtx> const& vortices);

template<typename T>
void evalDetectionAlgorithm(T const& alg);

template<typename T>
void evalDetectionAlgorithm(T&& alg) {

	// Get database instance
	auto db = alg.db();

	// Get ground truth data
	auto gt = db.queryField<std::vector<Vrtx>>("ground_truth");

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
