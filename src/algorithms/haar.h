#pragma once

#include <cmath>

#include "detectionAlgorithm.h"

namespace haar {

using energies = arma::rowvec;

class haarTransformDetection : public vrtx::detectionAlgorithm {
public:

	haarTransform(int sigma, int revolutions, std::vector<double> const& stdDev, int maxScale=5, int gapWidth=15)
	: mSigmaFactor(sigma)
	, mMinRev(revolutions)
	, mStdDevs(stdDev)
	, mMaxScale((0 == maxScale) ? mStdDev.size() : maxScale)
	, mGapWidth(gapWidth)
	{}

	std::vector<vrtx::vrtx> detect(arma::cube const& trajectories);

private:

	// trajectory of form MxN with M dimensions and N timesteps
	arma::mat medianHaarTransform(arma::mat trajectory);
	void extractVortices(arma::mat const& energies, std::vector<vrtx::vrtx>& vortices);

	int mSigmaFactor;
	int mMinRev;
	int mMaxScale;
	int mGapWidth;
	std::vector<double> mStdDev; // Defines number of scales effectively

	double static constexpr mSqrt2 = std::sqrt(2.);
};

} // namespace haar
