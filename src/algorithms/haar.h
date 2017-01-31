#pragma once

#include <cmath>

#include "detectionAlgorithm.h"
#include "mongoDB/mongoDBDriver.h"

namespace vrtx {
namespace detection {

class HaarTransform {
public:

	HaarTransform(
		DBInstance const& db,
		int sigma,
		int revolutions,
		int maxScale=5,
		int gapWidth=15
	)
	: mDB(db)
	, mSigmaFactor(sigma)
	, mMinRev(revolutions)
	, mStdDevs(stdDev)
	, mMaxScale((0 == maxScale) ? mStdDev.size() : maxScale)
	, mGapWidth(gapWidth)
	{}

	std::vector<Vrtx> detect(arma::cube const& trajectories);

private:

	// trajectory of form MxN with M dimensions and N timesteps
	arma::mat medianHaarTransform(arma::mat trajectory);
	void extractVortices(arma::mat const& energies, std::vector<vrtx::vrtx>& vortices);

	DBInstance const& mDB;
	int mSigmaFactor;
	int mMinRev;
	int mMaxScale;
	int mGapWidth;
	std::vector<double> mStdDev; // Defines number of scales effectively

	double static constexpr mSqrt2 = std::sqrt(2.);
};

} // namespace detection
} // namespace haar
