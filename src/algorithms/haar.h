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
		//db::nvfou512n3& db,
		double sigma,
		int revolutions,
		int gapWidth=16,
		int minLen=50
	)
	: DetectionAlgorithm(/*db*/)
	, mSigma(sigma)
	, mMinRev(revolutions)
	, mGapWidth(gapWidth)
	, mMinLen(minLen)
	{}

	std::vector<Vrtx> detect(int minID=0, int maxID=-1);
	std::vector<Vrtx> detect(db::traj const& traj);

private:

	// trajectory of form MxN with M dimensions and N timesteps
	auto haarTransform(arma::mat trajectory) const;
	auto findVortices(db::traj const& traj, arma::mat const& energies) const;
	auto zeroCross(arma::mat const& vortex) const;
	auto buildEnergyMatrix(std::vector<arma::rowvec> const& energies) const;
	void printProgress(int minID, int id, int maxID);

	int static constexpr mScales = 5;
	double const mSigma;
	int const mMinRev;
	int const mGapWidth;
	int const mMinLen;

	double static constexpr mSqrt2 = std::sqrt(2.);
	arma::colvec static const mStandardDeviations;
	arma::colvec static const mHanningWindow;
};

} // namespace detection
} // namespace haar
