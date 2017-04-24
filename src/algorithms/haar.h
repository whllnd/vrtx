#pragma once

#include <cmath>
#include <cassert>

#include "detectionAlgorithm.h"
#include "mongoDB/mongoDBDriver.h"
#include "tools/visualize.h"

namespace vrtx {
namespace detection {

class HaarTransform : public DetectionAlgorithm {
public:

	HaarTransform(
		db::nvfou512n3& db,
		double sigma,
		int revolutions,
		int gapWidth=16
	)
	: DetectionAlgorithm(db)
	, mSigma(sigma)
	, mMinRev(revolutions)
	, mGapWidth(gapWidth)
	{}

	std::vector<Vrtx> detect(int minID=0, int maxID=-1);

private:

	// trajectory of form MxN with M dimensions and N timesteps
	auto haarTransform(arma::mat trajectory);
	auto findVortices(
		arma::mat const& traj,
		arma::mat const& energies,
		std::vector<Vrtx>& vortices,
		int const pId
	);
	auto zeroCross(arma::mat&& vortex);
	auto buildEnergyMatrix(std::vector<arma::rowvec> const& energies);

	int static constexpr mScales = 5;
	double mSigma;
	int mMinRev;
	int mGapWidth;

	double static constexpr mSqrt2 = std::sqrt(2.);
	arma::colvec static const mStandardDeviations;
};

} // namespace detection
} // namespace haar
