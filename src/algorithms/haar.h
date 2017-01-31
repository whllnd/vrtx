#pragma once

#include <cmath>

#include "detectionAlgorithm.h"
#include "mongoDB/mongoDBDriver.h"

namespace vrtx {
namespace detection {

class HaarTransform : public DetectionAlgorithm {
public:

	HaarTransform(
		DBInstance const& db,
		int sigma,
		int revolutions,
		int maxScale=5,
		int gapWidth=15
	)
	: DetectionAlgorithm(db)
	, mSigmaFactor(sigma)
	, mMinRev(revolutions)
	, mMaxScale((0 == maxScale) ? mStdDev.size() : maxScale)
	, mGapWidth(gapWidth)
	{}

	std::vector<Vrtx> detect();

private:

	// trajectory of form MxN with M dimensions and N timesteps
	arma::mat medianHaarTransform(arma::mat trajectory);
	void extractVortices(arma::mat const& energies, std::vector<vrtx::vrtx>& vortices);

	int mSigmaFactor;
	int mMinRev;
	int mMaxScale;
	int mGapWidth;

	int mTimesteps;
	int mDim;

	double static constexpr mSqrt2 = std::sqrt(2.);
};

} // namespace detection
} // namespace haar
